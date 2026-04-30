#include "database.h"
#include "se_log.h"

#include <filesystem>
#include <ctime>

//------------------------------------------------------------------------------------------------------------

namespace database
{
	std::unique_ptr<SQLite::Database> db;
	std::unique_ptr<SQLite::Statement> stmtInsertData;
	std::unique_ptr<SQLite::Statement> stmtInsertSaveTime;
	std::unique_ptr<SQLite::Statement> stmtDeleteData;
	std::unique_ptr<SQLite::Statement> stmtDeleteSaveTime;
	std::unique_ptr<SQLite::Statement> stmtUpdateData;
	std::unique_ptr<SQLite::Statement> stmtSelectData;
	std::unique_ptr<SQLite::Statement> stmtSelectExpires;
}

namespace
{
	void updateOnlineTime();
	void initStatements();
}

//------------------------------------------------------------------------------------------------------------

bool database::init()
{
	try
	{
		std::filesystem::create_directories("scriptfiles");

		db = std::make_unique<SQLite::Database>(DB_PATH.data(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

		db->exec("PRAGMA journal_mode = WAL;");
		db->exec("PRAGMA synchronous = NORMAL;");
		db->exec("PRAGMA foreign_keys = ON;");

		db->exec("CREATE TABLE IF NOT EXISTS se_events ("
			"event_id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"event_name TEXT UNIQUE, "
			"online INTEGER DEFAULT 1)"
		);
		db->exec("CREATE TABLE IF NOT EXISTS se_data ("
			"u_id INTEGER, "
			"event_id INTEGER, "
			"issued_at INTEGER NOT NULL, "
			"expires_at INTEGER NOT NULL, "
			"PRIMARY KEY(u_id, event_id), "
			"FOREIGN KEY(event_id) REFERENCES se_events (event_id) ON DELETE CASCADE)"
		);
		db->exec("CREATE TABLE IF NOT EXISTS se_save_time ("
			"u_id INTEGER, "
			"event_id INTEGER, "
			"trigger_unixtime INTEGER NOT NULL, "
			"PRIMARY KEY(u_id, event_id), "
			"FOREIGN KEY(event_id) REFERENCES se_events (event_id) ON DELETE CASCADE)"
		);
		db->exec("CREATE TRIGGER IF NOT EXISTS cleanup_save_time "
			"AFTER DELETE ON se_data "
			"BEGIN "
			"  DELETE FROM se_save_time WHERE u_id = OLD.u_id AND event_id = OLD.event_id; "
			"END;"
		);
		updateOnlineTime();
		initStatements();
	}
	catch (std::exception& e)
	{
		se_log::logSE("[Error] SQL Initialization Error: %s", e.what());
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------

namespace
{
	void updateOnlineTime()
	{
		using namespace database;

		try
		{
			int32_t currentTime = static_cast<int32_t>(std::time(nullptr));

			SQLite::Statement select(*db,
				"SELECT "
				"s.u_id, "
				"s.event_id, "
				"s.trigger_unixtime, "
				"d.expires_at "
				"FROM se_save_time s "
				"INNER JOIN se_data d "
				"ON s.u_id = d.u_id AND s.event_id = d.event_id"
			);
			SQLite::Statement update(*db,
				"UPDATE se_data "
				"SET expires_at = ? "
				"WHERE u_id = ? AND event_id = ?"
			);
			while (select.executeStep())
			{
				int32_t uID = select.getColumn("u_id").getInt();
				int32_t eventId = select.getColumn("event_id").getInt();
				int32_t triggerUnixTime = select.getColumn("trigger_unixtime").getInt();
				int32_t seconds = select.getColumn("expires_at").getInt();

				int32_t elapsed = currentTime - triggerUnixTime;
				int32_t newSeconds = std::max<int32_t>(0, seconds - elapsed);

				update.bind(1, newSeconds);
				update.bind(2, uID);
				update.bind(3, eventId);
				update.exec();
				update.reset();
			}
			db->exec("DELETE FROM se_save_time");
		}
		catch (std::exception& e)
		{
			se_log::logSE("[Error] updateOnlineTime Error: %s", e.what());
		}
	}

//------------------------------------------------------------------------------------------------------------

	void initStatements()
	{
		using namespace database;

		stmtInsertData = std::make_unique<SQLite::Statement>(
			*db,
			"INSERT OR REPLACE INTO se_data (u_id, event_id, issued_at, expires_at) VALUES (?, ?, ?, ?)"
		);
		stmtInsertSaveTime = std::make_unique<SQLite::Statement>(
			*db,
			"INSERT OR IGNORE INTO se_save_time (u_id, event_id, trigger_unixtime) VALUES (?, ?, ?)"
		);
		stmtDeleteData = std::make_unique<SQLite::Statement>(
			*db,
			"DELETE FROM se_data WHERE u_id = ? AND event_id = ?"
		);
		stmtDeleteSaveTime = std::make_unique<SQLite::Statement>(
			*db,
			"DELETE FROM se_save_time WHERE u_id = ? AND event_id = ?"
		);
		stmtUpdateData = std::make_unique<SQLite::Statement>(
			*db,
			"UPDATE se_data SET expires_at = ? WHERE u_id = ? AND event_id = ?"
		);
		stmtSelectData = std::make_unique<SQLite::Statement>(
			*db,
			"SELECT * FROM se_data WHERE u_id = ?"
		);
		stmtSelectExpires = std::make_unique<SQLite::Statement>(
			*db,
			"SELECT expires_at FROM se_data WHERE u_id = ? AND event_id = ?"
		);
	}
}

//------------------------------------------------------------------------------------------------------------