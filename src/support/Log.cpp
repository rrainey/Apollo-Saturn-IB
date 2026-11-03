#include "Log.h"

#include <cstdarg>

namespace sim {

static LogManager *g_pLogManager = nullptr;

LogManager *GetLogManager() { return g_pLogManager; }

LogManager::LogManager(const char *pszPathname)
    : m_eLoggingLevel(LOG_INFORMATION), m_stream(nullptr) {
    g_pLogManager = this;
    m_stream = std::fopen(pszPathname ? pszPathname : "./errors.log", "w");
    if (!m_stream) std::printf("Error: cannot open the log file\n");
}

LogManager::~LogManager() {
    if (m_stream) {
        std::fclose(m_stream);
        m_stream = nullptr;
    }
}

void LogManager::LogMessage(enum LOG_LEVEL level, const char *file, int line,
                            const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if (m_stream && level <= m_eLoggingLevel) {
        std::fprintf(m_stream, "%s:%d ", file, line);
        std::vfprintf(m_stream, fmt, ap);
        std::fflush(m_stream);
    }
    va_end(ap);
}

}  // namespace sim
