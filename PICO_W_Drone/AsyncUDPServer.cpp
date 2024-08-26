// AsyncUDPServer.cpp

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

#include "AsyncUDPServer.h"
#include "DroneControlData.h"
#include "DroneSensorData.h"
#include "MotorControl.h"
#include "DebugLog.h"
#include "WifiNetworkScenner.h"

#ifndef STASSID
#define STASSID "RTAX999_EXT"
#define STAPSK "LiDoDa#959285$"
#endif

const char* WifiConnections[] = {
    "HUAWEI P30", "6381bf07b666",
    "RTAX999", "LiDoDa#959285$",
    "RTAX999_EXT", "LiDoDa#959285$",
    "ASUS_98_2G", "LiDoDa#959285$",
    nullptr, nullptr
};

#define UDPPORT 8888

const char* ssid = STASSID;
const char* password = STAPSK;

AsyncUDP udp;

void setupAsyncUDPServer()
{
    Serial.begin(115200);

    int ssidIndex = WifiNetworkScenner(WifiConnections);
    if (ssidIndex < 0) {
        Serial.println("WiFi Failed");
        while (1) {
            delay(1000);
        }
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WifiConnections[ssidIndex], WifiConnections[ssidIndex + 1]);
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
                if (packet.length() == sizeof(DroneControlData)) {
                    currentControl = *(const DroneControlData*)packet.data();
                }
            }
            //reply to the client
            CloneDroneSensorDataVector();
            //packet.printf("Got %u bytes of data.", packet.length());
            //packet.printf("Send DroneSensorDataVector %u/%u bytes of data:", sizeof (droneSensorDataVector2), sizeof(droneSensorDataVector2[0]));
//            Serial.println("ts3 " + String(droneSensorDataVector2[0].GetTimeStamp()) + " ms"); // debug log
            packet.write((uint8_t *) &droneExportData, sizeof(droneExportData));

            if (currentControl.GetTimeStamp() != previousControl.GetTimeStamp()) {
                previousControl = currentControl;
                uint32_t s1, s2, s3, s4;
                if (previousControl.GetSpeed(s1, s2, s3, s4)) {
                    SetSpeedMotor(s1,s2,s3,s4);
                    Serial.print("Speed: ");
                    Serial.print(s1);
                    Serial.print(", ");
                    Serial.print(s2);
                    Serial.print(", ");
                    Serial.print(s3);
                    Serial.print(", ");
                    Serial.println(s4);

               }
            }

            });
    }
}

void loopAsyncUDPServer()
{
    static uint32_t lastTS = 0;
    uint32_t currTS = millis ();
    if (currTS - lastTS < 1000)
        return;
    lastTS = currTS;
    //Send broadcast
    udp.broadcast("Anyone here?");
}
