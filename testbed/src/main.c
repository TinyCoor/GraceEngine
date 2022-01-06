#include "core/logger.h"
#include "core/asserts.h"
#include "platform/platform.h"

int main(void)
{
    KDEBUG("This is test: %d",10);
    KINFO("This is test: %d",10);
    KERROR("This is test: %d",10);
    KWARN("This is test: %d",10);
    KTRACE("This is test: %d",10);

    platform_state state;
    if(platform_startup(&state,"Engine",100,100,1280,720)) {
        while (true){
           platform_pump_message(&state);
        }
    }

    platform_shutdown(&state);

    return 0;
}
