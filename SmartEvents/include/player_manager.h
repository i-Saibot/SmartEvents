#pragma once

#include "player.h"
#include "constants.h"
#include "ankerl/unordered_dense.h"

#include <memory>
#include <string_view>

//------------------------------------------------------------------------------------------------------------

namespace player_manager
{
	inline std::unique_ptr<Player> m_players[constants::MAX_PLAYERS];
	inline ankerl::unordered_dense::map<int32_t, int32_t> m_mapUIDs;

	int32_t addEvent(AMX* amx, std::string_view name, std::string_view callback, const bool bOnline);
	bool onPlayerLogin(const int32_t playerId, const int32_t uID);
	bool onPlayerSignIn(const int32_t playerId, const int32_t uID);
	void endPlayerEvent(const int32_t playerId, const int32_t eventId);

	bool setPlayerEvent(const int32_t playerId, const int32_t eventId, const int32_t seconds);
	bool removePlayerEvent(const int32_t playerId, const int32_t eventId);
	bool isPlayerEventActive(const int32_t playerId, const int32_t eventId);

	bool setPlayerEventUID(const int32_t uID, const int32_t eventId, const int32_t seconds);
	bool removePlayerEventUID(const int32_t uID, const int32_t eventId);
	bool isPlayerEventActiveUID(const int32_t uID, const int32_t eventId);
	
	bool onPlayerDisconnect(const int32_t playerId);

	int32_t getLeftTime(const int32_t playerId, const int32_t eventId);
	int32_t getLeftTimeUID(const int32_t uID, const int32_t eventId);
}

//------------------------------------------------------------------------------------------------------------