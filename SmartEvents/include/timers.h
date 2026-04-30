#pragma once

#include "constants.h"
#include "ankerl/unordered_dense.h"

#include <chrono>
#include <string>
#include <functional>

//------------------------------------------------------------------------------------------------------------

namespace duration_timers
{
	using clock = std::chrono::steady_clock;

	struct TimersData
	{
		clock::time_point expire;
		std::function<void()> onExpire;
	};
	inline int32_t g_timersIds = 0;
	inline ankerl::unordered_dense::map<int32_t, TimersData> g_mapTimers;

	int32_t setTimer(const int32_t ms, std::function<void()> onExpire);
	bool killTimer(const int32_t timerId);
	void timersTick();
}

//------------------------------------------------------------------------------------------------------------
