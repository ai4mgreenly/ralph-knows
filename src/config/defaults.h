#ifndef RK_CONFIG_DEFAULTS_H
#define RK_CONFIG_DEFAULTS_H

// Compiled-in default path suffixes (relative to $HOME)
#define RK_DEFAULT_WATCH_PATH_SUFFIX  "/projects"
#define RK_DEFAULT_STATE_DIR_SUFFIX   "/.local/state/ralph-knows"

// Default log level
#define RK_DEFAULT_LOG_LEVEL RK_LOG_INFO

// Environment variable names
#define RK_ENV_WATCH_PATH  "RALPH_KNOWS_WATCH_PATH"
#define RK_ENV_STATE_DIR   "RALPH_KNOWS_STATE_DIR"
#define RK_ENV_LOG_LEVEL   "RALPH_KNOWS_LOG_LEVEL"

#endif
