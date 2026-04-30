#pragma once

#include <cstdint>

namespace constants
{
	inline constexpr int32_t INVALID_SMART_EVENT	= 0;
	inline constexpr int32_t MAX_PLAYERS			= 1'000;
	inline constexpr int32_t INVALID_PLAYER_ID		= 65'535;
	inline constexpr int32_t INVALID_TIMER			= 0;
	inline constexpr int32_t MIN_TIMER_THRESHOLD	= 3'600 * 24;
	inline constexpr int32_t INVALID_CALLBACK		= 0;
	inline constexpr int32_t INVALID_UID			= 0;
};