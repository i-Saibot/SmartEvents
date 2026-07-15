#define HAVE_STDINT_H
#include "samp-sdk/amx/amx.h"
#include "player_manager.h"
#include "event_registry.h"
#include "database.h"
#include "utils.h"
#include "se_log.h"

#include <ctime>
#include <algorithm>

//------------------------------------------------------------------------------------------------------------

int32_t player_manager::addEvent(AMX* amx, std::string_view name, std::string_view callback, const bool bOnline)
{
	try
	{
		auto buildData = [&](bool online) -> event_registry::EventData
		{
			event_registry::EventData data;
			data.name = std::string(name);
			data.bOnline = online;
			data.callbackName = std::string(callback);
			data.amx = amx;
			return data;
		};
		SQLite::Statement select(
			*database::db,
			"SELECT "
				"event_id, "
				"online "
			"FROM se_events "
			"WHERE event_name = ?"
		);
		select.bind(1, name.data());

		if (select.executeStep())
		{
			int32_t eventId = select.getColumn("event_id").getInt();
			bool online = static_cast<bool>(select.getColumn("online").getInt());

			event_registry::g_mapEvents.emplace(eventId, buildData(online));
			return eventId;
		}
		SQLite::Statement queryInsert(
			*database::db,
			"INSERT INTO se_events "
				"(event_name, online) VALUES (?, ?)"
		);
		queryInsert.bind(1, name.data());
		queryInsert.bind(2, static_cast<int>(bOnline));
		queryInsert.exec();

		int32_t newId = static_cast<int32_t>(database::db->getLastInsertRowid());

		event_registry::g_mapEvents.emplace(newId, buildData(bOnline));
		return newId;
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_AddEvent Error ('%s'): %s", name.data(), e.what());
	}
	return constants::INVALID_SMART_EVENT;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::onPlayerLogin(const int32_t playerId, const int32_t uID)
{
	if (!utils::isValidUID(uID) || !utils::isValidPlayerID(playerId))
	{
		return false;
	}
	m_players[playerId] = std::make_unique<Player>(playerId, uID);
	m_mapUIDs.emplace(uID, playerId);

	try
	{
		Player* player = m_players[playerId].get();
		int32_t currentTime = static_cast<int32_t>(std::time(nullptr));

		SQLite::Statement querySelect(
			*database::db,
			"SELECT "
				"* "
			"FROM se_data "
			"WHERE u_id = ?"
		);
		querySelect.bind(1, uID);

		SQLite::Statement querySaveTime(
			*database::db,
			"INSERT OR IGNORE INTO se_save_time "
				"(u_id, event_id, trigger_unixtime) VALUES (?, ?, ?)"
		);
		querySaveTime.bind(1, uID);
		querySaveTime.bind(3, currentTime);

		SQLite::Transaction transaction(*database::db);

		while (querySelect.executeStep())
		{
			int32_t eventId = querySelect.getColumn("event_id").getInt();
			int32_t issuedAt = querySelect.getColumn("issued_at").getInt();
			int32_t expiresAt = querySelect.getColumn("expires_at").getInt();

			auto it = event_registry::g_mapEvents.find(eventId);

			if (it == event_registry::g_mapEvents.end())
			{
				continue;
			}
			if (expiresAt == 0)
			{
				player->endEvent(eventId);
				database::stmtDeleteData->reset();
				database::stmtDeleteData->bind(1, uID);
				database::stmtDeleteData->bind(2, eventId);
				database::stmtDeleteData->exec();
				continue;
			}
			int32_t timerSeconds = 0;

			if (it->second.bOnline)
			{
				querySaveTime.bind(2, eventId);
				querySaveTime.exec();
				querySaveTime.reset();

				expiresAt = currentTime + expiresAt;
				timerSeconds = expiresAt;
			}
			else
			{
				int32_t leftSeconds = expiresAt - currentTime;

				if (leftSeconds <= 0)
				{
					player->endEvent(eventId);
					database::stmtDeleteData->reset();
					database::stmtDeleteData->bind(1, uID);
					database::stmtDeleteData->bind(2, eventId);
					database::stmtDeleteData->exec();
					continue;
				}
				timerSeconds = leftSeconds;
			}
			EventParams params;

			params.eventId = eventId;
			params.timerSeconds = timerSeconds;
			params.issuedAt = issuedAt;
			params.expiresAt = expiresAt;
			params.onExpire = [playerId, eventId]()
				{
					endPlayerEvent(playerId, eventId);
				};
			player->setData(params);
		}
		transaction.commit();
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_OnPlayerLogin Error: %s", e.what());
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::onPlayerSignIn(const int32_t playerId, const int32_t uID)
{
	if (!utils::isValidUID(uID) || !utils::isValidPlayerID(playerId))
	{
		return false;
	}
	m_players[playerId] = std::make_unique<Player>(playerId, uID);
	m_mapUIDs.emplace(uID, playerId);
	return true;
}

//------------------------------------------------------------------------------------------------------------

void player_manager::endPlayerEvent(const int32_t playerId, const int32_t eventId)
{
	Player* player = m_players[playerId].get();

	if (!player)
	{
		return;
	}
	int32_t uID = player->getUID();
	player->endEvent(eventId);

	try
	{
		SQLite::Statement query(
			*database::db,
			"DELETE FROM se_data "
				"WHERE u_id = ? AND event_id = ?"
		);
		query.bind(1, uID);
		query.bind(2, eventId);
		query.exec();
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] endPlayerEvent Error: %s", e.what());
	}
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::setPlayerEvent(const int32_t playerId, const int32_t eventId, const int32_t seconds)
{
	if (!utils::isValidPlayerID(playerId))
	{
		return false;
	}
	Player* player = m_players[playerId].get();

	if (!player)
	{
		return false;
	}
	int32_t uID = player->getUID();

	if (!utils::isValidUID(uID))
	{
		return false;
	}
	auto it = event_registry::g_mapEvents.find(eventId);

	if (it == event_registry::g_mapEvents.end())
	{
		return false;
	}
	bool isOnlineEvent = it->second.bOnline;
	int32_t currentTime = static_cast<int32_t>(std::time(nullptr));
	int32_t expiresAt = currentTime + seconds;
	int32_t expiresAtSaved = isOnlineEvent ? seconds : expiresAt;

	EventParams params;
	params.eventId = eventId;
	params.timerSeconds = seconds;
	params.issuedAt = currentTime;
	params.expiresAt = expiresAt;
	params.onExpire = [playerId, eventId]()
		{
			endPlayerEvent(playerId, eventId);
		};
	player->setData(params);

	try
	{
		SQLite::Transaction transaction(*database::db);

		database::stmtInsertData->reset();
		database::stmtInsertData->bind(1, uID);
		database::stmtInsertData->bind(2, eventId);
		database::stmtInsertData->bind(3, currentTime);
		database::stmtInsertData->bind(4, expiresAtSaved);
		database::stmtInsertData->exec();

		if (isOnlineEvent)
		{
			database::stmtInsertSaveTime->reset();
			database::stmtInsertSaveTime->bind(1, uID);
			database::stmtInsertSaveTime->bind(2, eventId);
			database::stmtInsertSaveTime->bind(3, currentTime);
			database::stmtInsertSaveTime->exec();
		}
		transaction.commit();
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_SetPlayerEvent Error: %s", e.what());
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::removePlayerEvent(const int32_t playerId, const int32_t eventId)
{
	if (!utils::isValidPlayerID(playerId)) return false;

	Player* player = m_players[playerId].get();

	if (!player)
	{
		return false;
	}
	int32_t uID = player->getUID();

	if (!utils::isValidUID(uID))
	{
		return false;
	}
	if (!player->removeEvent(eventId))
	{
		return false;
	}
	try
	{
		SQLite::Transaction transaction(*database::db);

		database::stmtDeleteData->reset();
		database::stmtDeleteData->bind(1, uID);
		database::stmtDeleteData->bind(2, eventId);
		database::stmtDeleteData->exec();

		database::stmtDeleteSaveTime->reset();
		database::stmtDeleteSaveTime->bind(1, uID);
		database::stmtDeleteSaveTime->bind(2, eventId);
		database::stmtDeleteSaveTime->exec();

		transaction.commit();
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_RemovePlayerEvent Error: %s", e.what());
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::isPlayerEventActive(const int32_t playerId, const int32_t eventId)
{
	if (!utils::isValidPlayerID(playerId))
	{
		return false;
	}
	Player* player = m_players[playerId].get();

	if (!player)
	{
		return false;
	}
	return player->getEvent(eventId) != nullptr;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::setPlayerEventUID(const int32_t uID, const int32_t eventId, const int32_t seconds)
{
	if (!utils::isValidUID(uID))
	{
		return false;
	}
	auto it = event_registry::g_mapEvents.find(eventId);

	if (it == event_registry::g_mapEvents.end())
	{
		return false;
	}
	bool isOnlineEvent = it->second.bOnline;
	int32_t currentTime = static_cast<int32_t>(std::time(nullptr));
	int32_t expiresAt = isOnlineEvent ? seconds : currentTime + seconds;

	try
	{
		SQLite::Statement queryInsertData(
			*database::db,
			"INSERT OR REPLACE INTO se_data "
				"(u_id, event_id, issued_at, expires_at) VALUES (?, ?, ?, ?)"
		);
		queryInsertData.bind(1, uID);
		queryInsertData.bind(2, eventId);
		queryInsertData.bind(3, currentTime);
		queryInsertData.bind(4, expiresAt);
		queryInsertData.exec();

		auto itUID = m_mapUIDs.find(uID);

		if (itUID != m_mapUIDs.end())
		{
			int32_t playerId = itUID->second;

			if (!utils::isValidPlayerID(playerId))
			{
				return false;
			}
			Player* player = m_players[playerId].get();

			if (!player)
			{
				return false;
			}
			expiresAt = currentTime + seconds;

			EventParams params;
			params.eventId = eventId;
			params.timerSeconds = seconds;
			params.issuedAt = currentTime;
			params.expiresAt = expiresAt;
			params.onExpire = [playerId, eventId]() { endPlayerEvent(playerId, eventId); };

			player->setData(params);

			if (isOnlineEvent)
			{
				SQLite::Statement queryInsertSaveTime(
					*database::db,
					"INSERT OR IGNORE INTO se_save_time "
						"(u_id, event_id, trigger_unixtime) VALUES (?, ?, ?)"
				);
				queryInsertSaveTime.bind(1, uID);
				queryInsertSaveTime.bind(2, eventId);
				queryInsertSaveTime.bind(3, currentTime);
				queryInsertSaveTime.exec();
			}
		}
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_SetPlayerEventUID Error: %s", e.what());
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::removePlayerEventUID(const int32_t uID, const int32_t eventId)
{
	if (!utils::isValidUID(uID))
	{
		return false;
	}
	try
	{
		SQLite::Transaction transaction(*database::db);

		database::stmtDeleteData->reset();
		database::stmtDeleteData->bind(1, uID);
		database::stmtDeleteData->bind(2, eventId);
		database::stmtDeleteData->exec();

		database::stmtDeleteSaveTime->reset();
		database::stmtDeleteSaveTime->bind(1, uID);
		database::stmtDeleteSaveTime->bind(2, eventId);
		database::stmtDeleteSaveTime->exec();

		transaction.commit();

		auto itUID = m_mapUIDs.find(uID);
		if (itUID != m_mapUIDs.end())
		{
			int32_t pid = itUID->second;
			if (utils::isValidPlayerID(pid))
			{
				if (Player* p = m_players[pid].get())
				{
					p->removeEvent(eventId);
				}
			}
		}
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_RemovePlayerEventUID Error: %s", e.what());
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::isPlayerEventActiveUID(const int32_t uID, const int32_t eventId)
{
	if (!utils::isValidUID(uID))
	{
		return false;
	}
	auto eventIt = event_registry::g_mapEvents.find(eventId);

	if (eventIt == event_registry::g_mapEvents.end())
	{
		return false;
	}
	auto uidIt = m_mapUIDs.find(uID);

	if (uidIt != m_mapUIDs.end())
	{
		if (Player* player = m_players[uidIt->second].get())
		{
			return player->getEvent(eventId) != nullptr;
		}
	}
	try
	{
		database::stmtSelectExpires->reset();
		database::stmtSelectExpires->bind(1, uID);
		database::stmtSelectExpires->bind(2, eventId);

		if (!database::stmtSelectExpires->executeStep())
		{
			return false;
		}
		int32_t expiresAt = database::stmtSelectExpires->getColumn("expires_at").getInt();

		if (!eventIt->second.bOnline)
		{
			return expiresAt > static_cast<int32_t>(std::time(nullptr));
		}
		return expiresAt != 0;
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_IsPlayerEventActiveUID Error: %s", e.what());
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------

bool player_manager::onPlayerDisconnect(const int32_t playerId)
{
	if (!utils::isValidPlayerID(playerId))
	{
		return false;
	}
	Player* player = m_players[playerId].get();

	if (!player || player->getUID() == constants::INVALID_UID)
	{
		return false;
	}
	int32_t uID = player->getUID();
	int32_t currentTime = static_cast<int32_t>(std::time(nullptr));

	try
	{
		SQLite::Transaction transaction(*database::db);

		for (auto& [eventId, data] : player->getMapEvents())
		{
			auto regIt = event_registry::g_mapEvents.find(eventId);

			if (regIt == event_registry::g_mapEvents.end() || !regIt->second.bOnline)
			{
				continue;
			}
			int32_t expiresAt = std::max<int32_t>(0, data.expiresAt - currentTime);

			database::stmtUpdateData->reset();
			database::stmtUpdateData->bind(1, expiresAt);
			database::stmtUpdateData->bind(2, uID);
			database::stmtUpdateData->bind(3, eventId);
			database::stmtUpdateData->exec();
		}
		SQLite::Statement deleteSaveTime(
			*database::db,
			"DELETE FROM se_save_time WHERE u_id = ?"
		);
		deleteSaveTime.bind(1, uID);
		deleteSaveTime.exec();

		transaction.commit();
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] OnPlayerDisconnect Error: %s", e.what());
	}
	player->disconnect();
	m_mapUIDs.erase(uID);
	m_players[playerId].reset();
	return true;
}

//------------------------------------------------------------------------------------------------------------

int32_t player_manager::getLeftTime(const int32_t playerId, const int32_t eventId)
{
	if (!utils::isValidPlayerID(playerId))
	{
		return 0;
	}
	Player* player = player_manager::m_players[playerId].get();

	if (!player)
	{
		return 0;
	}
	return player->getLeftTime(eventId);
}

//------------------------------------------------------------------------------------------------------------

int32_t player_manager::getLeftTimeUID(const int32_t uID, const int32_t eventId)
{
	if (!utils::isValidUID(uID))
	{
		return 0;
	}
	auto it = event_registry::g_mapEvents.find(eventId);

	if (it == event_registry::g_mapEvents.end())
	{
		return 0;
	}
	int32_t currentTime = static_cast<int32_t>(std::time(nullptr));
	auto uidIt = m_mapUIDs.find(uID);

	if (uidIt != m_mapUIDs.end())
	{
		if (Player* player = m_players[uidIt->second].get())
		{
			const EventData* eventData = player->getEvent(eventId);

			if (!eventData)
			{
				return 0;
			}
			return std::max<int32_t>(0, eventData->expiresAt - currentTime);
		}
	}
	try
	{
		database::stmtSelectExpires->reset();
		database::stmtSelectExpires->bind(1, uID);
		database::stmtSelectExpires->bind(2, eventId);

		if (!database::stmtSelectExpires->executeStep())
		{
			return 0;
		}
		int32_t expiresAt = database::stmtSelectExpires->getColumn("expires_at").getInt();

		if (!it->second.bOnline)
		{
			return std::max<int32_t>(0, expiresAt - currentTime);
		}
		return expiresAt;
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SE_GetLeftTimeUID Error: %s", e.what());
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------