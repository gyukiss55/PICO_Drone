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

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_WEB_BUFLEN (512 * 4)
#define DEFAULT_PORT "8080"
#define DEFAULT_WEB_PORT "80"
#define DEFAULT_UDP_PORT 8888


/*
int WinWebSocketClient (const std::wstring& ipAddressStr, SharedDataServer& sharedData)
{
    memset ((void*)&sharedData, 0, sizeof (SharedDataServer));
    // Set URL and call back function.
    WinHttpClient client (ipAddressStr);
    client.SendHttpRequest ();
    wstring httpResponseHeader = client.GetHttpResponseHeader ();
    wstring httpResponse = client.GetHttpResponse ();
    ConvertWebBuffer2SharedData (httpResponse, sharedData);
    return 0;
}
*/

/*
int WinSocketClient (const char* ipAddressStr, SharedDataServer& sharedData)
{
    WSADATA wsaData;

    memset ((void*)&sharedData, 0, sizeof (SharedDataServer));
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "Get Ballance Control Data";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    char logStr[DEFAULT_BUFLEN];

    // Initialize Winsock
    iResult = WSAStartup (MAKEWORD (2, 2), &wsaData);
    if (iResult != 0) {
        snprintf (logStr, DEFAULT_BUFLEN, "WSAStartup failed with error: %d\n", iResult);
        OutputDebugStringA (logStr);
        return 1;
    }

    ZeroMemory (&hints, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo (ipAddressStr, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        snprintf (logStr, DEFAULT_BUFLEN, "getaddrinfo failed with error: %d\n", iResult);
        OutputDebugStringA (logStr);
        WSACleanup ();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket (ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            snprintf (logStr, DEFAULT_BUFLEN, "socket failed with error: %ld\n", WSAGetLastError ());
            OutputDebugStringA (logStr);
            WSACleanup ();
            return 1;
        }

        // Connect to server.
        iResult = connect (ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket (ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo (result);

    if (ConnectSocket == INVALID_SOCKET) {
        OutputDebugStringA ("Unable to connect to server!\n");
        WSACleanup ();
        return 1;
    }

    // Send an initial buffer
    iResult = send (ConnectSocket, sendbuf, (int)strlen (sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        snprintf (logStr, DEFAULT_BUFLEN, "send failed with error: %d\n", WSAGetLastError ());
        OutputDebugStringA (logStr);
        closesocket (ConnectSocket);
        WSACleanup ();
        return 1;
    }

    printf ("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown (ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        snprintf (logStr, DEFAULT_BUFLEN, "shutdown failed with error: %d\n", WSAGetLastError ());
        closesocket (ConnectSocket);
        WSACleanup ();
        return 1;
    }

    // Receive until the peer closes the connection
    //do {

    iResult = recv (ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult == sizeof (SharedDataServer)) {
        const SharedDataServer* ptrSharedDataServer = (const SharedDataServer*)recvbuf;
        if (ptrSharedDataServer->sizeOfData == sizeof (SharedDataServer)) {
            sharedData = *ptrSharedDataServer;
            OutputDebugStringA (ptrSharedDataServer->nameOfData);
        }
    }
    if (iResult > 0)
        snprintf (logStr, DEFAULT_BUFLEN, "\nBytes received: %d\n", iResult);
    else if (iResult == 0)
        snprintf (logStr, DEFAULT_BUFLEN, "\nConnection closed\n");
    else
        snprintf (logStr, DEFAULT_BUFLEN, "\nrecv failed with error: %d\n", WSAGetLastError ());
    OutputDebugStringA (logStr);

    //} while (iResult > 0 && iResult < DEFAULT_BUFLEN);

    // cleanup
    closesocket (ConnectSocket);
    WSACleanup ();

    return 0;
}
*/


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
#define DEFAULT_BUFLEN 1024

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
        if (iResult % sizeof(DroneSensorData) == 0) {
            uint32_t num = iResult / sizeof(DroneSensorData);
            DroneSensorData* ptrRecv = (DroneSensorData*)&recvbuf[0];
            for (droneSensorDataVectorIndex1 = 0; droneSensorDataVectorIndex1 < num && droneSensorDataVectorIndex1 < DroneSensorDataVectorSize1; droneSensorDataVectorIndex1++) {
                droneSensorDataVector1[droneSensorDataVectorIndex1] = *ptrRecv;
                ptrRecv++;
            }
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


