#pragma once

#include "ankerl/unordered_dense.h"

#include <string>
#include <array>
#include <string_view>
#include <cstdint>

//------------------------------------------------------------------------------------------------------------

namespace localization
{
	const std::string PATH = "scriptfiles/SmartEventsLanguage.ini";

	struct WordForms
	{
		std::string one;
		std::string few;
		std::string many;
	};
	struct LangData
	{
		WordForms second;
		WordForms minute;
		WordForms hour;
		WordForms day;
		WordForms month;
		WordForms year;
	};
	enum class Encoding
	{
		UTF8,
		Windows1251
	};
	inline ankerl::unordered_dense::map<std::string, LangData> g_languages;
	inline std::string g_currentLang = "en";
	inline Encoding g_encoding = Encoding::UTF8;

	bool init();
	bool setLanguage(std::string_view lang);
	bool setEncoding(std::string_view enc);
	std::string getRemainingTimeString(int32_t seconds);
	
}

//------------------------------------------------------------------------------------------------------------