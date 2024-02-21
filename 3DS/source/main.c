#include <malloc.h>
#include <setjmp.h>

#include "wireless.h"
#include "settings.h"
#include "drawing.h"
#include "input.h"
#include "common.h"
// #include "keyboard.h"

#define SOC_ALIGN 0x1000
#define SOC_BUFFERSIZE 0x100000

static u32 *SOC_buffer = NULL;
int rec;
static jmp_buf exitJmp;

int exitImmediately = 0;

int main(void)
{
	acInit();
	gfxInitDefault();
	gfxSetWide(true); // Enable wide mode
	consoleInit(GFX_TOP, NULL);

	// gfxSetDoubleBuffering(GFX_TOP, true);
	// gfxSetDoubleBuffering(GFX_BOTTOM, true);

	if (setjmp(exitJmp))
		goto exit;

	// preRenderKeyboard();

	printf("Initializing FS...\n");
	fsInit();

	printf("Initializing Socket...\n");

	// allocate buffer for SOC service
	SOC_buffer = (u32 *)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if (SOC_buffer == NULL)
	{
		errorf("memalign: failed to allocate");
		longjmp(exitJmp, 1);
	}

	if (socInit(SOC_buffer, 0x100000) < 0)
	{
		errorf("socInit: failed to initialize");
		longjmp(exitJmp, 1);
	}

	while (aptMainLoop())
	{ /* Wait for WiFi; break when WiFiStatus is truthy */
		u32 wifiStatus = 0;
		ACU_GetWifiStatus(&wifiStatus);
		if (wifiStatus)
			break;

		hidScanInput();
		consoleClear();
		printf("Waiting for WiFi connection...");
		printf("Ensure you are in range of an access point,");
		printf("and that wireless communications are enabled.");
		printf("You can alternatively press START and SELECT to exit.");

		u32 kDown = hidKeysDown();
		if ((kDown & KEY_SELECT) && (kDown & KEY_START))
		{
			exitImmediately = 1;
			longjmp(exitJmp, 1);
		}

		gfxFlushBuffers();
		gspWaitForVBlank();
		gfxSwapBuffers();
	}

	// clearScreen();
	printf("Reading settings...\n");
	// gfxFlushBuffers();
	// gfxSwapBuffers();

	if (!readSettings())
	{
		errorf("Error reading 3DSController.ini!");
		longjmp(exitJmp, 1);
	}

	// clearScreen();
	printf("Connecting to %s on port %d...\n", settings.IPString, settings.port);
	// gfxFlushBuffers();
	// gfxSwapBuffers();

	if (!openSocket(settings.port))
		longjmp(exitJmp, 1);
	sendConnectionRequest();

	// clearScreen();
	// gfxFlushBuffers();
	// gfxSwapBuffers();

	// disableBacklight();

	printf("Press START and SELECT to exit\n");

	while (aptMainLoop())
	{
		hidScanInput();
		irrstScanInput();

		u32 kHeld = hidKeysHeld();
		circlePosition circlePad;
		circlePosition cStick;
		u8 vol8;
		u8 *volp = &vol8; // As a test for pointing at things.
		hidCstickRead(&cStick);
		hidCircleRead(&circlePad);
		HIDUSER_GetSoundVolume(volp);
		u32 volume = (u32)vol8; // Upscale to 32 for transmission
		touchPosition touch;
		touchRead(&touch);

		// clearScreen();

		// if ((kHeld & KEY_L) && (kHeld & KEY_R) && (kHeld & KEY_X))
		// {
		// 	if (keyboardToggle)
		// 	{
		// 		keyboardActive = !keyboardActive;
		// 		keyboardToggle = false;

		// 		if (keyboardActive)
		// 		{
		// 			enableBacklight();
		// 		}
		// 		else
		// 		{
		// 			disableBacklight();
		// 		}
		// 	}
		// }
		// else
		// 	keyboardToggle = true;

		// if (keyboardActive)
		// {
		// 	drawKeyboard();

		// 	if (touch.px >= 1 && touch.px <= 312 && touch.py >= 78 && touch.py <= 208)
		// 	{
		// 		int x = (int)((float)touch.px * 12.0f / 320.0f);
		// 		int y = (int)((float)(touch.py - 78) * 12.0f / 320.0f);
		// 		int width = 24;
		// 		int height = 24;

		// 		if (keyboardChars[x + y * 12] == ' ')
		// 		{
		// 			while (keyboardChars[(x - 1) + y * 12] == ' ')
		// 				x--;

		// 			width = (int)(5.0f * 320.0f / 12.0f) - 1;
		// 		}

		// 		else if (keyboardChars[x + y * 12] == '\13')
		// 		{
		// 			while (keyboardChars[(x - 1) + y * 12] == '\13')
		// 				x--;
		// 			while (keyboardChars[x + (y - 1) * 12] == '\13')
		// 				y--;

		// 			width = (int)(2.0f * 320.0f / 12.0f) - 1;
		// 			height = (int)(3.0f * 320.0f / 12.0f) - 1;
		// 		}

		// 		if (keyboardChars[x + y * 12])
		// 			drawBox((int)((float)x * 320.0f / 12.0f) + 1, (int)(78.0f + (float)y * 320.0f / 12.0f) + 1, width, height, 31, 31, 0);
		// 	}
		// }

		// printf("Volume: %x", volume);
		// rec = receiveBuffer(sizeof(struct packet));
		// if (rec)
		// {
		// 	printf("Received %d bytes\n", rec);
		// }

		if ((kHeld & KEY_START) && (kHeld & KEY_SELECT))
		{
			// sendKeys(0, circlePad, touch, cStick, volume);
			exitImmediately = 1;
			longjmp(exitJmp, 1);
		}

		sendKeys(kHeld, circlePad, touch, cStick, volume);

		gfxFlushBuffers();
		gspWaitForVBlank();
		gfxSwapBuffers();
	}

exit:
	printf("shutting down...\n");
	// enableBacklight();
	printf("waiting for socExit...\n");
	// if (sock > 0)
	// 	close(sock);
	socExit();

	printf("waiting for fsExit...\n");
	svcCloseHandle(fileHandle);
	fsExit();

	printf("waiting for acExit...\n");
	acExit();

	if (!exitImmediately)
	{
		// wait for user to press buttons, this allows the user to see the console output
		printf("\nPress START and SELECT to exit\n");

		while (aptMainLoop())
		{
			gspWaitForVBlank();
			hidScanInput();

			u32 kHeld = hidKeysHeld();
			if ((kHeld & KEY_START) && (kHeld & KEY_SELECT))
				break;
		}
	}

	printf("waiting for gfxExit...\n");
	gfxExit();

	return 0;
}
