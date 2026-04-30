#pragma once

#include "constants.h"
#include "ankerl/unordered_dense.h"

#include <cstdint>
#include <ctime>
#include <string>

//------------------------------------------------------------------------------------------------------------

struct EventData
{
	int32_t issuedAt = 0;
	int32_t expiresAt = 0;
	int32_t timerId = constants::INVALID_TIMER;
};

struct EventParams
{
	int32_t eventId = constants::INVALID_SMART_EVENT;
	int32_t timerSeconds = 0;
	int32_t issuedAt = 0;
	int32_t expiresAt = 0;
	std::function<void()> onExpire;
};

//------------------------------------------------------------------------------------------------------------

class Player
{
public:
	Player(int32_t playerId, int32_t uID);

	bool setData(EventParams& params);
	void endEvent(const int32_t eventId);
	bool removeEvent(const int32_t eventId);
	void disconnect();

	int32_t getLeftTime(const int32_t eventId);
	int32_t getID() const;
	int32_t getUID() const;
	const ankerl::unordered_dense::map<int32_t, EventData>& getMapEvents() const;
	const EventData* getEvent(int32_t eventId) const;

private:
	int32_t m_uID = constants::INVALID_UID;
	int32_t m_playerId = constants::INVALID_PLAYER_ID;

	ankerl::unordered_dense::set<int32_t> m_setTimers;
	ankerl::unordered_dense::map<int32_t, EventData> m_mapEvents;
};

//------------------------------------------------------------------------------------------------------------