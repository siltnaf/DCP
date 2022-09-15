////////////////////////////////////////////////////////////////////////////////
/// @file    tim2.h
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FIRMWARE FUNCTIONS.
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
#ifndef __TIM14_H
#define __TIM14_H

// Files includes

#include "hal_conf.h"
#include <string.h>

#include "mm32_device.h"
#include "hal_device.h"
#include "hal_rcc.h"

#define TP1_Port   		GPIOD
#define TP1_Pin    		GPIO_Pin_7
#define TP2_Port   		GPIOC
#define TP2_Pin    		GPIO_Pin_14
#define TP3_Port   		GPIOC
#define TP3_Pin    		GPIO_Pin_15

#define TP1_LO()			GPIO_ResetBits(TP1_Port,TP1_Pin)
#define TP1_HI()			GPIO_SetBits(TP1_Port,TP1_Pin)
#define TP1_TOGGLE()	(GPIO_ReadOutputDataBit(TP1_Port,TP1_Pin))?(GPIO_ResetBits(TP1_Port,TP1_Pin)):(GPIO_SetBits(TP1_Port,TP1_Pin))

#define TP2_LO()			GPIO_ResetBits(TP2_Port,TP2_Pin)
#define TP2_HI()			GPIO_SetBits(TP2_Port,TP2_Pin)
#define TP2_TOGGLE()	(GPIO_ReadOutputDataBit(TP2_Port,TP2_Pin))?(GPIO_ResetBits(TP2_Port,TP2_Pin)):(GPIO_SetBits(TP2_Port,TP2_Pin))

#define TP3_LO()			GPIO_ResetBits(TP3_Port,TP3_Pin)
#define TP3_HI()			GPIO_SetBits(TP3_Port,TP3_Pin)
#define TP3_TOGGLE()	(GPIO_ReadOutputDataBit(TP3_Port,TP3_Pin))?(GPIO_ResetBits(TP3_Port,TP3_Pin)):(GPIO_SetBits(TP3_Port,TP3_Pin))

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Example_Layer
/// @brief MM32 Example Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_RESOURCE
/// @brief MM32 Examples resource modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Constants
/// @{


/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Enumeration
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief XXXX enumerate definition.
/// @anchor XXXX
////////////////////////////////////////////////////////////////////////////////


/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Variables
/// @{
#ifdef _TIMER14_C_
#define GLOBAL







#else
#define GLOBAL extern







#endif




#undef GLOBAL

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Functions
/// @{




void TIM14_UPCount_Init(u16 arr, u16 psc);
void TIM14_UPStatusOVCheck(void);
void Keypad_Timer_Init(void);


/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
