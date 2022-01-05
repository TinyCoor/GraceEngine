//
// Created by 12132 on 2022/1/5.
//

#ifndef BACKEND_ENGINE_SRC_CORE_LOGGER_H
#define BACKEND_ENGINE_SRC_CORE_LOGGER_H

#include "../defines.h"
#define LOG_WARN_ENABLE 1
#define LOG_INFO_ENABLE 1
#define LOG_DEBUG_ENABLE 1
#define LOG_TRACE_ENABLE 1

#if KRELEASE == 1
#define LOG_DEBUG_ENABLE 0
#define LOG_TRACE_ENABLE 0
#endif


typedef enum log_level{
    LOG_LEVEL_FATAL=0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
}log_level;

b8 initialize_logging();

void shutdown_logging();

void log_output(log_level level,const char *message,...);

#define KFATAL(message,...) log_output(LOG_LEVEL_FATAL,message,##__VA_ARGS__)

#ifndef KERROR
#define KERROR(message,...) log_output(LOG_LEVEL_ERROR,message,##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLE
#define KWARN(message,...) log_output(LOG_LEVEL_WARN,message,##__VA_ARGS__)
#else
#define KWARN(message,...)
#endif

#if LOG_INFO_ENABLE == 1
#define KINFO(message,...) log_output(LOG_LEVEL_INFO,message,##__VA_ARGS__)
#else
#define KINFO(message,...)
#endif


#if LOG_DEBUG_ENABLE == 1
#define KDEBUG(message,...) log_output(LOG_LEVEL_DEBUG,message,##__VA_ARGS__)
#else
#define KDEBUG(message,...)
#endif

#if LOG_TRACE_ENABLE == 1
#define KTRACE(message,...) log_output(LOG_LEVEL_TRACE,message,##__VA_ARGS__)
#else
#define KTRACE(message,...)
#endif









#endif //BACKEND_ENGINE_SRC_CORE_LOGGER_H
