//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_CORE_KSTRING_H
#define BACKEND_ENGINE_SRC_CORE_KSTRING_H
#include "defines.h"

KAPI u64 string_length(const char* str);

KAPI char* string_duplicate(const char* str);

KAPI b8 string_equal(const char* lhs,const char* rhs);

#endif //BACKEND_ENGINE_SRC_CORE_KSTRING_H
