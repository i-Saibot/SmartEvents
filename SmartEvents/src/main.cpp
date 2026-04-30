/**
* The MIT License (MIT)
*
* Copyright (c) 2026 Saibot
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#define SAMP_SDK_IMPLEMENTATION
#define SAMP_SDK_WANT_PROCESS_TICK
#define SAMP_SDK_WANT_AMX_EVENTS

#include "samp-sdk/samp_sdk.hpp"
#include "database.h"
#include "timers.h"
#include "localization.h"
#include "version.h"
#include <memory>


//------------------------------------------------------------------------------------------------------------

bool OnLoad()
{
	Samp_SDK::Log(" ");
	Samp_SDK::Log(" [SmartEvents Plugin] ------------------------------------- ");
	Samp_SDK::Log(" [SmartEvents Plugin]  Version: %s", PLUGIN_VERSION);
	Samp_SDK::Log(" [SmartEvents Plugin]  Author:  Saibot");
	Samp_SDK::Log(" [SmartEvents Plugin]  GitHub:  github.com/i-Saibot/SmartEvents");

	if (!database::init())
	{
		Samp_SDK::Log(" [SmartEvents Plugin]  Status:  FAILED (database error)");
		Samp_SDK::Log(" [SmartEvents Plugin] ------------------------------------- ");
		Samp_SDK::Log(" ");
		return false;
	}
	if (!localization::init())
	{
		Samp_SDK::Log(" [SmartEvents Plugin]  Warning: localization loaded with defaults");
	}
	Samp_SDK::Log(" [SmartEvents Plugin]  Status:  Loaded successfully");
	Samp_SDK::Log(" [SmartEvents Plugin] ------------------------------------- ");
	Samp_SDK::Log(" ");
	return true;
}

//------------------------------------------------------------------------------------------------------------

void OnUnload()
{
	Samp_SDK::Log("[SmartEvents] Plugin Unloaded");
}

//------------------------------------------------------------------------------------------------------------

unsigned int GetSupportFlags()
{
	return SUPPORTS_VERSION;
}

//------------------------------------------------------------------------------------------------------------

void OnProcessTick()
{
	duration_timers::timersTick();
}

//------------------------------------------------------------------------------------------------------------

void OnAmxLoad(AMX* amx) { }
void OnAmxUnload(AMX* amx) { }

//------------------------------------------------------------------------------------------------------------
