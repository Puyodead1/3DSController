#include <errno.h>

// #include "keyboard.h"

#include "common.h"
#include "wireless.h"

int sock;
struct sockaddr_in sain, saout;
struct packet outBuf, rcvBuf;

socklen_t sockaddr_in_sizePtr = (int)sizeof(struct sockaddr_in);

bool openSocket(int port)
{
	int res;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		errorf("Socket creation failed: %s\n", strerror(errno));
		return false;
	}

	saout.sin_family = sain.sin_family = AF_INET;
	saout.sin_port = sain.sin_port = htons(port);
	sain.sin_addr.s_addr = INADDR_ANY;

	res = bind(sock, (struct sockaddr *)&sain, sizeof(sain));
	if (res < 0)
	{
		errorf("bind() failed: %d\n", strerror(errno));
		return false;
	}

	res = fcntl(sock, F_SETFL, O_NONBLOCK);
	if (res < 0)
	{
		errorf("fcntl() failed: %d\n", strerror(errno));
		return false;
	}

	return true;
}

void sendBuf(int length)
{
	int result = sendto(sock, (char *)&outBuf, length, 0, (struct sockaddr *)&saout, sizeof(saout));
	// if (result < 0)
	// {
	// 	printf("sendto() failed: %d\n", result);
	// }
}

int receiveBuffer(int length)
{
	return recvfrom(sock, (char *)&rcvBuf, length, 0, (struct sockaddr *)&sain, &sockaddr_in_sizePtr);
}

void sendConnectionRequest(void)
{
	outBuf.command = CONNECT;
	outBuf.keyboardActive = false;
	sendBuf(offsetof(struct packet, connectPacket) + sizeof(struct connectPacket));
}

void sendKeys(unsigned int keys, circlePosition circlePad, touchPosition touch, circlePosition cStick, unsigned int volume)
{
	outBuf.command = KEYS;
	outBuf.keyboardActive = false;
	memcpy(&outBuf.keys, &keys, 4);
	memcpy(&outBuf.circlePad, &circlePad, 4);
	memcpy(&outBuf.touch, &touch, 4);
	memcpy(&outBuf.cStick, &cStick, 4);
	memcpy(&outBuf.volume, &volume, 4);
	sendBuf(offsetof(struct packet, keysPacket) + sizeof(struct keysPacket));
}
