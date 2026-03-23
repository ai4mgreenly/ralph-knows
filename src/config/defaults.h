#ifndef RK_CONFIG_DEFAULTS_H
#define RK_CONFIG_DEFAULTS_H

// Compiled-in default path suffixes (relative to $HOME)
#define RK_DEFAULT_WATCH_PATH_SUFFIX     "/projects"
#define RK_DEFAULT_DB_PATH_SUFFIX        "/.local/state/ralph-knows/ralph-knows.db"

// Compiled-in default socket path suffix (relative to /run/user/$UID)
#define RK_DEFAULT_SOCKET_PATH_SUFFIX    "/ralph-knows/ralph-knows.sock"

// Default log level
#define RK_DEFAULT_LOG_LEVEL RK_LOG_INFO

// Environment variable names
#define RK_ENV_WATCH_PATH   "RALPH_KNOWS_WATCH_PATH"
#define RK_ENV_DB_PATH      "RALPH_KNOWS_DB_PATH"
#define RK_ENV_SOCKET_PATH  "RALPH_KNOWS_SOCKET_PATH"
#define RK_ENV_LOG_LEVEL    "RALPH_KNOWS_LOG_LEVEL"

#endif
