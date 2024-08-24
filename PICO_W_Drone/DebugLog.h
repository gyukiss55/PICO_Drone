// DebugLog.h

#define SERIAL2_DEBUG_LOG_LEVEL 1
//#define SERIAL2_DEBUG_LOG_LEVEL 2

#define SERIAL_DEBUG_LOG_LEVEL 1
//#define SERIAL_DEBUG_LOG_LEVEL 2


#define DEBUG_ID_1 1
#define DEBUG_ID_2 2
#define DEBUG_ID_3 3
#define DEBUG_ID_4 4
#define DEBUG_ID_5 5
#define DEBUG_ID_6 6
#define DEBUG_ID_7 7
//#define DEBUG_ID_8 8


extern int logSerial2Request;
extern int logSerialRequest;

int CheckLogStateSerial(int id = 0);
int CheckLogStateSerial2(int id = 0);
