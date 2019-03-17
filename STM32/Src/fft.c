#include "fft.h"
#include "lcd.h"
#include <stdlib.h>
#include "arm_math.h"
#include "arm_const_structs.h"
#include "functions.h"
#include "audio_processor.h"
#include "wm8731.h"
#include "settings.h"

uint32_t FFT_buff_index = 0;
float32_t FFTInput_A[FFT_SIZE * 2] = { 0 };
float32_t FFTInput_B[FFT_SIZE * 2] = { 0 };
bool FFTInputBufferInProgress = false; //false - A, true - B
float32_t FFTOutput[FFT_SIZE];

uint8_t FFT_status;
const static arm_cfft_instance_f32 *S = &arm_cfft_sR_f32_len512;

uint16_t wtf_buffer[FFT_WTF_HEIGHT][FFT_PRINT_SIZE] = { 0 };

uint32_t maxIndex = 0; // Индекс элемента массива с максимальной амплитудой в результирующей АЧХ
float32_t maxValue = 0; // Максимальное значение амплитуды в результирующей АЧХ
float32_t meanValue = 0; // Среднее значение амплитуды в результирующей АЧХ
float32_t maxValueFFT = 0; // Максимальное значение амплитуды в результирующей АЧХ
float32_t diffValue = 0; // Разница между максимальным значением в FFT и пороге в водопаде
uint16_t height = 0; //высота столбца в выводе FFT
uint16_t maxValueErrors = 0; //количество превышений сигнала в FFT
uint16_t tmp = 0;
uint8_t fft_compress_rate = FFT_SIZE / FFT_PRINT_SIZE;
float32_t fft_compress_tmp = 0;

bool FFT_need_fft = true; //необходимо полдготовить данные для отображения на экран

void FFT_doFFT(void)
{
	if (!TRX.FFT_Enabled) return;
	if (!FFT_need_fft) return;
	if (FFTInputBufferInProgress) //B in progress
	{
		for (int i = 0; i < FFT_SIZE; i++) //Hanning window
		{
			double multiplier = (float32_t)0.5 * ((float32_t)1 - arm_cos_f32(2 * PI*i / FFT_SIZE));
			FFTInput_A[i * 2] = multiplier * FFTInput_A[i * 2];
			FFTInput_A[i * 2 + 1] = multiplier * FFTInput_A[i * 2 + 1];
		}
		arm_cfft_f32(S, FFTInput_A, 0, 1);
		arm_cmplx_mag_f32(FFTInput_A, FFTOutput, FFT_SIZE);
		//arm_cmplx_mag_squared_f32(FFTInput_A, FFTOutput, FFT_SIZE);
	}
	else //A in progress
	{
		for (int i = 0; i < FFT_SIZE; i++) //Hanning window
		{
			double multiplier = (float32_t)0.5 * ((float32_t)1 - arm_cos_f32(2 * PI*i / FFT_SIZE));
			FFTInput_B[i * 2] = multiplier * FFTInput_B[i * 2];
			FFTInput_B[i * 2 + 1] = multiplier * FFTInput_B[i * 2 + 1];
		}
		arm_cfft_f32(S, FFTInput_B, 0, 1);
		arm_cmplx_mag_f32(FFTInput_B, FFTOutput, FFT_SIZE);
		//arm_cmplx_mag_squared_f32(FFTInput_B, FFTOutput, FFT_SIZE);
	}
	//Autocalibrate MIN and MAX on FFT
	arm_max_f32(FFTOutput, FFT_SIZE, &maxValue, &maxIndex); //ищем максимум в АЧХ
	arm_mean_f32(FFTOutput, FFT_SIZE, &meanValue); //ищем среднее в АЧХ
	
	diffValue=(maxValue-maxValueFFT)/FFT_STEP_COEFF;
	if (maxValueErrors >= FFT_MAX_IN_RED_ZONE && diffValue>0) maxValueFFT+=diffValue;
	else if (maxValueErrors <= FFT_MIN_IN_RED_ZONE && diffValue<0 && diffValue<-FFT_STEP_FIX) maxValueFFT+=diffValue;
	else if (maxValueErrors <= FFT_MIN_IN_RED_ZONE && maxValueFFT>FFT_STEP_FIX) maxValueFFT-=FFT_STEP_FIX;
	else if (maxValueErrors <= FFT_MIN_IN_RED_ZONE && diffValue<0 && diffValue<-FFT_STEP_PRECISION) maxValueFFT+=diffValue;
	else if (maxValueErrors <= FFT_MIN_IN_RED_ZONE && maxValueFFT>FFT_STEP_PRECISION) maxValueFFT-=FFT_STEP_PRECISION;
	if((meanValue*4)>maxValueFFT) maxValueFFT=(meanValue*4);
	//sendToDebug_float32(maxValueErrors); //красных пиков на водопаде (перегрузок)
	//sendToDebug_float32(diffValue); //разница между максимальным и пороговым в FFT
	//sendToDebug_float32(maxValue); //максимальное в выборке
	//sendToDebug_float32(meanValue); //среднее в выборке
	//sendToDebug_float32(maxValueFFT); //максимальный порог в отображаемом FFT
	//sendToDebug_str("\r\n");
	maxValueErrors = 0;
	if (maxValueFFT < FFT_MIN) maxValueFFT = FFT_MIN;
	//
	// Нормируем АЧХ к единице
	for (uint16_t n = 0; n < FFT_SIZE; n++)
	{
		FFTOutput[n] = FFTOutput[n] / maxValueFFT;
		if (FFTOutput[n] > 1) FFTOutput[n] = 1;
	}
	//Compress FFT_SIZE to FFT_PRINT_SIZE
	for (uint16_t n = 0; n < FFT_PRINT_SIZE; n++)
	{
		fft_compress_tmp = 0;
		for (uint8_t c = 0; c < fft_compress_rate; c++)
			fft_compress_tmp += FFTOutput[n*fft_compress_rate + c];
		FFTOutput[n] = fft_compress_tmp / fft_compress_rate;
		if (FFTOutput[n] > 1) FFTOutput[n] = 1;
	}
	//
	FFT_need_fft = false;
}

void FFT_printFFT(void)
{
	if (LCD_busy) return;
	if (!TRX.FFT_Enabled) return;
	if (TRX_ptt || TRX_tune || TRX_getMode() == TRX_MODE_LOOPBACK) return;
	if (FFT_need_fft) return;
	if (LCD_mainMenuOpened) return;
	if (LCD_modeMenuOpened) return;
	if (LCD_bandMenuOpened) return;
	LCD_busy = true;

	//смещаем водопад вниз c помощью DMA
	for (tmp = FFT_WTF_HEIGHT - 1; tmp > 0; tmp--)
	{
		HAL_DMA_Start(&hdma_memtomem_dma2_stream7, (uint32_t)&wtf_buffer[tmp - 1], (uint32_t)&wtf_buffer[tmp], sizeof(wtf_buffer[tmp - 1])/4);
		HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream7, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	}

	uint8_t new_x = 0;
	
	for (uint32_t fft_x = 0; fft_x < FFT_PRINT_SIZE; fft_x++)
	{
		if (fft_x < (FFT_PRINT_SIZE / 2)) new_x = fft_x + (FFT_PRINT_SIZE / 2);
		if (fft_x >= (FFT_PRINT_SIZE / 2)) new_x = fft_x - (FFT_PRINT_SIZE / 2);
		if ((new_x + 1) == FFT_PRINT_SIZE / 2) continue;
		height = FFTOutput[(uint16_t)fft_x] * FFT_MAX_HEIGHT;
		if (height > FFT_MAX_HEIGHT - 1)
		{
			height = FFT_MAX_HEIGHT;
			tmp = COLOR_RED;
			maxValueErrors++;
		}
		else
			tmp = getFFTColor(height);
		wtf_buffer[0][new_x] = tmp;
		ILI9341_drawFastVLine(new_x + 1, FFT_BOTTOM_OFFSET, -FFT_MAX_HEIGHT - 1, COLOR_BLACK);
		ILI9341_drawFastVLine(new_x + 1, FFT_BOTTOM_OFFSET, -height, tmp);
	}

	//разделитель и полоса приёма
	ILI9341_drawFastVLine(FFT_PRINT_SIZE / 2, FFT_BOTTOM_OFFSET, -FFT_MAX_HEIGHT, COLOR_GREEN);
	switch(CurrentVFO()->Mode)
	{
		case TRX_MODE_LSB:
		case TRX_MODE_CW_L:
		case TRX_MODE_DIGI_L:
			ILI9341_drawFastHLine(FFT_PRINT_SIZE / 2, FFT_BOTTOM_OFFSET-FFT_MAX_HEIGHT-1, -CurrentVFO()->Filter_Width/FFT_HZ_IN_PIXEL, COLOR_GREEN);
			break;
		case TRX_MODE_USB:
		case TRX_MODE_CW_U:
		case TRX_MODE_DIGI_U:
			ILI9341_drawFastHLine(FFT_PRINT_SIZE / 2, FFT_BOTTOM_OFFSET-FFT_MAX_HEIGHT-1, CurrentVFO()->Filter_Width/FFT_HZ_IN_PIXEL, COLOR_GREEN);
			break;
		case TRX_MODE_NFM:
		case TRX_MODE_AM:
			ILI9341_drawFastHLine(FFT_PRINT_SIZE / 2, FFT_BOTTOM_OFFSET-FFT_MAX_HEIGHT-1, CurrentVFO()->Filter_Width/FFT_HZ_IN_PIXEL/2, COLOR_GREEN);
			ILI9341_drawFastHLine(FFT_PRINT_SIZE / 2, FFT_BOTTOM_OFFSET-FFT_MAX_HEIGHT-1, -CurrentVFO()->Filter_Width/FFT_HZ_IN_PIXEL/2, COLOR_GREEN);
			break;
		default:
			break;
	}
	
	//выводим на экран водопада с помощью DMA
	ILI9341_SetCursorAreaPosition(1, FFT_BOTTOM_OFFSET, FFT_PRINT_SIZE, FFT_BOTTOM_OFFSET + FFT_WTF_HEIGHT);
	HAL_DMA_Start(&hdma_memtomem_dma2_stream6, (uint32_t)&wtf_buffer, 0x60080000, FFT_WTF_HEIGHT*FFT_PRINT_SIZE);
	HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream6, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	
	ILI9341_drawFastVLine(FFT_PRINT_SIZE / 2, FFT_BOTTOM_OFFSET, FFT_PRINT_SIZE, COLOR_GREEN);
	
	FFT_need_fft = true;
	LCD_busy = false;
}

void FFT_moveWaterfall(int16_t freq_diff)
{
	int16_t new_x = 0;
	freq_diff = freq_diff / FFT_HZ_IN_PIXEL;

	for (uint8_t y = 0; y < FFT_WTF_HEIGHT; y++)
	{
		if (freq_diff > 0) //freq up
		{
			for (int16_t x = 0; x <= FFT_PRINT_SIZE; x++)
			{
				new_x = x + freq_diff;
				if (new_x<0 || new_x>=FFT_PRINT_SIZE)
				{
					wtf_buffer[y][x] = 0;
					continue;
				};
				wtf_buffer[y][x] = wtf_buffer[y][new_x];
			}
		}
		else if (freq_diff < 0) // freq down
		{
			for (int16_t x = FFT_PRINT_SIZE; x >= 0; x--)
			{
				new_x = x + freq_diff;
				if (new_x<=0 || new_x>FFT_PRINT_SIZE)
				{
					wtf_buffer[y][x] = 0;
					continue;
				};
				wtf_buffer[y][x] = wtf_buffer[y][new_x];
			}
		}
	}
}

uint16_t getFFTColor(uint8_t height)
{
	//r g b
	//0 0 0
	//0 0 255
	//255 255 0
	//255 0 0

	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	if (height <= FFT_MAX_HEIGHT / 3)
	{
		blue = (height * 255 / (FFT_MAX_HEIGHT / 3));
	}
	else if (height <= 2 * FFT_MAX_HEIGHT / 3)
	{
		green = ((height - FFT_MAX_HEIGHT / 3) * 255 / (FFT_MAX_HEIGHT / 3));
		red = green;
		blue = 255 - green;
	}
	else
	{
		red = ((height - 2 * FFT_MAX_HEIGHT / 3) * 255 / (FFT_MAX_HEIGHT / 3));
		blue = 255 - red;
		green = 255 - red;
	}
	return rgb888torgb565(red, green, blue);
}
