#ifndef AUDIO_PROCESSOR_h
#define AUDIO_PROCESSOR_h

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "functions.h"

#define FPGA_AUDIO_BUFFER_SIZE 192 * 2
#define FPGA_AUDIO_BUFFER_HALF_SIZE FPGA_AUDIO_BUFFER_SIZE / 2

#define FM_RX_LPF_ALPHA 0.05f         // For NFM demodulator:  "Alpha" (low-pass) factor to result in -6dB "knee" at approx. 270 Hz 0.05f
#define FM_RX_HPF_ALPHA 0.96f         // For NFM demodulator:  "Alpha" (high-pass) factor to result in -6dB "knee" at approx. 180 Hz 0.96f
#define FM_TX_HPF_ALPHA 0.95f         // For FM modulator:  "Alpha" (high-pass) factor to pre-emphasis
#define FM_SQUELCH_HYSTERESIS 0.3f    // Hysteresis for FM squelch
#define FM_SQUELCH_PROC_DECIMATION 50 // Number of times we go through the FM demod algorithm before we do a squelch calculation
#define FM_RX_SQL_SMOOTHING 0.005f    // Smoothing factor for IIR squelch noise averaging

extern DMA_HandleTypeDef hdma_i2s3_ext_rx;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream1;

typedef enum
{
	AUDIO_RX1,
	AUDIO_RX2
} AUDIO_PROC_RX_NUM;

extern void processRxAudio(void);
extern void processTxAudio(void);
extern void initAudioProcessor(void);

extern volatile uint32_t AUDIOPROC_samples;
extern volatile uint32_t AUDIOPROC_TXA_samples;
extern volatile uint32_t AUDIOPROC_TXB_samples;
extern int32_t Processor_AudioBuffer_A[FPGA_AUDIO_BUFFER_SIZE];
extern int32_t Processor_AudioBuffer_B[FPGA_AUDIO_BUFFER_SIZE];
extern volatile uint_fast8_t Processor_AudioBuffer_ReadyBuffer;
extern volatile bool Processor_NeedRXBuffer;
extern volatile bool Processor_NeedTXBuffer;
extern volatile float32_t Processor_AVG_amplitude;
extern volatile float32_t Processor_TX_MAX_amplitude_IN;
extern volatile float32_t Processor_TX_MAX_amplitude_OUT;
extern volatile float32_t ALC_need_gain;
extern volatile float32_t ALC_need_gain_new;
extern float32_t FPGA_Audio_Buffer_RX1_Q_tmp[FPGA_AUDIO_BUFFER_HALF_SIZE];
extern float32_t FPGA_Audio_Buffer_RX1_I_tmp[FPGA_AUDIO_BUFFER_HALF_SIZE];
extern float32_t FPGA_Audio_Buffer_RX2_Q_tmp[FPGA_AUDIO_BUFFER_HALF_SIZE];
extern float32_t FPGA_Audio_Buffer_RX2_I_tmp[FPGA_AUDIO_BUFFER_HALF_SIZE];
extern float32_t FPGA_Audio_Buffer_TX_Q_tmp[FPGA_AUDIO_BUFFER_HALF_SIZE];
extern float32_t FPGA_Audio_Buffer_TX_I_tmp[FPGA_AUDIO_BUFFER_HALF_SIZE];
extern volatile float32_t Processor_RX_Power_value;
extern volatile float32_t Processor_selected_RFpower_amplitude;

#endif
