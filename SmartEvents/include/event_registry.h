#pragma once

#include "ankerl/unordered_dense.h"

//------------------------------------------------------------------------------------------------------------

namespace event_registry
{
	struct EventData
	{
		std::string name;
		std::string callbackName;
		uint32_t callbackHash = 0;
		bool bOnline = true;
	};
	struct CallbackData
	{
		std::string name;
		uint32_t hash = 0;
	};
	inline ankerl::unordered_dense::map<int32_t, EventData> g_mapEvents;
	CallbackData getCallbackHash(const int32_t eventId);
}

//------------------------------------------------------------------------------------------------------------