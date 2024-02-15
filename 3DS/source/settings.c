#include <stdio.h>
#include <malloc.h>

#include <3ds.h>

#include "wireless.h"

#include "settings.h"

struct settings settings;

struct settings defaultSettings = {
	IPString : "",
	port : DEFAULT_PORT,
};

Handle fileHandle;

static bool getSetting(char *name, char *src, char *dest)
{
	char *start = strstr(src, name);

	if (start)
	{
		start += strlen(name);

		char *end = start + strlen(start);
		if (strstr(start, "\n") - 1 < end)
			end = strstr(start, "\n") - 1;
		size_t size = (size_t)end - (size_t)start;

		strncpy(dest, start, size);
		dest[size] = '\0';

		return true;
	}

	return false;
}

bool readSettings(void)
{
	Result ret;
	char *buffer = NULL;
	u64 size;
	u32 bytesRead;

	static const char settingsFilename[] = "/3ds/3DSController.ini";
	FS_Path apath = {PATH_EMPTY, 1, ""};
	FS_Path fpath = {PATH_ASCII, sizeof(settingsFilename), settingsFilename};

	ret = FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, apath, fpath, FS_OPEN_READ, 0);
	if (R_FAILED(ret))
		return false;

	ret = FSFILE_GetSize(fileHandle, &size);
	if (R_FAILED(ret))
	{
		FSFILE_Close(fileHandle);
		return false;
	}

	buffer = malloc(size);
	if (!buffer)
	{
		FSFILE_Close(fileHandle);
		return false;
	}

	ret = FSFILE_Read(fileHandle, &bytesRead, 0x0, buffer, size);
	FSFILE_Close(fileHandle);
	if (R_FAILED(ret))
	{
		free(buffer);
		return false;
	}

	memcpy(&settings, &defaultSettings, sizeof(struct settings));

	char setting[64] = {'\0'};

	if (getSetting("IP: ", buffer, settings.IPString))
	{
		// inet_pton(AF_INET, settings.IPString, &(saout.sin_addr));
		inet_pton4(settings.IPString, (unsigned char *)&(saout.sin_addr));
	}
	else
	{
		free(buffer);
		return false;
	}

	if (getSetting("Port: ", buffer, setting))
	{
		sscanf(setting, "%d", &settings.port);
	}

	free(buffer);

	return true;
}
