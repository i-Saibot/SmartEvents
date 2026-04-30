#include "event_registry.h"

//------------------------------------------------------------------------------------------------------------

event_registry::CallbackData event_registry::getCallbackHash(const int32_t eventId)
{
	auto it = g_mapEvents.find(eventId);

	if (it == g_mapEvents.end())
	{
		return {};
	}
	return { it->second.callbackName, it->second.callbackHash };
}

//------------------------------------------------------------------------------------------------------------