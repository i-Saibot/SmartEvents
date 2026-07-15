/*
* Smart Events Plugin
* Author: Saibot
* Created: 2026
* * The MIT License (MIT)
* Copyright (c) 2026 Saibot
*/

#if defined _smart_events_included
	#endinput
#endif
#define _smart_events_included

//------------------------------------------------------------------------------

#define SE_PLUGIN_VERSION "1.0.1"

//------------------------------------------------------------------------------

const INVALID_SMART_EVENT = 0;

//------------------------------------------------------------------------------

#define SE_Event:%0(%1) forward %0(%1);  public %0(%1)

//------------------------------------------------------------------------------

//---------------Global
native bool:SE_SetLanguage(const lang[]);
native SE:SE_AddEvent(const name[], const callback[], bool:online);

//---------------Player
native bool:SE_OnPlayerLogin(playerid, uid);
native bool:SE_OnPlayerSignIn(playerid, uid);

//---------------online
native bool:SE_SetPlayerEvent(playerid, SE:eventid, seconds);
native bool:SE_RemovePlayerEvent(playerid, SE:eventid);
native bool:SE_IsPlayerEventActive(playerid, SE:eventid);
native SE_GetPlayerLeftTime(playerid, SE:eventid);

//---------------offline
native bool:SE_SetPlayerEventUID(uid, SE:eventid, seconds);
native bool:SE_RemovePlayerEventUID(uid, SE:eventid);
native bool:SE_IsPlayerEventActiveUID(uid, SE:eventid);
native SE_GetPlayerLeftTimeUID(uid, SE:eventid);

//---------------utils
native SE_GetRemainingTimeString(seconds, output[], const size = sizeof(output));
native SE_SecondsToDateString(seconds, output[], const size = sizeof(output));
native SE_MinutesToSeconds(minutes);
native SE_HoursToSeconds(hours);
native SE_DaysToSeconds(days);

//---------------Version
native SE_GetVersion(value[], const len = sizeof(value));

//------------------------------------------------------------------------------

public OnGameModeInit()
{
	new version[16];
	SE_GetVersion(version, sizeof(version));

	if (strcmp(version, SE_PLUGIN_VERSION) != 0)
	{
		printf(" ");
		printf(" [SmartEvents Plugin] ! ERROR: VERSION MISMATCH !");
		printf(" [SmartEvents Plugin] --------------------------------------------------");
		printf(" [SmartEvents Plugin]  Plugin binary:  v.%s", version);
		printf(" [SmartEvents Plugin]  Include file:   v.%s", SE_PLUGIN_VERSION);
		printf(" ");
		printf(" [SmartEvents Plugin]  Action: Update your .inc file or the plugin binary.");
		printf(" [SmartEvents Plugin]  Download: github.com/i-Saibot/SmartEvents/releases");
		printf(" [SmartEvents Plugin] --------------------------------------------------");
		printf(" ");
	}
	#if defined smartevents_OnGameModeInit
		return smartevents_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit smartevents_OnGameModeInit
#if defined smartevents_OnGameModeInit
	forward smartevents_OnGameModeInit();
#endif

native SE_OnPlayerDisconnect(playerid);

public OnPlayerDisconnect(playerid, reason)
{
	SE_OnPlayerDisconnect(playerid);
#if defined smartevents_OnPlayerDisconnect
	return smartevents_OnPlayerDisconnect(playerid, reason);
#else
	return 1;
#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect smartevents_OnPlayerDisconnect
#if defined    smartevents_OnPlayerDisconnect
	forward smartevents_OnPlayerDisconnect(playerid, reason);
#endif
