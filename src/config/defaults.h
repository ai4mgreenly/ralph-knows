#ifndef FX_CONFIG_DEFAULTS_H
#define FX_CONFIG_DEFAULTS_H

// Compiled-in default path suffixes (relative to $HOME)
#define FX_DEFAULT_WATCH_PATH_SUFFIX     "/projects"
#define FX_DEFAULT_DB_PATH_SUFFIX        "/.local/state/fandex/fandex.db"

// Compiled-in default socket path suffix (relative to /run/user/$UID)
#define FX_DEFAULT_SOCKET_PATH_SUFFIX    "/fandex/fandex.sock"

// Environment variable names
#define FX_ENV_WATCH_PATH   "FANDEX_WATCH_PATH"
#define FX_ENV_DB_PATH      "FANDEX_DB_PATH"
#define FX_ENV_SOCKET_PATH  "FANDEX_SOCKET_PATH"

#endif
