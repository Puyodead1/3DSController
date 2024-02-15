#include "common.h"

void errorf(const char *fmt, ...)
{
    va_list ap;
    printf(CONSOLE_RED);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf(CONSOLE_RESET);
}

//---------------------------------------------------------------------------------
void failExit(const char *fmt, ...)
{
    //---------------------------------------------------------------------------------

    // if (sock > 0)
    // 	close(sock);

    va_list ap;

    printf(CONSOLE_RED);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf(CONSOLE_RESET);
    printf("\nPress START to exit\n");

    while (aptMainLoop())
    {
        gspWaitForVBlank();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            exit(0);
    }
}