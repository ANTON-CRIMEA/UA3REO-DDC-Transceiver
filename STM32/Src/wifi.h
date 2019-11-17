#ifndef WIFI_H
#define WIFI_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define WIFI_ANSWER_BUFFER_SIZE 512
#define WIFI_COMMAND_DELAY 10
#define WIFI_COMMAND_TIMEOUT 5000
#define WIFI_FOUNDED_AP_MAXCOUNT 10
typedef enum
{
	WIFI_UNDEFINED = 0x00U,
	WIFI_NOTFOUND = 0x01U,
	WIFI_INITED = 0x02U,
	WIFI_CONNECTING = 0x03U,
	WIFI_READY = 0x04U,
	WIFI_PROCESS_COMMAND = 0x05U,
	WIFI_TIMEOUT = 0x06U,
	WIFI_FAIL = 0x07U,
} WiFiState;

typedef enum
{
	WIFI_COMM_NONE = 0x00U,
	WIFI_COMM_LISTAP = 0x01U,
	WIFI_COMM_GETSNMP = 0x02U,
	WIFI_COMM_GETIP = 0x03U,
	WIFI_COMM_GETSTATUS = 0x04U,
	WIFI_COMM_DEEPSLEEP = 0x05U,
} WiFiProcessingCommand;

extern RTC_HandleTypeDef hrtc;

extern volatile uint8_t WIFI_InitStateIndex;
extern volatile WiFiState WIFI_State;
extern volatile char WIFI_FoundedAP[10][32];

extern void WIFI_Init(void);
extern void WIFI_ProcessAnswer(void);
extern uint32_t WIFI_GetSNMPTime(void);
extern void WIFI_ListAP(void);
extern void WIFI_GetIP(void);
extern void WIFI_GetStatus(void);
extern void WIFI_GoSleep(void);

#endif
