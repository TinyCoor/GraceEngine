//
// Created by 12132 on 2022/1/5.
//

#include "logger.h"
#include "asserts.h"

///todo  temp
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


b8 initialize_logging()
{
    return true;
}

void shutdown_logging()
{

}


void log_output(log_level level,const char *message,...)
{
    const char* level_strings[6] = {
        "[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "
    };
    b8 is_error = level < 2;

    char output_message[1024];
    memset(output_message,0, sizeof(output_message));

    ///
    __builtin_va_list arg_ptr;
    va_start(arg_ptr,message);
    vsnprintf(output_message,1024,message,arg_ptr);
    va_end(arg_ptr);

    /// todo platform
    char output[1024];
    memset(output,0, sizeof(output_message));
    sprintf(output,"%s%s\n",level_strings[level],output_message);
    printf("%s",output);
}


void report_assertion_failure(const char* expression,
                              const char* message,
                              const char* file,
                              i32 line)
{
    log_output(LOG_LEVEL_FATAL,"Assertion failure: %s, message: '%s',in file: %s, line %d\n",
               expression,message,file,line);
}
