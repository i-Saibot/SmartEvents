/**
* The MIT License (MIT)
*
* Copyright (c) 2026 Saibot
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#define HAVE_STDINT_H
#include "samp-sdk/amx/amx.h"
#include "samp-sdk/plugincommon.h"

#include "database.h"
#include "timers.h"
#include "localization.h"
#include "version.h"
#include "common.h"
#include <memory>
#include "native.h"

//------------------------------------------------------------------------------------------------------------

logprintf_t logprintf = nullptr;

//------------------------------------------------------------------------------------------------------------

AMX_NATIVE_INFO Natives[] =
{
	{ "SE_SetLanguage",				native::SE_SetLanguage },
	{ "SE_AddEvent",				native::SE_AddEvent },
	{ "SE_OnPlayerLogin",			native::SE_OnPlayerLogin },
	{ "SE_OnPlayerSignIn",			native::SE_OnPlayerSignIn },
	{ "SE_SetPlayerEvent",			native::SE_SetPlayerEvent },
	{ "SE_RemovePlayerEvent",		native::SE_RemovePlayerEvent },
	{ "SE_IsPlayerEventActive",		native::SE_IsPlayerEventActive },
	{ "SE_GetPlayerLeftTime",		native::SE_GetPlayerLeftTime },
	{ "SE_SetPlayerEventUID",		native::SE_SetPlayerEventUID },
	{ "SE_RemovePlayerEventUID",	native::SE_RemovePlayerEventUID },
	{ "SE_IsPlayerEventActiveUID",	native::SE_IsPlayerEventActiveUID },
	{ "SE_GetPlayerLeftTimeUID",	native::SE_GetPlayerLeftTimeUID },
	{ "SE_GetRemainingTimeString",	native::SE_GetRemainingTimeString },
	{ "SE_SecondsToDateString",		native::SE_SecondsToDateString },
	{ "SE_MinutesToSeconds",		native::SE_MinutesToSeconds },
	{ "SE_HoursToSeconds",			native::SE_HoursToSeconds },
	{ "SE_DaysToSeconds",			native::SE_DaysToSeconds },
	{ "SE_OnPlayerDisconnect",		native::SE_OnPlayerDisconnect },
	{ "SE_GetVersion",				native::SE_GetVersion },
	{ 0, 0 }
};

//------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

//------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	logprintf(" ");
	logprintf(" [SmartEvents Plugin] ------------------------------------- ");
	logprintf(" [SmartEvents Plugin]  Version: %s", PLUGIN_VERSION);
	logprintf(" [SmartEvents Plugin]  Author:  Saibot");
	logprintf(" [SmartEvents Plugin]  GitHub:  github.com/i-Saibot/SmartEvents");

	if (!database::init())
	{
		logprintf(" [SmartEvents Plugin]  Status:  FAILED (database error)");
		logprintf(" [SmartEvents Plugin] ------------------------------------- ");
		logprintf(" ");
		return false;
	}
	if (!localization::init())
	{
		logprintf(" [SmartEvents Plugin]  Warning: localization loaded with defaults");
	}
	logprintf(" [SmartEvents Plugin]  Status:  Loaded successfully");
	logprintf(" [SmartEvents Plugin] ------------------------------------- ");
	logprintf(" ");
	return true;
}

//------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	logprintf("[SmartEvents] Plugin Unloaded");
}

//------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
	return amx_Register(amx, Natives, -1);
}

//------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
	return AMX_ERR_NONE;
}

//------------------------------------------------------------------------------------------------------------

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	duration_timers::timersTick();
}

//------------------------------------------------------------------------------------------------------------