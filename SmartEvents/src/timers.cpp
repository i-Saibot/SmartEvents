#include "timers.h"

//------------------------------------------------------------------------------------------------------------

int32_t duration_timers::setTimer(const int32_t ms, std::function<void()> onExpire)
{
	if (ms <= 0 || !onExpire)
	{
		return constants::INVALID_TIMER;
	}
	TimersData data;
	data.expire = clock::now() + std::chrono::milliseconds(ms);
	data.onExpire = std::move(onExpire);

	if (auto [it, inserted] = g_mapTimers.emplace(++g_timersIds, std::move(data)); inserted)
	{
		return g_timersIds;
	}
	return constants::INVALID_TIMER;
}

//------------------------------------------------------------------------------------------------------------

bool duration_timers::killTimer(const int32_t timerId)
{
	if (auto it = g_mapTimers.find(timerId); it != g_mapTimers.end())
	{
		g_mapTimers.erase(it);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------

void duration_timers::timersTick()
{
	if (g_mapTimers.empty())
	{
		return;
	}
	auto currentTime = clock::now();

	for (auto it = g_mapTimers.begin(); it != g_mapTimers.end();)
	{
		if (currentTime < it->second.expire)
		{
			++it;
			continue;
		}
		it->second.onExpire();
		it = g_mapTimers.erase(it);
	}
}

//------------------------------------------------------------------------------------------------------------