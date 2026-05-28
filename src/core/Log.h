#pragma once
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <mutex>

namespace forge {

enum class LogLevel : int { Trace = 0, Debug, Info, Warn, Error, Fatal };

class Log {
public:
    static void init(LogLevel minLevel = LogLevel::Trace);
    static void setLevel(LogLevel l) { s_minLevel = l; }
    static LogLevel level()          { return s_minLevel; }

    static void log(LogLevel level, const char* file, int line, const char* fmt, ...);

private:
    static LogLevel   s_minLevel;
    static std::mutex s_mutex;
};

} // namespace forge

// clang-format off
#define FG_TRACE(...) ::forge::Log::log(::forge::LogLevel::Trace, __FILE__, __LINE__, __VA_ARGS__)
#define FG_DEBUG(...) ::forge::Log::log(::forge::LogLevel::Debug, __FILE__, __LINE__, __VA_ARGS__)
#define FG_INFO(...)  ::forge::Log::log(::forge::LogLevel::Info,  __FILE__, __LINE__, __VA_ARGS__)
#define FG_WARN(...)  ::forge::Log::log(::forge::LogLevel::Warn,  __FILE__, __LINE__, __VA_ARGS__)
#define FG_ERROR(...) ::forge::Log::log(::forge::LogLevel::Error, __FILE__, __LINE__, __VA_ARGS__)
#define FG_FATAL(...) ::forge::Log::log(::forge::LogLevel::Fatal, __FILE__, __LINE__, __VA_ARGS__)
// clang-format on
