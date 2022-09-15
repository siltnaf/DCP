////////////////////////////////////////////////////////////////////////////////
/// @file    led.c
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
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
#define _GPIO_C_

// Files includes

#include "gpio.h"
#include "lcd_grp1.h"				//? Add


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup GPIO
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup GPIO_Exported_Constants
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @brief  initialize #1215 GPIO pin
/// @note   if use jtag/swd interface GPIO PIN as #1215, need to be careful,
///         can not debug or program.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void PORT_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

#ifndef EVBOARD
    GPIO_InitStructure.GPIO_Pin = 0xFE38;					//PB3-5,9-15 Output, 1111 1110 0011 1000
#else
		GPIO_InitStructure.GPIO_Pin = 0x0238;					//PB3-5,9 Output, 0000 0010 0011 1000
#endif
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifndef EVBOARD
    GPIO_InitStructure.GPIO_Pin = 0x3FFF;					//PC0-PC13 Output, 0011 1111 1111 1111
#else
		GPIO_InitStructure.GPIO_Pin = 0xFFFF;					//PC0-PC15 Output, 1111 1111 1111 1111
#endif
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#ifndef EVBOARD
    GPIO_InitStructure.GPIO_Pin = 0x04C;					//PD2,3,6 Output, 0000 0000 0100 1100
#else
		GPIO_InitStructure.GPIO_Pin = 0x00CC;					//PD2,3,6,7 Output, 0000 0000 1100 1100
#endif
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_Write(GPIOC, 0x3F00);
		
		GPIO_SetBits(GPIOB, GPIO_Pin_3);				//PB3 = 1
		GPIO_SetBits(GPIOB, GPIO_Pin_4);				//PB4 = 1
		GPIO_SetBits(GPIOB, GPIO_Pin_5);				//PB5 = 1
		
		GPIO_ResetBits(GPIOD, GPIO_Pin_7);			//TP1 = 0
		GPIO_ResetBits(GPIOC, GPIO_Pin_14);			//TP2 = 0 
		GPIO_ResetBits(GPIOC, GPIO_Pin_15);			//TP3 = 0
}

/// @}

/// @}

/// @}
