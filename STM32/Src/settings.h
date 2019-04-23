#ifndef SETTINGS_h
#define SETTINGS_h

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdbool.h>
#include "arm_math.h"
#include "bands.h"

#define ADCDAC_CLOCK 49152000 //Частота генератора АЦП/ЦАП
#define MAX_FREQ_HZ 750000000 //Максимальная частота приёма (из даташита АЦП)
#define ADC_BITS 12 //разрядность АЦП
#define MAX_TX_AMPLITUDE 31000.0f //Максимальный размах при передаче в ЦАП (32767.0f - лимит)
#define TUNE_AMPLITUDE MAX_TX_AMPLITUDE/4.0f //Мощность при использовании кнопки TUNE
#define AGC_CLIP_THRESHOLD 14000 //Максимальный уровень усиления в AGC
#define AGC_OPTIMAL_THRESHOLD 7000 //Рабочий уровень усиления в AGC
#define TX_AGC_STEPSIZE 500.0f //Время срабатывания компрессора голосового сигнала на передачу (меньше-быстрее)
#define TX_AGC_MAXGAIN 500.0f //Максимальное усиление микрофона при компрессировании
#define TX_AGC_NOISEGATE 15.0f //Минимальный уровень сигнала для усиления (ниже - шум, отрезаем)
#define TOUCHPAD_DELAY 200 //Время защиты от анти-дребезга нажания на тачпад
#define ADC_VREF 1.0f //опорное напряжение АЦП, при подаче на вход которого АЦП отдаёт максимальное значение, вольт
#define ADC_RESISTANCE 100 //сопротивление входа АЦП, ом
#define ADC_RF_TRANS_RATIO 4 //коэффициент трансформации трансформатора :) на входе АЦП
#define ADC_RF_INPUT_VALUE_CALIBRATION 0.60f //коэффициент, на который умножаем данные с АЦП, чтобы получить реальное напряжение, устанавливается при калибровке трансивера (PREAMP включен)

#define ILI9341 true //выбираем используемый дисплей
//#define ILI9325 true //другие комментируем
#define FSMC_REGISTER_SELECT 18 //из FSMC настроек в STM32Cube (A18, A6, и т.д.)

//Данные по пропускной частоте с BPF фильтров (снимаются с помощью ГКЧ или выставляются по чувствительности)
//Далее выставляются средние пограничные частоты срабатывания
#define BPF_0_START 0
#define BPF_0_END 0
#define BPF_1_START 1600000
#define BPF_1_END 2650000
#define BPF_2_START 2650000
#define BPF_2_END 4850000
#define BPF_3_START 4850000
#define BPF_3_END 7500000
#define BPF_4_START 7500000
#define BPF_4_END 12800000
#define BPF_5_START 12800000
#define BPF_5_END 17000000
#define BPF_6_START 17000000
#define BPF_6_END 34000000
#define BPF_7_START 34000000
#define BPF_7_END MAX_FREQ_HZ

#define W25Q16_COMMAND_Write_Enable 0x06
#define W25Q16_COMMAND_Erase_Chip 0xC7
#define W25Q16_COMMAND_Sector_Erase 0x20
#define W25Q16_COMMAND_Page_Program 0x02
#define W25Q16_COMMAND_Read_Data 0x03

typedef struct {
	uint32_t Freq;
	uint8_t Mode;
	uint16_t Filter_Width;
} VFO;

extern struct TRX_SETTINGS {
	uint8_t clean_flash;
	bool current_vfo; // false - A; true - B
	VFO VFO_A;
	VFO VFO_B;
	bool AGC;
	bool Preamp;
	bool ATT;
	bool LPF;
	bool BPF;
	bool TX_Amplifier;
	uint8_t Notch;
	bool DNR;
	uint8_t Agc_speed;
	uint8_t LCD_menu_freq_index;
	bool BandMapEnabled;
	float Touchpad_ax;
	int16_t Touchpad_bx;
	float Touchpad_ay;
	int16_t Touchpad_by;
	uint8_t Volume;
	uint8_t InputType; //0 - mic ; 1 - line ; 2 - usb
	bool Mute;
	bool Fast;
	uint16_t CW_Filter;
	uint16_t SSB_Filter;
	uint16_t FM_Filter;
	uint8_t RF_Power;
	bool FFT_Enabled;
	uint8_t	FM_SQL_threshold;
	uint8_t	RF_Gain;
	uint32_t saved_freq[BANDS_COUNT];
	uint8_t FFT_Zoom;
	//system settings
	uint16_t CW_GENERATOR_SHIFT_HZ;
	uint8_t	ENCODER_SLOW_RATE;
	uint8_t LCD_Brightness;
	uint8_t Standby_Time;
	bool Beeping;
	uint16_t Key_timeout;
} TRX;

extern bool NeedSaveSettings;
extern SPI_HandleTypeDef hspi1;
extern void LoadSettings(void);
extern void SaveSettings(void);
void Flash_Sector_Erase(void);
void Flash_Erase_Chip(void);
void Flash_Write_Data(void);
void Flash_Read_Data(void);
extern VFO *CurrentVFO(void);

#endif
