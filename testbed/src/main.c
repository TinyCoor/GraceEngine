#include "core/logger.h"
#include "core/asserts.h"

int main(void)
{
    KDEBUG("This is test: %d",10);
    KINFO("This is test: %d",10);
    KERROR("This is test: %d",10);
    KWARN("This is test: %d",10);
    KTRACE("This is test: %d",10);
    KASSERT_MSG(2 > 3,"not");

    return 0;
}
