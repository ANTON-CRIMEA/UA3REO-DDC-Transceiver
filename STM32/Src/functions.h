#ifndef Functions_h
#define Functions_h

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "arm_math.h"
#include "profiler.h"

#define TRX_MODE_LSB 0
#define TRX_MODE_USB 1
#define TRX_MODE_IQ 2
#define TRX_MODE_CW_L 3
#define TRX_MODE_CW_U 4
#define TRX_MODE_DIGI_L 5
#define TRX_MODE_DIGI_U 6
#define TRX_MODE_NO_TX 7
#define TRX_MODE_NFM 8
#define TRX_MODE_WFM 9
#define TRX_MODE_AM 10
#define TRX_MODE_LOOPBACK 11
#define TRX_MODE_COUNT 12

#define IRAM2 __attribute__((section("IRAM2")))

// Internal Macros
#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)

// User-visible Macros
#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) + B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) \
(((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define	TWOPI		6.28318530717958647692f
#define SQRT2   1.41421356237f

#define MINI_DELAY for(int wait_i=0;wait_i<100;wait_i++) { __asm("nop"); };

extern uint8_t FPGA_spi_data;
extern UART_HandleTypeDef huart1;

extern uint32_t getFrequencyFromPhrase(uint32_t phrase);
extern uint32_t getPhraseFromFrequency(uint32_t freq);
extern void addSymbols(char* dest, char* str, uint8_t length, char* symbol, bool toEnd);
extern void sendToDebug_str(char* str);
extern void sendToDebug_str2(char* data1, char* data2);
extern void sendToDebug_str3(char* data1, char* data2, char* data3);
extern void sendToDebug_newline(void);
extern void sendToDebug_flush(void);
extern void sendToDebug_uint8(uint8_t data, bool _inline);
extern void sendToDebug_uint16(uint16_t data, bool _inline);
extern void sendToDebug_uint32(uint32_t data, bool _inline);
extern void sendToDebug_int8(int8_t data, bool _inline);
extern void sendToDebug_int16(int16_t data, bool _inline);
extern void sendToDebug_int32(int32_t data, bool _inline);
extern void sendToDebug_float32(float32_t data, bool _inline);
extern void sendToDebug_hex(uint8_t data, bool _inline);
extern void delay_us(uint32_t us);
extern bool beetween(float32_t a, float32_t b, float32_t val);
extern float32_t log10f_fast(float32_t X);
extern void readHalfFromCircleBuffer32(uint32_t *source, uint32_t *dest, uint32_t index, uint32_t length);
extern void readHalfFromCircleUSBBuffer(int16_t *source, int32_t *dest, uint16_t index, uint16_t length);
extern void dma_memcpy32(uint32_t dest, uint32_t src, uint32_t len);
extern float32_t db2rateV(float32_t i);
extern float32_t db2rateP(float32_t i);
extern float32_t rate2dbV(float32_t i);
extern float32_t rate2dbP(float32_t i);
extern void shiftTextLeft(char *string, int16_t shiftLength);
extern float32_t getMaxTXAmplitudeOnFreq(uint32_t freq);
extern float32_t generateSin(float32_t amplitude, uint32_t index, uint32_t samplerate, uint32_t freq);

#endif
