#define HAVE_STDINT_H
#include "samp-sdk/amx/amx.h"

#include "player.h"
#include "timers.h"
#include "event_registry.h"

#include <ctime>

//------------------------------------------------------------------------------------------------------------

Player::Player(int32_t playerId, int32_t uID) : m_playerId(playerId), m_uID(uID) { }

//------------------------------------------------------------------------------------------------------------

bool Player::setData(EventParams& params)
{
	EventData data;
	data.issuedAt = params.issuedAt;
	data.expiresAt = params.expiresAt;
	data.timerId = constants::INVALID_TIMER;

	if (auto it = m_mapEvents.find(params.eventId); it != m_mapEvents.end())
	{
		if (it->second.timerId != constants::INVALID_TIMER)
		{
			duration_timers::killTimer(it->second.timerId);
			m_setTimers.erase(it->second.timerId);
		}
	}
	if (params.timerSeconds > 0 && params.timerSeconds < constants::MIN_TIMER_THRESHOLD)
	{
		int32_t timerId = duration_timers::setTimer(params.timerSeconds * 1'000, params.onExpire);

		if (timerId != constants::INVALID_TIMER)
		{
			m_setTimers.emplace(timerId);
			data.timerId = timerId;
		}
	}
	m_mapEvents.insert_or_assign(params.eventId, data);
	return true;
}

//------------------------------------------------------------------------------------------------------------

void Player::endEvent(const int32_t eventId)
{
	if (auto it = m_mapEvents.find(eventId); it != m_mapEvents.end())
	{
		m_setTimers.erase(it->second.timerId);
		m_mapEvents.erase(it);
	}

	event_registry::EventData callbackData = event_registry::getCallbackData(eventId);

	if (callbackData.amx != nullptr)
	{
		int index = 0;
		if (amx_FindPublic(callbackData.amx, callbackData.name.data(), &index) == AMX_ERR_NONE)
		{
			amx_Push(callbackData.amx, static_cast<cell>(m_playerId));

			cell retval = 0;
			amx_Exec(callbackData.amx, &retval, index);
		}
	}
}

//------------------------------------------------------------------------------------------------------------

bool Player::removeEvent(const int32_t eventId)
{
	auto it = m_mapEvents.find(eventId);

	if (it != m_mapEvents.end())
	{
		if (it->second.timerId != constants::INVALID_TIMER)
		{
			duration_timers::killTimer(it->second.timerId);
			m_setTimers.erase(it->second.timerId);
		}
		m_mapEvents.erase(it);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------

void Player::disconnect()
{
	for (const auto& timerId : m_setTimers)
	{
		duration_timers::killTimer(timerId);
	}
	m_setTimers.clear();
	m_mapEvents.clear();
}

//------------------------------------------------------------------------------------------------------------

int32_t Player::getLeftTime(const int32_t eventId)
{
	auto it = m_mapEvents.find(eventId);

	if (it == m_mapEvents.end())
	{
		return 0;
	}
	auto regIt = event_registry::g_mapEvents.find(eventId);

	if (regIt == event_registry::g_mapEvents.end())
	{
		return 0;
	}
	int32_t left = it->second.expiresAt - static_cast<int32_t>(std::time(nullptr));
	return std::max<int32_t>(0, left);
}

//------------------------------------------------------------------------------------------------------------

int32_t Player::getID() const
{
	return m_playerId;
}

//------------------------------------------------------------------------------------------------------------

int32_t Player::getUID() const
{
	return m_uID;
}

//------------------------------------------------------------------------------------------------------------

const ankerl::unordered_dense::map<int32_t, EventData>& Player::getMapEvents() const
{
	return m_mapEvents;
}

//------------------------------------------------------------------------------------------------------------

const EventData* Player::getEvent(int32_t eventId) const
{
	auto it = m_mapEvents.find(eventId);

	if (it == m_mapEvents.end())
	{
		return nullptr;
	}
	return &it->second;
}

//------------------------------------------------------------------------------------------------------------