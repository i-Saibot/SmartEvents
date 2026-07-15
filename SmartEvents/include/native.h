#pragma once

#define HAVE_STDINT_H
#include <samp-sdk/amx/amx.h>

namespace native
{
	cell AMX_NATIVE_CALL SE_SetLanguage(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_AddEvent(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_OnPlayerLogin(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_OnPlayerSignIn(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_SetPlayerEvent(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_RemovePlayerEvent(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_IsPlayerEventActive(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_GetPlayerLeftTime(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_SetPlayerEventUID(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_RemovePlayerEventUID(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_IsPlayerEventActiveUID(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_GetPlayerLeftTimeUID(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_GetRemainingTimeString(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_SecondsToDateString(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_MinutesToSeconds(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_HoursToSeconds(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_DaysToSeconds(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_OnPlayerDisconnect(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SE_GetVersion(AMX* amx, cell* params);
}