#include "se_log.h"
#include <iomanip>
#include <fstream>
#include <chrono>
#include <cstdarg>

//----------------------------------------------------------------------------------------------------------------------------

namespace se_log
{
	void logSE(const char* format, ...)
	{
		char buffer[512];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm tm_info;

#if defined(_WIN32)
		localtime_s(&tm_info, &now);
#else
		localtime_r(&now, &tm_info);
#endif

		std::ofstream logFile("SmartEvents.log", std::ios::app);

		if (logFile.is_open())
		{
			logFile << "[" << std::put_time(&tm_info, "%d/%m/%Y %H:%M:%S") << "] ";

			logFile << buffer << std::endl;
			logFile.close();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------