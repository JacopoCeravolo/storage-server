#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>

#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"      
#define BOLDYELLOW  "\033[1m\033[33m"      
#define BOLDMAGENTA "\033[1m\033[35m"

typedef enum _loglevel {
    LOG_LVL_DEBUG       = 0,   
    LOG_LVL_INFO        = 1,
    LOG_LVL_WARNING     = 2,
    LOG_LVL_ERROR       = 3,
    LOG_LVL_FATAL       = 4,
} loglevel;

static char*
get_logname(loglevel lvl)
{
    switch (lvl) {
        case LOG_LVL_DEBUG:     return BOLDYELLOW "debug" RESET;
        case LOG_LVL_INFO:      return BOLDGREEN "info" RESET;
        case LOG_LVL_WARNING:   return BOLDMAGENTA "warning" RESET;
        case LOG_LVL_ERROR:     return BOLDRED "error" RESET;
        case LOG_LVL_FATAL:     return BOLDRED "fatal error" RESET;
        default:                return "(none)";
    }
}

#ifndef LOG_LVL
#define LOG_LVL LOG_LVL_FATAL
#endif

#ifndef LOG_FILE
#define LOG_FILE stderr
#endif

#define LOG(level, fmt, args...) { \
            fprintf(LOG_FILE, BOLD "%s:%s:%d: " RESET "%s: " fmt, __FILE__, __func__,  \
                    __LINE__, get_logname(level), ##args); fflush(stderr); }

#define LOG_INFO(fmt, args...) { \
            fprintf(LOG_FILE, "%s: " fmt, \
                    get_logname(LOG_LVL_INFO), ##args); fflush(stderr); }


#define LOG_DEBUG(args...)      LOG(LOG_LVL_DEBUG, args)
#define LOG_WARNING(args...)    LOG(LOG_LVL_WARNING, args)          
#define LOG_ERROR(args...)      LOG(LOG_LVL_ERROR, args)
#define LOG_FATAL(args...)      LOG(LOG_LVL_FATAL, args)

#endif