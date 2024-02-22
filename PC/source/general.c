#include "wireless.h"

#include "general.h"

void error(const char *functionName)
{
	char errorMsg[92];
	ZeroMemory(errorMsg, 92);

	sprintf(errorMsg, "Call to %s returned error %d!", (char *)functionName, WSAGetLastError());

	MessageBox(NULL, errorMsg, "socketIndication", MB_OK);

	closesocket(client);
	closesocket(listener);
	WSACleanup();

	exit(0);
}

int clamp(int val, int min, int max)
{
	if (val < min)
	{
		return min;
	}

	if (val > max)
	{
		return max;
	}

	return val;
}
double compensateDeadzone(double inputValue, double deadzoneThreshold, double minValue, double maxValue)
{
	if (fabs(inputValue) < deadzoneThreshold)
	{
		// If the input value is within the deadzone, treat it as zero
		return 0.0;
	}
	else
	{
		// Otherwise, scale and return the input value within the adjusted range
		double range = maxValue - minValue;
		double scaledValue = (inputValue - minValue) / range * 56.0 + 7.0;
		return scaledValue;
	}
}