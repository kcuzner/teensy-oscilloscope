#include "adc.h"

#include "common.h"
#include "arm_cm4.h"

/**
 * Note: ADC clock frequency must be 1-18Mhz for <=13bit modes, 2-12Mhz for 16-bit modes
 * These are based on a f_BUS of 36Mhz (72Mhz / 2)
 */
#define ADC_CFG1_16BIT  (ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(1)) // 9 MHz
#define ADC_CFG1_12BIT  (ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(1)) // 18 MHz
#define ADC_CFG1_10BIT  (ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(1)) // 18 MHz
#define ADC_CFG1_8BIT   (ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(1)) // 18 MHz

void adc_init(uint8_t bits)
{
    uint16_t sum;

    VREF_TRM = 0x60;
	VREF_SC = 0xE1;		// enable 1.2 volt ref

	if (bits == 8)
    {
		ADC0_CFG1 = ADC_CFG1_8BIT + ADC_CFG1_MODE(0);
		ADC0_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(3);
		ADC1_CFG1 = ADC_CFG1_8BIT + ADC_CFG1_MODE(0);
		ADC1_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(3);
	}
	else if (bits == 10)
    {
		ADC0_CFG1 = ADC_CFG1_10BIT + ADC_CFG1_MODE(2) + (1 << ADC_CFG1_ADLSMP_SHIFT);
		ADC0_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(3);
		ADC1_CFG1 = ADC_CFG1_10BIT + ADC_CFG1_MODE(2) + (1 << ADC_CFG1_ADLSMP_SHIFT);
		ADC1_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(3);
	}
	else if (bits == 12)
    {
		ADC0_CFG1 = ADC_CFG1_12BIT + ADC_CFG1_MODE(1) + (1 << ADC_CFG1_ADLSMP_SHIFT);
		ADC0_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(2);
		ADC1_CFG1 = ADC_CFG1_12BIT + ADC_CFG1_MODE(1) + (1 << ADC_CFG1_ADLSMP_SHIFT);
		ADC1_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(2);
	}
	else
    {
		ADC0_CFG1 = ADC_CFG1_16BIT + ADC_CFG1_MODE(3) + (1 << ADC_CFG1_ADLSMP_SHIFT);
		ADC0_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(2);
		ADC1_CFG1 = ADC_CFG1_16BIT + ADC_CFG1_MODE(3) + (1 << ADC_CFG1_ADLSMP_SHIFT);
		ADC1_CFG2 = (1 << ADC_CFG2_MUXSEL_SHIFT) + ADC_CFG2_ADLSTS(2);
	}

    ADC0_SC2 = ADC_SC2_REFSEL(1); // 1.2V ref
    ADC1_SC2 = ADC_SC2_REFSEL(1); // 1.2V ref

    ADC0_SC3 = ADC_SC3_CAL_MASK;
	ADC1_SC3 = ADC_SC3_CAL_MASK;  // begin cal

	while ((ADC0_SC3 & ADC_SC3_CAL_MASK) || (ADC1_SC3 & ADC_SC3_CAL_MASK))
    {
		// wait
	}

    sum = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 + ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;
    sum = (sum / 2) | 0x8000;
    ADC0_PG = sum;
    sum = ADC0_CLMS + ADC0_CLM4 + ADC0_CLM3 + ADC0_CLM2 + ADC0_CLM1 + ADC0_CLM0;
    sum = (sum / 2) | 0x8000;
    ADC0_MG = sum;
	sum = ADC1_CLPS + ADC1_CLP4 + ADC1_CLP3 + ADC1_CLP2 + ADC1_CLP1 + ADC1_CLP0;
    sum = (sum / 2) | 0x8000;
    ADC1_PG = sum;
    sum = ADC1_CLMS + ADC1_CLM4 + ADC1_CLM3 + ADC1_CLM2 + ADC1_CLM1 + ADC1_CLM0;
    sum = (sum / 2) | 0x8000;
    ADC1_MG = sum;


}

void adc_set_chan0(uint8_t chan)
{
    ADC0_SC1A = chan & ADC_SC1_ADCH_MASK;
}

void adc_set_chan1(uint8_t chan)
{
    ADC1_SC1A = chan & ADC_SC1_ADCH_MASK;
}

void ADC0_IRQHandler(void)
{

}

void ADC1_IRQHandler(void)
{

}
