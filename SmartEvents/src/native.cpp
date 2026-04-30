#define SAMP_SDK_WANT_AMX_EVENTS

#include "samp-sdk/samp_sdk.hpp"
#include "player_manager.h"
#include "localization.h"
#include "utils.h"
#include "version.h"

//------------------------------------------------------------------------------------------------------------
//--------------------------------------------CORE------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_SetLanguage, AMX* amx, cell* params)
{
	std::string lang;

	Register_Parameters(lang);
	return static_cast<cell>(localization::setLanguage(lang));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_AddEvent, AMX* amx, cell* params)
{
	std::string name;
	std::string callback;
	int32_t online = 0;

	Register_Parameters(name, callback, online);
	return static_cast<cell>(player_manager::addEvent(name, callback, static_cast<bool>(online)));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_OnPlayerLogin, AMX* amx, cell* params)
{
	int32_t playerId;
	int32_t uID;

	Register_Parameters(playerId, uID);
	return static_cast<cell>(player_manager::onPlayerLogin(playerId, uID));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_OnPlayerSignIn, AMX* amx, cell* params)
{
	int32_t playerId;
	int32_t uID;

	Register_Parameters(playerId, uID);
	return static_cast<cell>(player_manager::onPlayerSignIn(playerId, uID));
}

//------------------------------------------------------------------------------------------------------------
//--------------------------------------------ONLINE----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_SetPlayerEvent, AMX* amx, cell* params)
{
	int32_t playerId;
	int32_t eventId;
	int32_t seconds;

	Register_Parameters(playerId, eventId, seconds);
	return static_cast<cell>(player_manager::setPlayerEvent(playerId, eventId, seconds));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_RemovePlayerEvent, AMX* amx, cell* params)
{
	int32_t playerId;
	int32_t eventId;

	Register_Parameters(playerId, eventId);
	return static_cast<cell>(player_manager::removePlayerEvent(playerId, eventId));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_IsPlayerEventActive, AMX* amx, cell* params)
{
	int32_t playerId;
	int32_t eventId;

	Register_Parameters(playerId, eventId);
	return static_cast<cell>(player_manager::isPlayerEventActive(playerId, eventId));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_GetPlayerLeftTime, AMX* amx, cell* params)
{
	int32_t playerId;
	int32_t eventId;

	Register_Parameters(playerId, eventId);
	return static_cast<cell>(player_manager::getLeftTime(playerId, eventId));
}

//------------------------------------------------------------------------------------------------------------
//--------------------------------------------OFFLINE---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_SetPlayerEventUID, AMX* amx, cell* params)
{
	int32_t uID;
	int32_t eventId;
	int32_t seconds;

	Register_Parameters(uID, eventId, seconds);
	return static_cast<cell>(player_manager::setPlayerEventUID(uID, eventId, seconds));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_RemovePlayerEventUID, AMX* amx, cell* params)
{
	int32_t uID;
	int32_t eventId;

	Register_Parameters(uID, eventId);
	return static_cast<cell>(player_manager::removePlayerEventUID(uID, eventId));
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_IsPlayerEventActiveUID, AMX* amx, cell* params)
{
	int32_t uID;
	int32_t eventId;

	Register_Parameters(uID, eventId);
	return static_cast<cell>(player_manager::isPlayerEventActiveUID(uID, eventId));
}

//----------------------------------------------------------------------------------------------------------

Plugin_Native(SE_GetPlayerLeftTimeUID, AMX* amx, cell* params)
{
	int32_t uID;
	int32_t eventId;

	Register_Parameters(uID, eventId);
	return static_cast<cell>(player_manager::getLeftTimeUID(uID, eventId));
}

//------------------------------------------------------------------------------------------------------------
//--------------------------------------------UTILS-----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_GetRemainingTimeString, AMX* amx, cell* params)
{
	Samp_SDK::Native_Params p(amx, params);

	const int32_t seconds = p.Get<int32_t>(0);
	std::string result = localization::getRemainingTimeString(seconds);

	if (cell* addr = Samp_SDK::amx::Get_Addr_Safe(amx, 1))
	{
		const int32_t maxLen = p.Get<int32_t>(2);
		Samp_SDK::amx::Set_String(addr, result.c_str(), maxLen);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_SecondsToDateString, AMX* amx, cell* params)
{
	Samp_SDK::Native_Params p(amx, params);

	const int32_t seconds = p.Get<int32_t>(0);
	std::string result = utils::secondsToDate(seconds);

	if (cell* addr = Samp_SDK::amx::Get_Addr_Safe(amx, 1))
	{
		const int32_t maxLen = p.Get<int32_t>(2);
		Samp_SDK::amx::Set_String(addr, result.c_str(), maxLen);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_MinutesToSeconds, AMX* amx, cell* params)
{
	int32_t minutes;

	Register_Parameters(minutes);
	return static_cast<cell>(minutes * 60);
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_HoursToSeconds, AMX* amx, cell* params)
{
	int32_t hours;

	Register_Parameters(hours);
	return static_cast<cell>(hours * 3'600);
}

//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_DaysToSeconds, AMX* amx, cell* params)
{
	int32_t days;

	Register_Parameters(days);
	return static_cast<cell>(days * 86'400);
}

//------------------------------------------------------------------------------------------------------------
//--------------------------------------------CALLBACKS-------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

Plugin_Public(OnPlayerDisconnect, int playerid, int reason)
{
	player_manager::onPlayerDisconnect(playerid);
	return PUBLIC_CONTINUE;
}

//------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
//--------------------------------------------Version---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

Plugin_Native(SE_GetVersion, AMX* amx, cell* params)
{
	if (cell* addr = Samp_SDK::amx::Get_Addr_Safe(amx, 0))
	{
		Samp_SDK::amx::Set_String(addr, PLUGIN_VERSION, sizeof(PLUGIN_VERSION));
		return 1;
	}
	return 0;
}