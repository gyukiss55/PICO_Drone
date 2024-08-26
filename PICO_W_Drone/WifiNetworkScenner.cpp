// WifiNetworkScenner.cpp


// Simple WiFi network scanner application
// Released to the public   domain in 2022 by Earle F. Philhower, III
#include <Arduino.h>
#include <WiFi.h>
#include <string>
#include "WifiNetworkScenner.h"

const char* macToString(uint8_t mac[6])
{
    static char s[20];
    sprintf(s, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return s;
}

const   char* encToString(uint8_t enc)
{
    switch (enc) {
    case ENC_TYPE_NONE:   return "NONE";
    case ENC_TYPE_TKIP: return "WPA";
    case ENC_TYPE_CCMP:   return "WPA2";
    case ENC_TYPE_AUTO: return "AUTO";
    }
    return   "UNKN";
}

int WifiNetworkScenner(const char* kownWifiConnections[])
{
    int result = -1;
    int rssiMax = -200;
    delay(5000);
    Serial.printf("Beginning   scan at %d\r\n", millis());
    auto cnt = WiFi.scanNetworks();
    if (!cnt) {
        Serial.printf("No networks found\r\n");
    }
    else {
        Serial.printf("Found %d networks\r\n", cnt);
        Serial.printf("%32s   %5s %17s %2s %4s\r\n", "SSID", "ENC", "BSSID        ", "CH", "RSSI");
        for (auto i = 0; i < cnt; i++) {
            uint8_t bssid[6];
            WiFi.BSSID(i, bssid);
            char stat = ' ';
            for (auto j = 0; kownWifiConnections[j] != nullptr; j += 2) {
                std::string ssid1(kownWifiConnections[j]);
                std::string ssid2(WiFi.SSID(i));
                int rssi = WiFi.RSSI(i);
                if (ssid1 == ssid2 && rssi > rssiMax) {
                    rssiMax = rssi;
                    result = j;
                    stat = '#';
                }
            }
            Serial.printf("%c%32s %5s %17s %2d %4d\r\n", stat, WiFi.SSID(i), encToString(WiFi.encryptionType(i)), macToString(bssid), WiFi.channel(i), WiFi.RSSI(i));
        }
    }
//    Serial1.printf("\r\n--- Sleeping ---\r\n\r\n\r\n");
//    delay(5000);
    return result;
}