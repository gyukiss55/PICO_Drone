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

int ConvertWebBuffer2SharedData (const std::wstring& webBuffer, SharedDataServer& sharedData)
{
    memset ((void *)&sharedData, 0, sizeof (SharedDataServer));
    if (webBuffer.size () > 0) {
        std::wstring  webStr (webBuffer);
        std::wstring s1;
        size_t  pos = webStr.find (L"ax=");
        std::wstring::size_type sz;
        while (pos < webBuffer.size ()) {
            double ax = std::stod (webStr.substr (pos + 3), &sz);
            pos = webStr.find (L"ay=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double ay = std::stod (webStr.substr (pos + 3), &sz);
            pos = webStr.find (L"az=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double az = std::stod (webStr.substr (pos + 3), &sz);

            sharedData.gearAccMag.acc[0] = (long)ax;
            sharedData.gearAccMag.acc[1] = (long)ay;
            sharedData.gearAccMag.acc[2] = (long)az;

            pos = webStr.find (L"gx=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double gx = std::stod (webStr.substr (pos + 3), &sz);
            pos = webStr.find (L"gy=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double gy = std::stod (webStr.substr (pos + 3), &sz);
            pos = webStr.find (L"gz=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double gz = std::stod (webStr.substr (pos + 3), &sz);

            sharedData.gearAccMag.gear[0] = (long)gx * 10;
            sharedData.gearAccMag.gear[1] = (long)gy * 10;
            sharedData.gearAccMag.gear[2] = (long)gz * 10;

            pos = webStr.find (L"mx=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double mx = std::stod (webStr.substr (pos + 3), &sz);
            pos = webStr.find (L"my=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double my = std::stod (webStr.substr (pos + 3), &sz);
            pos = webStr.find (L"mz=", pos + 3 + sz, 3);
            if (pos >= webBuffer.size ())
                break;
            double mz = std::stod (webStr.substr (pos + 3), &sz);

            sharedData.gearAccMag.mag[0] = (long)mx * 10;
            sharedData.gearAccMag.mag[1] = (long)my * 10;
            sharedData.gearAccMag.mag[2] = (long)mz * 10;

            pos = webStr.find (L"quatw=", pos + 3 + sz, 6);
            if (pos >= webBuffer.size ())
                break;
            double qw = std::stod (webStr.substr (pos + 6), &sz);
            pos = webStr.find (L"quatx=", pos + 6 + sz, 6);
            if (pos >= webBuffer.size ())
                break;
            double qx = std::stod (webStr.substr (pos + 6), &sz);
            pos = webStr.find (L"quaty=", pos + 6 + sz, 6);
            if (pos >= webBuffer.size ())
                break;
            double qy = std::stod (webStr.substr (pos + 6), &sz);
            pos = webStr.find (L"quatz=", pos + 6 + sz, 6);
            if (pos >= webBuffer.size ())
                break;
            double qz = std::stod (webStr.substr (pos + 6), &sz);

            sharedData.gearAccMag.quat[0] = qx;
            sharedData.gearAccMag.quat[1] = qy;
            sharedData.gearAccMag.quat[2] = qz;
            sharedData.gearAccMag.quat[3] = qz;

            pos = webStr.find (L"Yaw=", pos + 6 + sz, 4);
            if (pos >= webBuffer.size ())
                break;
            double yaw = std::stod (webStr.substr (pos + 4), &sz);
            pos = webStr.find (L"Pitch=", pos + 4 + sz, 6);
            if (pos >= webBuffer.size ())
                break;
            double pitch = std::stod (webStr.substr (pos + 6), &sz);
            pos = webStr.find (L"Roll=", pos + 6 + sz, 5);
            if (pos >= webBuffer.size ())
                break;
            double roll = std::stod (webStr.substr (pos + 5), &sz);

            sharedData.gearAccMag.north = yaw;
            sharedData.gearAccMag.pitch = pitch;
            sharedData.gearAccMag.roll = roll;

            pos = webStr.find (L"Speed=", pos + 5 + sz, 6);
            if (pos >= webBuffer.size ())
                break;
            double speedL = std::stod (webStr.substr (pos + 6), &sz);
            pos = webStr.find (L", ", pos + 6 + sz, 2);
            if (pos >= webBuffer.size ())
                break;
            double speedR = std::stod (webStr.substr (pos + 2), &sz);

            sharedData.motorLeft.speed = speedL;
            sharedData.motorRight.speed = speedR;

            break;
        }
    }
    return 0;
}


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


int SendAndReceivePacket(const std::string& ipAddress, WORD port, const std::string& message, std::string& messageRet)
{
    const char* pkt = message.c_str();
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

    sendto(s, pkt, strlen(pkt), 0, (sockaddr*)&dest, sizeof(dest));

#define DEFAULT_BUFLEN 512

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
        recvbuf[iResult] = 0;
        messageRet += std::string(recvbuf);

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

int ConvertUDPBuffer2SharedData(const std::string& udpBuffer, SharedDataServer& sharedData)
{
    memset((void*)&sharedData, 0, sizeof(SharedDataServer));
    if (udpBuffer.size() > 0) {
        std::string s1;
        std::string::size_type sz;
        std::string findStr("Ts:");
        size_t  pos = udpBuffer.find(findStr);
        if (pos >= udpBuffer.size())
            return -1;
        pos += findStr.size();
        unsigned long ts = std::stoul(udpBuffer.substr(pos), &sz);
        pos += sz;
        sharedData.sec = ts / 1000;
        sharedData.microSec = (ts % 1000) * 1000;
        findStr = std::string("Raw: ");
        pos = udpBuffer.find(findStr);
        std::string separ (", ");
        while (pos < udpBuffer.size()) {
            pos += findStr.size();
            double ax = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double ay = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double az = std::stod(udpBuffer.substr(pos), &sz);

            sharedData.gearAccMag.acc[0] = ax;
            sharedData.gearAccMag.acc[1] = ay;
            sharedData.gearAccMag.acc[2] = az;

            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double gx = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double gy = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double gz = std::stod(udpBuffer.substr(pos), &sz);

            sharedData.gearAccMag.gear[0] = gx;
            sharedData.gearAccMag.gear[1] = gy;
            sharedData.gearAccMag.gear[2] = gz;

            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double mx = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double my = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double mz = std::stod(udpBuffer.substr(pos), &sz);

            sharedData.gearAccMag.mag[0] = mx;
            sharedData.gearAccMag.mag[1] = my;
            sharedData.gearAccMag.mag[2] = mz;
/*
            pos = udpBuffer.find("quatw=", pos + 3 + sz, 6);
            if (pos >= udpBuffer.size())
                break;
            double qw = std::stod(udpBuffer.substr(pos + 6), &sz);
            pos = udpBuffer.find("quatx=", pos + 6 + sz, 6);
            if (pos >= udpBuffer.size())
                break;
            double qx = std::stod(udpBuffer.substr(pos + 6), &sz);
            pos = udpBuffer.find("quaty=", pos + 6 + sz, 6);
            if (pos >= udpBuffer.size())
                break;
            double qy = std::stod(udpBuffer.substr(pos + 6), &sz);
            pos = udpBuffer.find("quatz=", pos + 6 + sz, 6);
            if (pos >= udpBuffer.size())
                break;
            double qz = std::stod(udpBuffer.substr(pos + 6), &sz);

            sharedData.gearAccMag.quat[0] = qx;
            sharedData.gearAccMag.quat[1] = qy;
            sharedData.gearAccMag.quat[2] = qz;
            sharedData.gearAccMag.quat[3] = qz;
*/
            std::string ori("Orientation:");
            pos = udpBuffer.find(ori, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += ori.size();
            //separ = " ";
            double yaw = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double pitch = std::stod(udpBuffer.substr(pos), &sz);
            pos = udpBuffer.find(separ, pos + sz);
            if (pos >= udpBuffer.size())
                break;
            pos += separ.size();
            double roll = std::stod(udpBuffer.substr(pos), &sz);

            sharedData.gearAccMag.north = yaw;
            sharedData.gearAccMag.pitch = pitch;
            sharedData.gearAccMag.roll = roll;
/*

            pos = udpBuffer.find("Speed=", pos + 5 + sz, 6);
            if (pos >= udpBuffer.size())
                break;
            double speedL = std::stod(udpBuffer.substr(pos + 6), &sz);
            pos = udpBuffer.find(separ, pos + 6 + sz, 2);
            if (pos >= udpBuffer.size())
                break;
            double speedR = std::stod(udpBuffer.substr(pos + 2), &sz);

            sharedData.motorLeft.speed = speedL;
            sharedData.motorRight.speed = speedR;
*/
            break;
        }
    }
    return 0;
}

int SendAndReceivePacket(const std::string& ipAddress, WORD port, SharedDataServer& sharedData)
{
    std::string sendMsg("ReadAHRS");
    std::string receiveMsg;
    int ret = SendAndReceivePacket(ipAddress, port, sendMsg, receiveMsg);
    if (receiveMsg.length() > 0) {
        ConvertUDPBuffer2SharedData(receiveMsg, sharedData);
        return 0;
    }
    return -1;
}

