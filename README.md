[![release](https://img.shields.io/github/v/release/i-Saibot/SmartEvents?include_prereleases)](https://github.com/i-Saibot/SmartEvents/releases)
[![downloads](https://img.shields.io/github/downloads/i-Saibot/SmartEvents/total)](https://github.com/i-Saibot/SmartEvents/releases)
[![license](https://img.shields.io/github/license/i-Saibot/SmartEvents)](LICENSE)
![Stars](https://img.shields.io/github/stars/i-Saibot/SmartEvents?style=social)

# *SmartEvents*

> A C++ plugin for SA-MP and open.mp to manage persistent time-based events (mutes, jails, etc.) without repetitive timers, using timestamp-based logic and SQLite storage.

---

## 📖 Documentation

Detailed information about functions and usage can be found in the **[SmartEvents Wiki](https://github.com/i-Saibot/SmartEvents/wiki)**.

---

## How it works

When creating an event you need to specify:
- **Event name**
- **Callback** — called once when the time expires. If the remaining time is more than 24 hours, the callback is not scheduled until the next server restart, optimizing performance.
- **Time type** — there are two types:
  - `false` — **offline**: time expires even when the player is not on the server. Saved once when the event is assigned.
  - `true` — **online**: time expires only while the player is on the server. Saved once when assigned and again each time the player disconnects.

---

## Crash safety

Even for online events, data loss on crash is not an issue. When the server restarts, the plugin checks the temporary table, calculates the difference between the saved time and the current startup time, and updates the main table accordingly.

> Your server must have an auto-restart script in case of a crash. This is included by default on all hosting providers, and on VPS it is set up manually.

---

## Requirements

- Automatic daily server restart (standard on all normal servers)
- Auto-restart script on crash (included by default on all hosting providers, set up manually on VPS)

---

## Data storage

All data is stored in a SQLite database at `scriptfiles/SmartEvents.db`.

---

## Integration

Call these two functions in your gamemode — one after login, one after registration:

```pawn
public OnPlayerLogin(playerid)
{
    // Your account loading
    cache_get_value_name_int(0, "ID", PlayerInfo[playerid][pID]);

    SE_OnPlayerLogin(playerid, PlayerInfo[playerid][pID]);
}

public OnPlayerSignIn(playerid)
{
    // When the player has registered
    PlayerInfo[playerid][pID] = cache_insert_id();
    SE_OnPlayerSignIn(playerid, PlayerInfo[playerid][pID]);
}
```

---

## Benchmark

Why is this better than decrementing every second?

| | Tick & Save | SmartEvents |
|---|---|---|
| DB writes | Every second per player | On assign + on disconnect |
| Callback calls | Every second | Once when expired |

100 players · mute 5 hours · 15 reconnects per player only for plugin

<img src="https://github.com/i-Saibot/SmartEvents/blob/main/Benchmark.png?raw=true" width="600"/>

<details>
<summary>Benchmark source code</summary>

```pawn
#define SCENARIO_PLAYERS        100
#define SCENARIO_RECONNECTS     15
#define SCENARIO_MUTE_SECONDS   (5 * 60 * 60) 

enum pInfo { pID, pMute }
new PlayerInfo[MAX_PLAYERS][pInfo];

new SE:g_MuteEvent;
new MySQL:dbHandle;

new g_TickSaveElapsed = 0;
new g_seElapsed = 0;

// Данные для HTML отчёта
new g_timerCalls = 0;
new g_dbQueriesB = 0;
new g_seOps = 0;
new g_callbacks = 0;

forward OnTableCleared();
forward OnUsersInserted();

stock DB_Setup()
{
    print("[SETUP] Clearing test records...");
    mysql_tquery(dbHandle, "DELETE FROM users WHERE id BETWEEN 1000 AND 1099", "OnTableCleared");
}

public OnTableCleared()
{
    print("[SETUP] Inserting test players...");
    for (new i = 0; i < SCENARIO_PLAYERS; i++)
    {
        PlayerInfo[i][pID]   = 1 + i;
        PlayerInfo[i][pMute] = 0;

        new query[128];
        format(query, sizeof(query), "INSERT INTO users (id, mute) VALUES (%d, 0)", 1000 + i);
        mysql_tquery(dbHandle, query);
    }
    mysql_tquery(dbHandle, "SELECT 1", "OnUsersInserted");
}

public OnUsersInserted()
{
    printf("[SETUP] Done %d players in DB", SCENARIO_PLAYERS);
    Bench_TickSave_B();
}


// =============================================================================
// Tick & Save
// =============================================================================
stock UpdateMysql(playerid)
{
    new query[128];
    format(query, sizeof(query),
        "UPDATE users SET mute = %d WHERE id = %d",
        PlayerInfo[playerid][pMute], PlayerInfo[playerid][pID]
    );
    mysql_tquery(dbHandle, query);
}

stock Bench_TickSave_B()
{
    print("\n--- [BENCH] Tick & Save: save every second ---");

    new totalTimerCalls = 0;
    new totalDbQueries  = 0;
    new t = GetTickCount();

    for (new pid = 0; pid < SCENARIO_PLAYERS; pid++)
    {
        PlayerInfo[pid][pMute] = SCENARIO_MUTE_SECONDS;
    }

    for (new sec = 0; sec < SCENARIO_MUTE_SECONDS; sec++)
    {
        for (new pid = 0; pid < SCENARIO_PLAYERS; pid++)
        {
            if (PlayerInfo[pid][pMute] > 0)
            {
                PlayerInfo[pid][pMute]--;
                totalTimerCalls++;

                UpdateMysql(pid);
                totalDbQueries++;

                if (PlayerInfo[pid][pMute] == 0)
                {
                    //SendClientMessage
                }
            }
        }
    }

    g_TickSaveElapsed = GetTickCount() - t;
    g_timerCalls = totalTimerCalls;
    g_dbQueriesB = totalDbQueries;

    printf("  Timer ticks:       %d", totalTimerCalls);
    printf("  MySQL queries:     %d", totalDbQueries);
    printf("  Crash-safe:        YES");
    printf("  Simulation time:   %d ms", g_TickSaveElapsed);

    Bench_SE();
}

// =============================================================================
// SE Plugin
// =============================================================================
stock Bench_SE()
{
    print("\n--- [BENCH] SE Plugin ---");

    new totalSetCalls = 0;
    new totalLoginCalls = 0;
    new totalDisconCalls = 0;
    new t = GetTickCount();

    for (new pid = 0; pid < SCENARIO_PLAYERS; pid++)
    {
        SE_OnPlayerLogin(pid, 1 + pid);
        SE_SetPlayerEvent(pid, g_MuteEvent, SCENARIO_MUTE_SECONDS);
        totalSetCalls++;
    }
    for (new reconnect = 0; reconnect < SCENARIO_RECONNECTS; reconnect++)
    {
        for (new pid = 0; pid < SCENARIO_PLAYERS; pid++)
        {
            SE_Disconnect(pid);
            totalDisconCalls++;

            SE_OnPlayerLogin(pid, 1 + pid);
            totalLoginCalls++;
        }
    }
    g_seElapsed = GetTickCount() - t;
    g_seOps = totalSetCalls + totalDisconCalls + totalLoginCalls;
    g_callbacks = SCENARIO_PLAYERS;

    printf("  Timer ticks:       0");
    printf("  SQLite ops:        %d (Set:%d + Disconnect:%d + Login:%d)",
        g_seOps, totalSetCalls, totalDisconCalls, totalLoginCalls);
    printf("  Crash-safe:        YES");
    printf("  Callbacks:         %d (fires when mute actually expires)", SCENARIO_PLAYERS);
    printf("  Simulation time:   %d ms", g_seElapsed);

    SaveHTMLReport();
}

public OnGameModeInit()
{
    dbHandle = mysql_connect("127.0.0.1", "root", "root", "test_db");
    mysql_log(ERROR | WARNING);

    switch (mysql_errno())
    {
        case 0:    print("MySQL: connected successfully");
        case 1045: print("MySQL: invalid login or password");
        case 1049: print("MySQL: database not found");
        case 2003: print("MySQL: server unreachable");
        default:   printf("MySQL: error %d", mysql_errno());
    }
    g_MuteEvent = SE_AddEvent("mute", "OnMuteExpired", true);

    print("\n================================================");
    printf(" Scenario: %d players | mute %d h | %d reconnects", SCENARIO_PLAYERS, SCENARIO_MUTE_SECONDS / 3600, SCENARIO_RECONNECTS);
    print("================================================");

    DB_Setup();
    return 1;
}

SE_Event:OnMuteExpired(playerid)
{
    return 1;
}

// =============================================================================
// HTML
// =============================================================================
stock FormatNumber(num, output[], size)
{
    // Форматируем число с пробелами: 1800000 -> 1 800 000
    new str[32];
    format(str, sizeof(str), "%d", num);
    new len = strlen(str);
    new pos = 0;
    new result[32];
    new offset = len % 3;
    if (offset == 0) offset = 3;
    for (new i = 0; i < len; i++)
    {
        result[pos++] = str[i];
        if ((i + 1 - (len % 3 == 0 ? 0 : (3 - len % 3))) % 3 == 0 && i < len - 1)
            result[pos++] = ' ';
    }
    result[pos] = '\0';
    format(output, size, "%s", result);
}

stock SaveHTMLReport()
{
    new File:f = fopen("bench_result.html", io_write);
    if (!f)
    {
        print("[ERROR] Failed to create bench_result.html");
        return;
    }

    new buf[512];

    // Форматируем числа
    new s_timerTickSave[32], s_timerSE[32];
    new s_callbacksTickSave[32], s_callbacksSE[32];
    new s_dbTickSave[32], s_dbSE[32];
    new s_timeTickSave[32], s_timeSE[32];

    format(s_timerTickSave,     sizeof(s_timerTickSave),     "%d", g_timerCalls);
    format(s_timerSE,          sizeof(s_timerSE),          "0");
    format(s_callbacksTickSave, sizeof(s_callbacksTickSave), "%d", g_timerCalls);
    format(s_callbacksSE,      sizeof(s_callbacksSE),      "%d", g_callbacks);
    format(s_dbTickSave,        sizeof(s_dbTickSave),        "%d", g_dbQueriesB);
    format(s_dbSE,             sizeof(s_dbSE),             "%d", g_seOps);
    format(s_timeTickSave,      sizeof(s_timeTickSave),      "%d ms", g_TickSaveElapsed);
    format(s_timeSE,           sizeof(s_timeSE),           "%d ms", g_seElapsed);

    fwrite(f, "<!DOCTYPE html>\n");
    fwrite(f, "<html lang=\"ru\">\n<head>\n<meta charset=\"UTF-8\">\n");
    fwrite(f, "<title>Benchmark Result</title>\n");
    fwrite(f, "<style>\n");
    fwrite(f, "@import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;500;700&family=Rajdhani:wght@400;500;600;700&display=swap');\n");
    fwrite(f, "*{box-sizing:border-box;margin:0;padding:0}\n");
    fwrite(f, "body{background:#1a1c1e;min-height:100vh;display:flex;align-items:center;justify-content:center;font-family:'Rajdhani',sans-serif;padding:32px 16px}\n");
    fwrite(f, ".card{background:#22252a;border-radius:12px;padding:28px 32px;width:100%;max-width:720px;box-shadow:0 8px 40px rgba(0,0,0,0.5)}\n");
    fwrite(f, ".title{font-size:13px;font-weight:600;color:#6b7280;letter-spacing:.12em;text-transform:uppercase;margin-bottom:18px}\n");
    fwrite(f, ".stat-row{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:22px}\n");
    fwrite(f, ".stat-card{background:#2c3038;border-radius:8px;padding:12px 16px}\n");
    fwrite(f, ".stat-label{font-size:11px;color:#6b7280;margin-bottom:4px;letter-spacing:.05em}\n");
    fwrite(f, ".stat-value{font-size:26px;font-weight:700;color:#e5e7eb;font-family:'JetBrains Mono',monospace}\n");
    fwrite(f, ".stat-sub{font-size:11px;color:#6b7280;margin-top:3px}\n");
    fwrite(f, ".badge-se-s{display:inline-block;padding:1px 6px;border-radius:3px;font-size:10px;font-weight:700;background:#1D9E75;color:#fff;margin-left:6px;vertical-align:middle;letter-spacing:.04em}\n");
    fwrite(f, "table{width:100%;border-collapse:collapse}\n");
    fwrite(f, "thead tr{border-bottom:1px solid #2c3038}\n");
    fwrite(f, "th{font-size:12px;font-weight:600;color:#6b7280;padding:8px 10px;text-align:left;letter-spacing:.06em;text-transform:uppercase}\n");
    fwrite(f, "th.r{text-align:right}\n");
    fwrite(f, ".sec-row td{padding:7px 10px 3px;font-size:10px;font-weight:700;color:#6b7280;background:#252830;letter-spacing:.12em;text-transform:uppercase}\n");
    fwrite(f, "td{padding:10px 10px;border-bottom:1px solid #2c3038;color:#d1d5db;vertical-align:middle;font-size:14px}\n");
    fwrite(f, "td.r{text-align:right;font-family:'JetBrains Mono',monospace;font-size:13px;font-weight:500}\n");
    fwrite(f, "td.name{font-weight:600;color:#e5e7eb}\n");
    fwrite(f, "tr:last-child td{border-bottom:none}\n");
    fwrite(f, ".badge{display:inline-block;padding:3px 9px;border-radius:4px;font-size:11px;font-weight:700;letter-spacing:.04em}\n");
    fwrite(f, ".se{background:#1D9E75;color:#fff}\n");
    fwrite(f, ".dash{color:#4b5563}\n");
    fwrite(f, "tr.dr:hover td{background:#272b32}\n");
    fwrite(f, ".num{color:#e5e7eb}\n");
    fwrite(f, ".dim{color:#6b7280}\n");
    fwrite(f, "</style>\n</head>\n<body>\n<div class=\"card\">\n");

    // Header
    fwrite(f, "<div class=\"title\">Benchmark — SE Plugin vs Tick & Save</div>\n");

    // Stat cards
    fwrite(f, "<div class=\"stat-row\">\n");
    fwrite(f, "<div class=\"stat-card\"><div class=\"stat-label\">Players</div>");

    format(buf, sizeof(buf), "<div class=\"stat-value\">%d</div>", SCENARIO_PLAYERS);
    fwrite(f, buf);
    fwrite(f, "<div class=\"stat-sub\">mute 5 hours</div></div>\n");

    fwrite(f, "<div class=\"stat-card\"><div class=\"stat-label\">Reconnection only for <span class=\"badge-se-s\">SE</span></div>");
    format(buf, sizeof(buf), "<div class=\"stat-value\">%d</div>", SCENARIO_RECONNECTS);
    fwrite(f, buf);
    fwrite(f, "<div class=\"stat-sub\">per player</div></div>\n");
    fwrite(f, "</div>\n");

    // Table
    fwrite(f, "<table>\n<thead><tr>");
    fwrite(f, "<th style=\"width:40%\">Metric</th>");
    fwrite(f, "<th class=\"r\" style=\"width:25%\">Tick & Save</th>");
    fwrite(f, "<th class=\"r\" style=\"width:25%\">SE Plugin</th>");
    fwrite(f, "<th style=\"width:10%\"></th>");
    fwrite(f, "</tr></thead>\n<tbody>\n");

    // Section: Server load
    fwrite(f, "<tr class=\"sec-row\"><td colspan=\"4\">Server Load</td></tr>\n");

    format(buf, sizeof(buf), "<tr class=\"dr\"><td class=\"name\">Timer ticks</td><td class=\"r num\">%s</td><td class=\"r dim\">0</td><td style=\"text-align:right\"><span class=\"badge se\">SE</span></td></tr>\n", s_timerTickSave);
    fwrite(f, buf);

    format(buf, sizeof(buf), "<tr class=\"dr\"><td class=\"name\">Callbacks total</td><td class=\"r num\">%s</td><td class=\"r num\">%s</td><td style=\"text-align:right\"><span class=\"badge se\">SE</span></td></tr>\n", s_callbacksTickSave, s_callbacksSE);
    fwrite(f, buf);

    format(buf, sizeof(buf), "<tr class=\"dr\"><td class=\"name\">Simulation time</td><td class=\"r num\">%s</td><td class=\"r num\">%s</td><td style=\"text-align:right\"><span class=\"badge se\">SE</span></td></tr>\n", s_timeTickSave, s_timeSE);
    fwrite(f, buf);

    // Section: DB operations
    fwrite(f, "<tr class=\"sec-row\"><td colspan=\"4\">Database Operations</td></tr>\n");

    format(buf, sizeof(buf), "<tr class=\"dr\"><td class=\"name\">Queries / DB ops</td><td class=\"r num\">%s</td><td class=\"r num\">%s</td><td style=\"text-align:right\"><span class=\"badge se\">SE</span></td></tr>\n", s_dbTickSave, s_dbSE);
    fwrite(f, buf);



    fwrite(f, "</tbody>\n</table>\n</div>\n</body>\n</html>\n");

    fclose(f);
    print("[HTML] Report saved: scriptfiles/bench_result.html");
}
```

</details>
