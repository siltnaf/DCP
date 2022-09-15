////////////////////////////////////////////////////////////////////////////////
/// @file     tim1_monopulse.c
/// @author   AE TEAM
/// @brief    PWM output.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////
// Define to prevent recursive inclusion
#define _TIM1_PWM1_2_C_

// Files includes
#include "tim1_pwm1_2.h"
#include "delay.h"

TIM_OCInitTypeDef  TIM_OCInitStructure;

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TIM1_MONOPULSE
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TIM_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  Timer Pin Config
/// @param  None
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM1_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#ifndef EVBOARD
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);

		GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#else
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_6);

		GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  PWM1 output mode initialization configuration
/// @note   It must be careful of the Chip Version.
/// @param  psc: Clock frequency division value.
/// @param  arr: Preloading value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM1_PWM1_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    //Enable the TIM1 clock. The default clock source is PCLK2.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    //Setting Clock Segmentation
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    ///TIM Upward Counting Mode
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);
    //Select Timer Mode: TIM Pulse Width Modulation Mode 2
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    //Setting the Pulse Value of the Capture Comparison Register to be Loaded
		TIM_OCInitStructure.TIM_Pulse = 0;
    //Output polarity: TIM output is more polar
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    //TIM_ARRPreloadConfig(TIM1, ENABLE);
    //TIM_SetCounter(TIM1, 0);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  PWM1 output mode initialization configuration
/// @note   It must be careful of the Chip Version.
/// @param  psc: Clock frequency division value.
/// @param  arr: Preloading value.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM1_PWM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    //Enable the TIM1 clock. The default clock source is PCLK2.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    //Setting Clock Segmentation
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    ///TIM Upward Counting Mode
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);
    //Select Timer Mode: TIM Pulse Width Modulation Mode 2
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    //Setting the Pulse Value of the Capture Comparison Register to be Loaded
		TIM_OCInitStructure.TIM_Pulse = 0;
    //Output polarity: TIM output is more polar
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

    //TIM_ARRPreloadConfig(TIM1, ENABLE);
    //TIM_SetCounter(TIM1, 0);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}

void PWM1_Init(void)
{
		TIM1_GPIO_Init();
 		TIM1_PWM1_Init(100 - 1, SystemCoreClock / 10000000 - 1);
}

void PWM1_Duty(u16 duty)
{
		TIM_OCInitStructure.TIM_Pulse = duty;
		WRITE_REG(TIM1->CCR1, TIM_OCInitStructure.TIM_Pulse);
}

void PWM1_On(void)
{
		SET_BIT(TIM1->CCER, TIM_CCER_CC1EN);
}

void PWM1_Off(void)
{
		CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1EN);
}

void PWM2_Init(void)
{
		TIM1_GPIO_Init();
    TIM1_PWM2_Init(100 - 1, SystemCoreClock / 10000000 - 1);
}

void PWM2_Duty(u16 duty)
{
		WRITE_REG(TIM1->CCR2, duty);
}

void PWM2_On(void)
{
		SET_BIT(TIM1->CCER, TIM_CCER_CC2EN);
}

void PWM2_Off(void)
{
		CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2EN);
}

void PWM_Test(void)
{
		u8 i;

		PWM1_Init();
		PWM2_Init();
		while (1)
		{
				for (i=0; i<101; i+=20)
				{
						PWM2_Duty(i);
						PWM2_On();
						DELAY_Ms(1000);
						PWM2_Off();
						DELAY_Ms(1000);
						UART_printf("PWM2 Duty = %d\n\r", i);
				}
		}
}

/// @}


/// @}

/// @}



