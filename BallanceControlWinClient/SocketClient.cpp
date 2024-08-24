#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <comutil.h>
#include <Winhttp.h>

#include "SocketClient.h"
#include "WinHttpClient.h"
#include "DroneSensorData.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_WEB_BUFLEN (512 * 4)
#define DEFAULT_PORT "8080"
#define DEFAULT_WEB_PORT "80"
#define DEFAULT_UDP_PORT 8888


int SendAndReceivePacket(const std::string& ipAddress, WORD port, SharedDataServer& sharedData)
{
    const char* destIP = ipAddress.c_str();
    const char* srcIP = IP_ADDRESS_WEB_SERVER;;

    sockaddr_in dest;
    sockaddr_in local;
    WSAData data;

    WSAStartup(MAKEWORD(2, 2), &data);

    local.sin_family = AF_INET;
    inet_pton(AF_INET, srcIP, &local.sin_addr.s_addr);
    local.sin_port = htons(0);

    dest.sin_family = AF_INET;
    inet_pton(AF_INET, destIP, &dest.sin_addr.s_addr);
    dest.sin_port = htons(port);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind(s, (sockaddr*)&local, sizeof(local));

    sendto(s, (const char *) &sharedData.droneControlData, sizeof(DroneControlData), 0, (sockaddr*)&dest, sizeof(dest));

//#define DEFAULT_BUFLEN 512
#define DEFAULT_BUFLEN 2048

    char recvbuf[DEFAULT_BUFLEN];
//    int iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Receive until the peer shuts down the connection

    int ret = 0;
    int iResult = 0;

    //	do {

    iResult = recv(s, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
        if (iResult == sizeof (DroneExportData)) {
            droneExportData = *(const DroneExportData*)&recvbuf[0];
            droneSensorDataVectorIndex = 10;
        }

    }
    else if (iResult == 0)
        printf("Connection closing...\n");
    else {
        printf("recv failed: %d\n", WSAGetLastError());
        ret = 1;
    }

    //	} while (iResult > 0);

    closesocket(s);
    WSACleanup();

    return ret;
}


