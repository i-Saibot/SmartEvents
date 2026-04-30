#include "utils.h"
#include "constants.h"
#include <ctime>

//------------------------------------------------------------------------------------------------------------

utils::TimeComponents utils::secondsToComponents(int32_t seconds)
{
	TimeComponents c{};
	c.years		= seconds / (365 * 24 * 3600);	seconds %= (365 * 24 * 3600);
	c.months	= seconds / (30 * 24 * 3600);	seconds %= (30 * 24 * 3600);
	c.days		= seconds / (24 * 3600);		seconds %= (24 * 3600);
	c.hours		= seconds / 3600;				seconds %= 3600;
	c.minutes	= seconds / 60;					seconds %= 60;
	c.seconds	= seconds;
	return c;
}

//------------------------------------------------------------------------------------------------------------

bool utils::isValidUID(const int32_t uID)
{
	return (uID > 0);
}

//------------------------------------------------------------------------------------------------------------

bool utils::isValidPlayerID(const int32_t playerId)
{
	return playerId >= 0 && playerId < constants::MAX_PLAYERS;
}

//------------------------------------------------------------------------------------------------------------

std::string utils::secondsToDate(int32_t seconds)
{
	time_t unixTime = std::time(nullptr) + seconds;

#ifdef _WIN32
	struct tm tm_info;
	localtime_s(&tm_info, &unixTime);
	char buffer[32];
	strftime(buffer, sizeof(buffer), "%d.%m.%Y - %H:%M:%S", &tm_info);
#else
	struct tm tm_info;
	localtime_r(&unixTime, &tm_info);
	char buffer[32];
	strftime(buffer, sizeof(buffer), "%d.%m.%Y - %H:%M:%S", &tm_info);
#endif

	return std::string(buffer);
}


//------------------------------------------------------------------------------------------------------------