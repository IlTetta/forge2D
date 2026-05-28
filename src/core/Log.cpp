#include "Log.h"
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

namespace forge {

LogLevel   Log::s_minLevel = LogLevel::Trace;
std::mutex Log::s_mutex;

static const char* levelLabel(LogLevel l) {
    switch (l) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
    }
    return "?????";
}

static const char* levelColor(LogLevel l) {
    switch (l) {
        case LogLevel::Trace: return "\033[90m";    // dark gray
        case LogLevel::Debug: return "\033[36m";    // cyan
        case LogLevel::Info:  return "\033[32m";    // green
        case LogLevel::Warn:  return "\033[33m";    // yellow
        case LogLevel::Error: return "\033[31m";    // red
        case LogLevel::Fatal: return "\033[1;31m";  // bold red
    }
    return "";
}

// Strips directory prefix from __FILE__ at runtime.
static const char* basename(const char* path) {
    const char* p = path;
    while (*p) ++p;            // move to end
    while (p > path && *p != '/' && *p != '\\') --p;
    return (*p == '/' || *p == '\\') ? p + 1 : p;
}

void Log::init(LogLevel minLevel) {
    s_minLevel = minLevel;

#ifdef _WIN32
    // Enable ANSI escape codes on Windows 10+
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void Log::log(LogLevel level, const char* file, int line, const char* fmt, ...) {
    if (level < s_minLevel) return;

    std::time_t t  = std::time(nullptr);
    std::tm*    tm = std::localtime(&t);
    char        ts[10];
    std::strftime(ts, sizeof(ts), "%H:%M:%S", tm);

    va_list args;
    va_start(args, fmt);
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        FILE* out = (level >= LogLevel::Error) ? stderr : stdout;
        std::fprintf(out, "%s[%s] %s  %s:%d  ", levelColor(level), ts, levelLabel(level), basename(file), line);
        std::vfprintf(out, fmt, args);
        std::fprintf(out, "\033[0m\n");
        std::fflush(out);
    }
    va_end(args);

    if (level == LogLevel::Fatal) std::abort();
}

} // namespace forge
