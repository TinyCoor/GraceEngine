//
// Created by 12132 on 2022/1/9.
//
#include "kstring.h"
#include "kmemory.h"
#include <string.h>

KAPI u64 string_length(const char* str)
{
    return strlen(str);
}

KAPI char* string_duplicate(const char* str)
{
    u64 length = string_length(str);
    char* copy = kallocate(length+1,MEMORY_TAG_STRING);
    kcopy_memory(copy,str,length + 1);
    return copy;
}

KAPI b8 string_equal(const char* lhs,const char* rhs)
{
    return strcmp(lhs,rhs) == 0;
}