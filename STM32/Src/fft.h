#ifndef FFT_h
#define FFT_h

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <math.h> 
#include "arm_math.h"

#define FFT_SIZE 512 //��������� ������ FFT
#define FFT_PRINT_SIZE 256 //��������� ������ FFT
#define FFT_MAX_HEIGHT 30 //��������� ������������ ������ FFT
#define FFT_BOTTOM_OFFSET 190 //start of FFT
#define FFT_WTF_HEIGHT 50 //��������� ������������ ������ ��������
#define FFT_MIN 0.000001f //MIN ����� ������� FFT
#define FFT_MAX 120 //MAX ����� ������� FFT
#define FFT_HZ_IN_PIXEL 187 // 48000/256

extern void FFT_doFFT(void);
extern void FFT_printFFT(void);
extern void FFT_moveWaterfall(int16_t freq_diff);

extern uint32_t FFT_buff_index;
extern bool FFTInputBufferInProgress;
extern bool FFT_need_fft;
extern float32_t FFTInput_A[FFT_SIZE * 2];
extern float32_t FFTInput_B[FFT_SIZE * 2];
uint16_t getFFTColor(uint8_t height);
	
#endif
