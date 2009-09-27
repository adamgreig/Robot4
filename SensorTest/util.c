// Utility functions

#include "util.h"

__attribute__ ((noinline)) void delay_ms(volatile uint16_t ms) {
    asm(
        "delayms_loop:                          \r\n"
        "   ldr r0, =12000                      \r\n"
        "   delayms_innerloop:                  \r\n"
        "       subs r0, #1                     \r\n"
        "       bne delayms_innerloop           \r\n"
        "   subs %0, #1                         \r\n"
        "   bne delayms_loop                    \r\n"
        :
        : "r"(ms)
        : "cc", "r0", "r1"
    );
}
