#include "event_registry.h"

//------------------------------------------------------------------------------------------------------------

event_registry::EventData event_registry::getCallbackData(const int32_t eventId)
{
	auto it = g_mapEvents.find(eventId);

	if (it == g_mapEvents.end())
	{
		return {};
	}
	return { it->second.amx, it->second.callbackName };
}

//------------------------------------------------------------------------------------------------------------