#ifndef FFT_h
#define FFT_h

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <math.h> 
#include "arm_math.h"

#define FFT_SIZE 256 //��������� ������ FFT
#define FFT_MAX_HEIGHT 30 //��������� ������������ ������ FFT
#define FFT_BOTTOM_OFFSET 190 //��������� ������������ ������ FFT
#define FFT_WTF_HEIGHT 50 //��������� ������������ ������ ��������
#define FFT_CONTRAST 100 //����������� ����� ������� FFT

extern void FFT_doFFT(void);
extern void FFT_printFFT(void);

extern uint32_t FFT_buff_index;
extern bool FFT_need_fft;
extern float32_t FFTInput[FFT_SIZE*2];
uint16_t getFFTColor(uint8_t height);
	
#endif
