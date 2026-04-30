#include "localization.h"
#include "utils.h"
#include "se_log.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <langinfo.h>
#endif

//------------------------------------------------------------------------------------------------------------

static const std::string DEFAULT_INI =
	"[en]\n"
	"second=second|seconds|seconds\n"
	"minute=minute|minutes|minutes\n"
	"hour=hour|hours|hours\n"
	"day=day|days|days\n"
	"month=month|months|months\n"
	"year=year|years|years\n"
	"\n"
	"[ru]\n"
	"second=секунда|секунды|секунд\n"
	"minute=минута|минуты|минут\n"
	"hour=час|часа|часов\n"
	"day=день|дня|дней\n"
	"month=месяц|месяца|месяцев\n"
	"year=год|года|лет\n"
	"\n"
	"[uk]\n"
	"second=секунда|секунди|секунд\n"
	"minute=хвилина|хвилини|хвилин\n"
	"hour=година|години|годин\n"
	"day=день|дні|днів\n"
	"month=місяць|місяці|місяців\n"
	"year=рік|роки|років\n"
	"\n"
	"[by]\n"
	"second=секунда|секунды|секунд\n"
	"minute=хвіліна|хвіліны|хвілін\n"
	"hour=гадзіна|гадзіны|гадзін\n"
	"day=дзень|дні|дзён\n"
	"month=месяц|месяцы|месяцаў\n"
	"year=год|гады|гадоў\n"
	"\n"
	"[pt]\n"
	"second=segundo|segundos|segundos\n"
	"minute=minuto|minutos|minutos\n"
	"hour=hora|horas|horas\n"
	"day=dia|dias|dias\n"
	"month=mês|meses|meses\n"
	"year=ano|anos|anos\n"
	"\n"
	"[es]\n"
	"second=segundo|segundos|segundos\n"
	"minute=minuto|minutos|minutos\n"
	"hour=hora|horas|horas\n"
	"day=día|días|días\n"
	"month=mes|meses|meses\n"
	"year=año|años|años\n";

//------------------------------------------------------------------------------------------------------------

static localization::WordForms parseWordForms(const std::string& value)
{
	localization::WordForms forms;

	std::stringstream ss(value);
	std::string token;
	std::array<std::string*, 3> parts = { &forms.one, &forms.few, &forms.many };
	int i = 0;

	while (std::getline(ss, token, '|') && i < 3)
	{
		*parts[i++] = token;
	}
	return forms;
}

//------------------------------------------------------------------------------------------------------------

static void parseINI(const std::string& content)
{
	std::istringstream stream(content);
	std::string line;
	std::string currentLang;
	localization::LangData currentData;

	auto saveCurrent = [&]()
		{
			if (!currentLang.empty())
			{
				localization::g_languages.emplace(currentLang, currentData);
				currentData = localization::LangData{};
			}
		};
	while (std::getline(stream, line))
	{
		if (line.empty() || line[0] == ';')
		{
			continue;
		}
		if (line[0] == '[')
		{
			saveCurrent();
			currentLang = line.substr(1, line.find(']') - 1);
			continue;
		}
		auto pos = line.find('=');

		if (pos == std::string::npos)
		{
			continue;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		if (key == "second")		currentData.second = parseWordForms(value);
		else if (key == "minute")	currentData.minute = parseWordForms(value);
		else if (key == "hour")		currentData.hour = parseWordForms(value);
		else if (key == "day")		currentData.day = parseWordForms(value);
		else if (key == "month")	currentData.month = parseWordForms(value);
		else if (key == "year")		currentData.year = parseWordForms(value);
	}
	saveCurrent();
}

//------------------------------------------------------------------------------------------------------------

namespace
{
	void detectEncoding()
	{
		using namespace localization;

	#ifdef _WIN32
		if (GetACP() == 1251)
		{
			g_encoding = Encoding::Windows1251;
		}
		else
		{
			g_encoding = Encoding::UTF8;
		}
	#else
		g_encoding = Encoding::UTF8;
	#endif
	}

//------------------------------------------------------------------------------------------------------------

	std::string convertString(const std::string& str)
	{
		using namespace localization;

	#ifdef _WIN32
		if (g_encoding == Encoding::Windows1251)
		{
			int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
			std::wstring wide(wlen, 0);
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wide[0], wlen);

			int len = WideCharToMultiByte(1251, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
			std::string result(len, 0);
			WideCharToMultiByte(1251, 0, wide.c_str(), -1, &result[0], len, nullptr, nullptr);
			return result;
		}
	#endif
		return str;
	}

//------------------------------------------------------------------------------------------------------------

	bool setEncoding(std::string_view enc)
	{
		using namespace localization;

		if (enc == "1251" || enc == "windows-1251")
		{
			g_encoding = Encoding::Windows1251;
			return true;
		}
		if (enc == "utf8" || enc == "utf-8")
		{
			g_encoding = Encoding::UTF8;
			return true;
		}
		se_log::logSE("[Error] Unknown encoding: %s", std::string(enc).c_str());
		return false;
	}
//------------------------------------------------------------------------------------------------------------

	std::string getForm(const localization::WordForms& forms, int32_t n)
	{
		n = std::abs(n);

		int32_t mod10 = n % 10;
		int32_t mod100 = n % 100;

		if (mod100 >= 11 && mod100 <= 19)
		{
			return forms.many;
		}
		if (mod10 == 1)
		{
			return forms.one;
		}
		if (mod10 >= 2 && mod10 <= 4)
		{
			return forms.few;
		}
		return forms.many;
	}
}

//------------------------------------------------------------------------------------------------------------

bool localization::init()
{
	detectEncoding();

	if (!std::filesystem::exists(PATH))
	{
		std::ofstream file(PATH);

		if (!file.is_open())
		{
			se_log::logSE("[Error] Failed to create language file, using defaults");
			parseINI(DEFAULT_INI);
			return false;
		}
		file << DEFAULT_INI;
		file.close();
		parseINI(DEFAULT_INI);
		return true;
	}
	std::ifstream file(PATH);

	if (!file.is_open())
	{
		se_log::logSE("[Error] Failed to open language file, using defaults");
		parseINI(DEFAULT_INI);
		return false;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	parseINI(buffer.str());
	return true;
}

//------------------------------------------------------------------------------------------------------------

bool localization::setLanguage(std::string_view lang)
{
	if (g_languages.find(std::string(lang)) == g_languages.end())
	{
		se_log::logSE("[Error] SE_SetLanguage '%s' not found", std::string(lang).c_str());
		return false;
	}
	g_currentLang = std::string(lang);
	return true;
}

//------------------------------------------------------------------------------------------------------------

std::string localization::getRemainingTimeString(int32_t seconds)
{
	if (seconds <= 0)
	{
		return "";
	}
	auto it = g_languages.find(g_currentLang);

	if (it == g_languages.end())
	{
		return "";
	}
	const LangData& lang = it->second;
	utils::TimeComponents c = utils::secondsToComponents(seconds);

	std::string result;
	auto append = [&](int32_t value, const WordForms& forms)
	{
		if (value <= 0)
		{
			return;
		}
		if (!result.empty())
		{
			result += " ";
		}
		result += std::to_string(value) + " " + getForm(forms, value);
	};
	append(c.years, lang.year);
	append(c.months, lang.month);
	append(c.days, lang.day);
	append(c.hours, lang.hour);
	append(c.minutes, lang.minute);
	append(c.seconds, lang.second);

	return convertString(result);
}

//------------------------------------------------------------------------------------------------------------