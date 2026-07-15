#include "native.h"
#include "player_manager.h"
#include "localization.h"
#include "utils.h"
#include "version.h"
#include "common.h"

//------------------------------------------------------------------------------------------------------------

namespace native
{
	cell AMX_NATIVE_CALL SE_SetLanguage(AMX* amx, cell* params)
	{
		char* lang = nullptr;
		amx_StrParam(amx, params[1], lang);

		if (lang == nullptr)
		{
			return 0;
		}
		return static_cast<cell>(localization::setLanguage(std::string(lang)));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_AddEvent(AMX* amx, cell* params)
	{
		char* name = nullptr;
		char* callback = nullptr;

		amx_StrParam(amx, params[1], name);
		amx_StrParam(amx, params[2], callback);

		if (name == nullptr || callback == nullptr)
		{
			return 0;
		}
		int32_t online = static_cast<int32_t>(params[3]);

		return static_cast<cell>(player_manager::addEvent(
			amx,
			std::string(name),
			std::string(callback),
			static_cast<bool>(online))
		);
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_OnPlayerLogin(AMX* amx, cell* params)
	{
		int32_t playerId = static_cast<int32_t>(params[1]);
		int32_t uID = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::onPlayerLogin(playerId, uID));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_OnPlayerSignIn(AMX* amx, cell* params)
	{
		int32_t playerId = static_cast<int32_t>(params[1]);
		int32_t uID = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::onPlayerSignIn(playerId, uID));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_SetPlayerEvent(AMX* amx, cell* params)
	{
		int32_t playerId = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);
		int32_t seconds = static_cast<int32_t>(params[3]);


		return static_cast<cell>(player_manager::setPlayerEvent(playerId, eventId, seconds));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_RemovePlayerEvent(AMX* amx, cell* params)
	{
		int32_t playerId = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::removePlayerEvent(playerId, eventId));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_IsPlayerEventActive(AMX* amx, cell* params)
	{
		int32_t playerId = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::isPlayerEventActive(playerId, eventId));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_GetPlayerLeftTime(AMX* amx, cell* params)
	{
		int32_t playerId = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::getLeftTime(playerId, eventId));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_SetPlayerEventUID(AMX* amx, cell* params)
	{
		int32_t uID = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);
		int32_t seconds = static_cast<int32_t>(params[3]);

		return static_cast<cell>(player_manager::setPlayerEventUID(uID, eventId, seconds));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_RemovePlayerEventUID(AMX* amx, cell* params)
	{
		int32_t uID = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::removePlayerEventUID(uID, eventId));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_IsPlayerEventActiveUID(AMX* amx, cell* params)
	{
		int32_t uID = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::isPlayerEventActiveUID(uID, eventId));
	}

//----------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_GetPlayerLeftTimeUID(AMX* amx, cell* params)
	{
		int32_t uID = static_cast<int32_t>(params[1]);
		int32_t eventId = static_cast<int32_t>(params[2]);

		return static_cast<cell>(player_manager::getLeftTimeUID(uID, eventId));
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_GetRemainingTimeString(AMX* amx, cell* params)
	{
		const int32_t seconds = static_cast<int32_t>(params[1]);
		std::string result = localization::getRemainingTimeString(seconds);

		cell* addr = nullptr;
		if (amx_GetAddr(amx, params[2], &addr) == AMX_ERR_NONE && addr != nullptr)
		{
			const int32_t maxLen = static_cast<int32_t>(params[3]);
			amx_SetString(addr, result.c_str(), 0, 0, maxLen);
			return 1;
		}
		return 0;
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_SecondsToDateString(AMX* amx, cell* params)
	{
		const int32_t seconds = static_cast<int32_t>(params[1]);
		std::string result = utils::secondsToDate(seconds);

		cell* addr = nullptr;
		if (amx_GetAddr(amx, params[2], &addr) == AMX_ERR_NONE && addr != nullptr)
		{
			const int32_t maxLen = static_cast<int32_t>(params[3]);
			amx_SetString(addr, result.c_str(), 0, 0, maxLen);
			return 1;
		}
		return 0;
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_MinutesToSeconds(AMX* amx, cell* params)
	{
		int32_t minutes = static_cast<int32_t>(params[1]);

		return static_cast<cell>(minutes * 60);
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_HoursToSeconds(AMX* amx, cell* params)
	{
		int32_t hours = static_cast<int32_t>(params[1]);

		return static_cast<cell>(hours * 3'600);
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_DaysToSeconds(AMX* amx, cell* params)
	{
		int32_t days = static_cast<int32_t>(params[1]);

		return static_cast<cell>(days * 86'400);
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_OnPlayerDisconnect(AMX* amx, cell* params)
	{
		int32_t playerid = static_cast<int32_t>(params[1]);

		player_manager::onPlayerDisconnect(playerid);
		return 1;
	}

//------------------------------------------------------------------------------------------------------------

	cell AMX_NATIVE_CALL SE_GetVersion(AMX* amx, cell* params)
	{
		cell* addr = nullptr;
		if (amx_GetAddr(amx, params[1], &addr) == AMX_ERR_NONE && addr != nullptr)
		{
			amx_SetString(addr, PLUGIN_VERSION, 0, 0, sizeof(PLUGIN_VERSION));
			return 1;
		}
		return 0;
	}
}
//------------------------------------------------------------------------------------------------------------