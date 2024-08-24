// GAMDraw.cpp : Defines the entry point for the application.
//




#include "framework.h"
#include "BallanceControlWinClient.h"
#include <stdio.h>
#include <array>
#include <map>
#include <string>
#include <debugapi.h>

#include "SocketClient.h"
#include "GAMDraw.h"
#include "GearAccMag.h"
#include "DroneSensorData.h"
#include "CalcOrientation.h"

#define COLOR_RED		RGB (255, 0, 0)
#define COLOR_GREEN		RGB (0, 255, 0)
#define COLOR_BLUE		RGB (0, 0, 255)
#define COLOR_YELLOW	RGB (255, 255, 0)
#define COLOR_BLACK		RGB (0, 0, 0)

#define SERVER_IP_ADDRESS		"192.168.50.173"
//#define UDPSERVER_IP_ADDRESS	"192.168.50.173"
#define UDPSERVER_IP_ADDRESS	"192.168.50.114"
#define UDPSERVER_PORT			8888
#define LOG_BUFFER_LENGTH		512

#define SIZEOFGAMARRAY 4000



static std::array<GearAccMagItem, SIZEOFGAMARRAY> arrayGAM;
static std::map<COLORREF, HPEN> mapHPEN;
UINT32 indexGAM = 0;
UINT32 numGAM = 0;


bool GetPenObject (COLORREF color, HPEN& hPen)
{
	auto it = mapHPEN.find (color);
	if (it == mapHPEN.end ()) {
		HPEN hPen = CreatePen (PS_SOLID, 2, color);
		mapHPEN[color] = hPen;
		it = mapHPEN.find (color);
	}
	if (it != mapHPEN.end ()) {
		hPen = it->second;
		return true;
	}
	return false;
}

void ReleasePens ()
{
	for (auto& i : mapHPEN)
		DeleteObject (i.second);
}



void ReadSocketClient (HWND hWnd)
{
	static SharedDataServer sharedDataPrev;
	if (numGAM == 0)
		sharedDataPrev.cycle = 0;
    SharedDataServer sharedData;
	std::string ipAddress (UDPSERVER_IP_ADDRESS);
//	std::string ipAddress (SERVER_IP_ADDRESS);
//    WinWebSocketClient (ipAddress, sharedData);
	SendAndReceivePacket(ipAddress, UDPSERVER_PORT, sharedData);
//	if (sharedData.cycle == sharedDataPrev.cycle)
//		return;

	for (size_t i = 0; i < droneSensorDataVectorIndex; i++) {
		const DroneSensorData& sd = droneExportData.droneSensorDataVector[i];
		if (sd.GetTimeStamp() <= sharedDataPrev.lastTimestamp) {
			continue;
		}
		sharedDataPrev.lastTimestamp = sd.GetTimeStamp();
		GearAccMagItem item;
		sd.GetAccel(item.acc[0], item.acc[1], item.acc[2]);
		sd.GetGyro(item.gear[0], item.gear[1], item.gear[2]);
		sd.GetMag(item.mag[0], item.mag[1], item.mag[2]);
		sd.GetHorizontalDir(item.horizontalDir);
		sd.GetElevationUS(item.elevationUS);
		sd.GetElevationBaro(item.elevationBaro);
		sd.GetBatteryMotor(item.batteryMotor);
		sd.GetBatteryMPU(item.batteryMPU);

		item.microsec = (sd.GetTimeStamp() % 1000) * 1000;
		item.sec = sd.GetTimeStamp() / 1000;
	//	item.pitch = sharedData.gearAccMag.pitch;
	//	item.roll = sharedData.gearAccMag.roll;
	//	item.north = sharedData.gearAccMag.north;

	//	item.speed[0] = sharedData.motorLeft.speed;
	//	item.speed[1] = sharedData.motorRight.speed;

		arrayGAM[indexGAM] = item;
		indexGAM++;
		if (indexGAM >= SIZEOFGAMARRAY)
			indexGAM = 0;
		if (numGAM < SIZEOFGAMARRAY)
			numGAM++;
	}
	{ // debug log
		std::string log;
		for (size_t i = 0; i < droneSensorDataVectorIndex; i++) {
			if (i == 0)
				log = "Ts:";
			log += std::to_string(droneExportData.droneSensorDataVector[i].GetTimeStamp());
			if (i < droneSensorDataVectorIndex - 1)
				log += ", ";
			else
				log += "\r\n";
		}
		OutputDebugStringA(log.c_str ());
		std::string gpsStr = droneExportData.droneGPSData.GetGPGGA();
		if (gpsStr.length() > 0) {
			OutputDebugStringA(gpsStr.c_str());
		}
		gpsStr = droneExportData.droneGPSData.GetGPGSA();
		if (gpsStr.length() > 0) {
			OutputDebugStringA(gpsStr.c_str());
		}
		gpsStr = droneExportData.droneGPSData.GetGPGSA();
		if (gpsStr.length() > 0) {
			OutputDebugStringA(gpsStr.c_str());
		}
	}
	
	InvalidateRect (hWnd, nullptr, FALSE);

    char logStr[LOG_BUFFER_LENGTH];
    snprintf (logStr, LOG_BUFFER_LENGTH, "Cycle read: %d  length: %d \n ", sharedData.cycle, sharedData.droneSensorDataVectorNumber * sizeof(DroneSensorData));
    OutputDebugStringA (logStr);
}



void DrawLine (HWND /*hWnd*/, HDC hDC, INT32 x1, INT32 x2, double v1, double v2, COLORREF color, UINT32 offsY, INT32 sizeY, double minY, double maxY)
{
	INT32 y1 = (INT32)((v1 - minY) * sizeY / (maxY - minY)) + offsY;
	INT32 y2 = (INT32)((v2 - minY) * sizeY / (maxY - minY)) + offsY;

	HPEN hPen = 0;
	HGDIOBJ hPenPrev = 0;
	if (GetPenObject (color, hPen)) {
		hPenPrev = SelectObject (hDC, (HGDIOBJ)hPen);
		MoveToEx (hDC, x1, y1, nullptr);
		LineTo (hDC, x1, y1);
		LineTo (hDC, x2, y2);
		SelectObject (hDC, hPenPrev);
	}
}

static unsigned int cnt = 0;

void DrawCurves (HWND hWnd, HDC hDC, UINT32 indexIn, UINT32 num)
{
	RECT rect;
	GetClientRect (hWnd, &rect);

	INT32 index1 = indexGAM - 2;
	INT32 index2 = indexGAM - 1;
	if (index1 < 0 || index2 < 0)
		return;
	INT32 x1 = indexIn % rect.right;
	INT32 h = rect.bottom / 5;
	RECT rectPart;
	rectPart = rect;
	rectPart.left = x1 + 1;
	rectPart.right = x1 + 11;
	FillRect (hDC, &rectPart, (HBRUSH)GetStockObject (WHITE_BRUSH));
	GearAccMagItem gam_tmp2;
	GearAccMagItem gam_tmp1;
	gam_tmp1 = arrayGAM[index1];
	CalcOrientation(gam_tmp1);
	for (UINT32 i = 1; i < num; i++) {
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].acc[0], arrayGAM[index2].acc[0],   COLOR_RED,   h, h, -1., 1.);
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].acc[1], arrayGAM[index2].acc[1],   COLOR_GREEN, h, h, -1., 1.);
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].acc[2], arrayGAM[index2].acc[2],   COLOR_BLUE,  h, h, -1., 1.);
//		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].speed[0], arrayGAM[index2].speed[0], COLOR_BLACK, 0, h, -10., 10.);

		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].gear[0], arrayGAM[index2].gear[0], COLOR_RED,   0, h, -500., 500.);
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].gear[1], arrayGAM[index2].gear[1], COLOR_GREEN, 0, h, -500., 500.);
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].gear[2], arrayGAM[index2].gear[2], COLOR_BLUE,  0, h, -500., 500.);
		 
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].mag[0], arrayGAM[index2].mag[0],   COLOR_RED,   2 * h, h, -100., 100.);
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].mag[1], arrayGAM[index2].mag[1],   COLOR_GREEN, 2 * h, h, -100., 100.);
		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].mag[2], arrayGAM[index2].mag[2],   COLOR_BLUE,  2 * h, h, -100., 100.);

		gam_tmp2 = arrayGAM[index2];
		CalcOrientation(gam_tmp2);
		DrawLine (hWnd, hDC, x1, x1 + 1, gam_tmp1.pitch,  gam_tmp2.pitch,    COLOR_RED,   3 * h, h, -180., 180.);
		DrawLine (hWnd, hDC, x1, x1 + 1, gam_tmp1.roll,   gam_tmp2.roll,     COLOR_GREEN, 3 * h, h, -180., 180.);
		DrawLine (hWnd, hDC, x1, x1 + 1, gam_tmp1.north,  gam_tmp2.north,    COLOR_BLUE,  3 * h, h, -180., 180.);
		gam_tmp1 = gam_tmp2;

//		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].quat[0], arrayGAM[index2].quat[0], COLOR_RED,    4 * h, h, -1., 1.);
//		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].quat[1], arrayGAM[index2].quat[1], COLOR_GREEN,  4 * h, h, -1., 1.);
//		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].quat[2], arrayGAM[index2].quat[2], COLOR_BLUE,   4 * h, h, -1., 1.);
//		DrawLine (hWnd, hDC, x1, x1 + 1, arrayGAM[index1].quat[3], arrayGAM[index2].quat[3], COLOR_YELLOW, 4 * h, h, -1., 1.);
	}
	x1 = rect.right - 500;
	INT32 y1 = rect.bottom - 120;
	INT32 dy = 20;
	std::wstring text;
	text = L"Acc: " + std::to_wstring (arrayGAM[index1 + 1].acc[0]) + L", " + std::to_wstring (arrayGAM[index1 + 1].acc[1]) + L", " + std::to_wstring (arrayGAM[index1 + 1].acc[2]) + L"   ";
	TextOut (hDC, x1, y1, text.c_str (), text.size ());
	y1 += dy;
	text = L"Gyr: " + std::to_wstring (arrayGAM[index1 + 1].gear[0]) + L", " + std::to_wstring (arrayGAM[index1 + 1].gear[1]) + L", " + std::to_wstring (arrayGAM[index1 + 1].gear[2]) + L"   ";
	TextOut (hDC, x1, y1, text.c_str (), text.size ());
	y1 += dy;
	text = L"Mag: " + std::to_wstring (arrayGAM[index1 + 1].mag[0]) + L", " + std::to_wstring (arrayGAM[index1 + 1].mag[1]) + L", " + std::to_wstring (arrayGAM[index1 + 1].mag[2]) + L"   ";
	TextOut (hDC, x1, y1, text.c_str (), text.size ());
	y1 += dy;
	text = L"Yaw: " + std::to_wstring (gam_tmp2.north) + L", Roll: " + std::to_wstring (gam_tmp2.roll) + L", Pitch: " + std::to_wstring (gam_tmp2.pitch) + L"   ";
	TextOut (hDC, x1, y1, text.c_str (), text.size ());
	y1 += dy;
	text = L"ElevUS: " + std::to_wstring (arrayGAM[index1 + 1].elevationUS) + L", ElevBM: " + std::to_wstring (arrayGAM[index1 + 1].elevationBaro) + L", BatteryMotor: " + std::to_wstring (arrayGAM[index1 + 1].batteryMotor) + L", BatteryMPU: " + std::to_wstring(arrayGAM[index1 + 1].batteryMPU) + L"   ";
	TextOut (hDC, x1, y1, text.c_str (), text.size ());
	y1 += dy;
	//text = L"Quat: " + std::to_wstring (arrayGAM[index1 + 1].quat[0]) + L", " + std::to_wstring (arrayGAM[index1 + 1].quat[1]) + L", " + std::to_wstring (arrayGAM[index1 + 1].quat[2]) + L", " + std::to_wstring (arrayGAM[index1 + 1].quat[3]) + L"   ";
	text = L"Ts: " + std::to_wstring(arrayGAM[index1 + 1].sec) + L":" + std::to_wstring(arrayGAM[index1 + 1].microsec) + L" cnt:" + std::to_wstring(++cnt);
	TextOut (hDC, x1, y1, text.c_str (), text.size ());
	y1 += dy;
	//text = L"Speed: " + std::to_wstring (arrayGAM[index1 + 1].speed[0]) + L", " + std::to_wstring (arrayGAM[index1 + 1].speed[1]) + L"   ";
	//TextOut (hDC, x1, y1, text.c_str (), text.size ());
}

static UINT32 indexGlobal = 0;

void GAMDraw (HWND hWnd, HDC hdc)
{
	if (numGAM < 2)
		return;

	UINT32 index1 = SIZEOFGAMARRAY - 1;
	if (indexGAM > 1)
		DrawCurves (hWnd, hdc, indexGlobal, numGAM);

	indexGlobal++;
}
