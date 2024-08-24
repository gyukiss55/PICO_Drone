// AsyncUDPServer.cpp

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

#include "AsyncUDPServer.h"
#include "DroneSensorData.h"
#include "DebugLog.h"

#ifndef STASSID
#define STASSID "RTAX999"
#define STAPSK "LiDoDa#959285$"
#endif

#define UDPPORT 8888

const char* ssid = STASSID;
const char* password = STAPSK;

AsyncUDP udp;

void setupAsyncUDPServer()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while (1) {
            delay(1000);
        }
    }
    if (udp.listen(UDPPORT)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            static int cnt = 0;
            cnt++;
            if (cnt == 10) {
                cnt = 0;

#if defined SERIAL_DEBUG_LOG_LEVEL
                if (CheckLogStateSerial(DEBUG_ID_1) > 0) {
                    Serial.print("UDP Packet Type: ");
                    Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
                    Serial.print(", From: ");
                    Serial.print(packet.remoteIP());
                    Serial.print(":");
                    Serial.print(packet.remotePort());
                    Serial.print(", To: ");
                    Serial.print(packet.localIP());
                    Serial.print(":");
                    Serial.print(packet.localPort());
                    Serial.print(", Length: ");
                    Serial.print(packet.length());
                    Serial.print(", Data: ");
                    Serial.write(packet.data(), packet.length());
                    Serial.println();
                }
#endif
            }
            //reply to the client
            CloneDroneSensorDataVector();
            //packet.printf("Got %u bytes of data.", packet.length());
            //packet.printf("Send DroneSensorDataVector %u/%u bytes of data:", sizeof (droneSensorDataVector2), sizeof(droneSensorDataVector2[0]));
//            Serial.println("ts3 " + String(droneSensorDataVector2[0].GetTimeStamp()) + " ms"); // debug log
            packet.write((uint8_t *) &droneSensorDataVector2[0], sizeof(droneSensorDataVector2));
            });
    }
}

void loopAsyncUDPServer()
{
    delay(1000);
    //Send broadcast
    udp.broadcast("Anyone here?");
}
