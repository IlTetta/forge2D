#pragma once
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <mutex>

namespace forge {

/** @brief Severity levels for the logging system, ordered from least to most severe. */
enum class LogLevel : int { Trace = 0, Debug, Info, Warn, Error, Fatal };

/**
 * @brief Thread-safe, leveled logger with coloured console output.
 *
 * Each message is prefixed with a timestamp, severity tag, and the
 * source file + line number. Use the convenience macros instead of
 * calling Log::log() directly.
 *
 * @code
 * forge::Log::init(forge::LogLevel::Info);
 * FG_INFO("Player spawned at (%.1f, %.1f)", x, y);
 * FG_WARN("Tilemap file not found: %s", path.c_str());
 * @endcode
 */
class Log {
public:
    /**
     * @brief Initialises the logger with a minimum severity level.
     * @param minLevel Messages below this level are silently discarded.
     */
    static void init(LogLevel minLevel = LogLevel::Trace);

    /** @brief Changes the minimum severity level at runtime. */
    static void setLevel(LogLevel l) { s_minLevel = l; }

    /** @brief Returns the current minimum severity level. */
    static LogLevel level() { return s_minLevel; }

    /**
     * @brief Logs a formatted message (printf-style).
     *
     * Prefer the FG_* macros which capture __FILE__ and __LINE__ automatically.
     *
     * @param level   Severity of this message.
     * @param file    Source file name (typically __FILE__).
     * @param line    Source line number (typically __LINE__).
     * @param fmt     printf-style format string.
     */
    static void log(LogLevel level, const char* file, int line, const char* fmt, ...);

private:
    static LogLevel   s_minLevel;
    static std::mutex s_mutex;
};

} // namespace forge

// clang-format off
/** @brief Log a TRACE message (finest detail, usually disabled in release). */
#define FG_TRACE(...) ::forge::Log::log(::forge::LogLevel::Trace, __FILE__, __LINE__, __VA_ARGS__)
/** @brief Log a DEBUG message. */
#define FG_DEBUG(...) ::forge::Log::log(::forge::LogLevel::Debug, __FILE__, __LINE__, __VA_ARGS__)
/** @brief Log an INFO message. */
#define FG_INFO(...)  ::forge::Log::log(::forge::LogLevel::Info,  __FILE__, __LINE__, __VA_ARGS__)
/** @brief Log a WARNING message. */
#define FG_WARN(...)  ::forge::Log::log(::forge::LogLevel::Warn,  __FILE__, __LINE__, __VA_ARGS__)
/** @brief Log an ERROR message. */
#define FG_ERROR(...) ::forge::Log::log(::forge::LogLevel::Error, __FILE__, __LINE__, __VA_ARGS__)
/** @brief Log a FATAL message (engine is in an unrecoverable state). */
#define FG_FATAL(...) ::forge::Log::log(::forge::LogLevel::Fatal, __FILE__, __LINE__, __VA_ARGS__)
// clang-format on
