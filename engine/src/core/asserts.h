//
// Created by 12132 on 2022/1/5.
//

#ifndef BACKEND_ENGINE_SRC_CORE_ASSERTS_H
#define BACKEND_ENGINE_SRC_CORE_ASSERTS_H

#include "../defines.h"

#define KASSERTION_ENABLED

#ifdef KASSERTION_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

void report_assertion_failure(const char* expression,
                              const char* message,
                              const char* file,
                              i32 line);

#define KASSERT(expr)                                                   \
    do {                                                                   \
        if(expr){                                                       \
        }else {                                                         \
              report_assertion_failure(#expr,"",__FILE__,__LINE__);     \
              debugBreak();                                             \
        }                                                               \
    }while(0)

#define KASSERT_MSG(expr, message) \
    do {                                                                       \
        if(expr){                                                           \
        } else {                                                             \
              report_assertion_failure(#expr,message,__FILE__,__LINE__);     \
              debugBreak();                                                  \
        }                                                                    \
    }while(0)

#ifdef _DEBUG
#define KASSERT_DEBUG(expr)                                                   \
    do{                                                                   \
        if(expr){                                                       \
        }else {                                                         \
              report_assertion_failure(#expr,"",__FILE__,__LINE__);     \
              debugBreak();                                             \
        }                                                               \
    }while(0)
#else
#define KASSERT_DEBUG(expr)
#endif

#else
#define KASSERT(expr)
#define KASSERT_MSG(expr, message)
#define KASSERT_DEBUG(expr)
#endif


#endif //BACKEND_ENGINE_SRC_CORE_ASSERTS_H
