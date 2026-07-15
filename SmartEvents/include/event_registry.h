#pragma once

#include "ankerl/unordered_dense.h"
#define HAVE_STDINT_H
#include "samp-sdk/amx/amx.h"

//------------------------------------------------------------------------------------------------------------

namespace event_registry
{
	struct EventData
	{
		AMX* amx = nullptr;
		std::string name;
		std::string callbackName;
		bool bOnline = true;
	};
	inline ankerl::unordered_dense::map<int32_t, EventData> g_mapEvents;
	EventData getCallbackData(const int32_t eventId);
}

//------------------------------------------------------------------------------------------------------------