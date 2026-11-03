// Simple file logger.  A single LogManager is created at startup; the
// LogInfo/LogError macros write through it.

#pragma once

#include <cstdio>

namespace sim {

class LogManager {
public:
    enum LOG_LEVEL { LOG_FATAL, LOG_ERROR, LOG_WARNING, LOG_INFORMATION };

    explicit LogManager(const char *pszPathname = nullptr);
    ~LogManager();

    void LogMessage(enum LOG_LEVEL level, const char *file, int line,
                    const char *fmt, ...);

private:
    enum LOG_LEVEL m_eLoggingLevel;
    FILE *m_stream;
};

LogManager *GetLogManager();

#define LogError(fmt) \
    sim::GetLogManager()->LogMessage(sim::LogManager::LOG_ERROR, __FILE__, __LINE__, fmt)
#define LogError1(fmt, p1) \
    sim::GetLogManager()->LogMessage(sim::LogManager::LOG_ERROR, __FILE__, __LINE__, fmt, p1)
#define LogInfo(fmt) \
    sim::GetLogManager()->LogMessage(sim::LogManager::LOG_INFORMATION, __FILE__, __LINE__, fmt)
#define LogInfo1(fmt, p1) \
    sim::GetLogManager()->LogMessage(sim::LogManager::LOG_INFORMATION, __FILE__, __LINE__, fmt, p1)

}  // namespace sim
