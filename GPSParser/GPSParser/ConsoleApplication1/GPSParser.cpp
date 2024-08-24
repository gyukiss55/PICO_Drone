// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <windows.h>
#include <debugapi.h>

struct GPSdata {
    std::string timeHMS;
    std::string timeYMD;
    std::string latitude;
    std::string NS;
    std::string longitude;
    std::string EW;
    std::string fix; // 1 - fix, 0 - invalid
    std::string nr; // 1 - fix, 0 - invalid
    std::string valid; // A - valid, V - invalid
    std::string satelits;
    std::string altitudeSee;
    std::string altitudeGeo;
    std::string accuracy;
};

void DumpResult(const GPSdata& d) {
    if (d.timeHMS.length() > 0) {
        OutputDebugStringA("timeHMS:");
        OutputDebugStringA(d.timeHMS.c_str());
        OutputDebugStringA(",");
    }
    if (d.valid.length() > 0) {
        OutputDebugStringA("valid:");
        OutputDebugStringA(d.valid.c_str());
        OutputDebugStringA(",");
    }
}

bool GPSParser(const char* strGPSIno, GPSdata& result)
{
    std::string s(strGPSIno);
    if (s.find("$GPGGA") == 0) {
        size_t p1 = s.find(',');
        size_t p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.timeHMS = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.latitude = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.NS = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.longitude = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.EW = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.fix = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.nr = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.altitudeSee = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.altitudeGeo = s.substr(p1 + 1, p2 - p1 - 1);
        }
    }
    else if (s.find("$GPGSA") == 0) {

    }
    else if (s.find("$GPRMC") == 0) {
        size_t p1 = s.find(',');
        size_t p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.timeHMS = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.valid = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.latitude = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.NS = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.longitude = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.EW = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            //result.speed = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            //result.empty = s.substr(p1 + 1, p2 - p1 - 1);
        }
        p1 = p2;
        p2 = s.find(',', p1 + 1);
        if (p2 - p1 > 1) {
            result.timeYMD = s.substr(p1 + 1, p2 - p1 - 1);
        }
    }
    return true;
}

const char* inputString[] = { 
    {"$GPRMC,190037.00,A,4735.75538,N,01903.66812,E,0.199,,240824,,,A*7C"},
    {"$GPGGA,190037.00,4735.75538,N,01903.66812,E,1,06,2.93,120.8,M,39.4,M,,*52"},
    {"$GPGSA,A,3,15,24,23,30,12,10,,,,,,,3.41,2.93,1.74*0C"},
    {"$GPRMC,190038.00,A,4735.75541,N,01903.66800,E,0.153,,240824,,,A*78"},
    {"$GPGGA,190038.00,4735.75541,N,01903.66800,E,1,06,2.93,120.8,M,39.4,M,,*50"},
    {"$GPGSA,A,3,15,24,23,30,12,10,,,,,,,3.41,2.93,1.74*0C"},
    {"$GPGGA,190114.00,4735.75560,N,01903.67082,E,1,06,2.94,122.2,M,39.4,M,,*50"},
    {"$GPGSA,A,3,15,24,23,30,12,10,,,,,,,3.42,2.94,1.75*09"},
    {"$GPRMC,190115.00,A,4735.75554,N,01903.67088,E,0.079,,240824,,,A*72"},
    {"$GPGGA,190115.00,4735.75554,N,01903.67088,E,1,06,2.94,122.2,M,39.4,M,,*5C"},
    {"$GPGSA,A,3,15,24,23,30,12,10,,,,,,,3.42,2.94,1.75*09"},
    {"$GPRMC,190116.00,A,4735.75546,N,01903.67099,E,0.011,,240824,,,A*7C"},
    nullptr
};


int main()
{
    for (int i = 0; inputString[i] != nullptr; i++) {
        GPSdata result;
        if (GPSParser(inputString[i], result))
            DumpResult(result);
    }
}

