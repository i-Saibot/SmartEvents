#pragma once

#include <cstdint>
#include <string>

//------------------------------------------------------------------------------------------------------------

namespace utils
{
	struct TimeComponents
	{
		int32_t years;
		int32_t months;
		int32_t days;
		int32_t hours;
		int32_t minutes;
		int32_t seconds;
	};
	TimeComponents secondsToComponents(int32_t seconds);
	bool isValidUID(const int32_t uID);
	bool isValidPlayerID(const int32_t playerId);
	std::string secondsToDate(int32_t seconds);
}

//------------------------------------------------------------------------------------------------------------