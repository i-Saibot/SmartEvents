#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string_view>

//------------------------------------------------------------------------------------------------------------

namespace database
{
	inline constexpr std::string_view DB_PATH = "scriptfiles/SmartEvents.db";

	extern std::unique_ptr<SQLite::Database> db;
	extern std::unique_ptr<SQLite::Statement> stmtInsertData;
	extern std::unique_ptr<SQLite::Statement> stmtInsertSaveTime;
	extern std::unique_ptr<SQLite::Statement> stmtDeleteData;
	extern std::unique_ptr<SQLite::Statement> stmtDeleteSaveTime;
	extern std::unique_ptr<SQLite::Statement> stmtUpdateData;
	extern std::unique_ptr<SQLite::Statement> stmtSelectData;
	extern std::unique_ptr<SQLite::Statement> stmtSelectExpires;

	bool init();
}

//------------------------------------------------------------------------------------------------------------