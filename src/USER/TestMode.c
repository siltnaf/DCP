/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file TestMode.c
       \brief Routines for reading and writing to EEPROM

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright ?2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/eeprom.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "TestMode.h"
#include "uart.h"
#include "string.h"
#include "can.h"
#include "eeprom.h"
#include "adc18x.h"
#include "fonts.h"
#include "iconblinker.h"
#include "lcd_grp1.h"
#include "gpio.h"
#include "delay.h"
#include "pwm18x.h"
#include "i2c.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#define STEP_IO_CHECK                   0
#define STEP_WAIT_IO_CHECK_FINISHED     1
#define STEP_DISPLAY_ICONS              2
#define STEP_WAIT_KEYPAD_TEST           3
#define STEP_WAIT_CAN_PING              4
#define STEP_CAN_PING_RESPONSE          5
#define STEP_WAIT_CAN_RTC               6
#define STEP_CAN_RTC_RESPONSE           7
#define STEP_WAIT_CAN_EEPROM            8
#define STEP_CAN_EEPROM_RESPONSE        9
#define STEP_WAIT_CAN_READ_12VDC        10
#define STEP_CAN_READ_12VDC_RESPONSE    11
#define STEP_WAIT_CAN_READ_TEMP         12
#define STEP_CAN_READ_TEMP_RESPONSE     13
#define STEP_END                        14

#define ALT_OFF_SET()         GPIO_SetBits(GPIOD, GPIO_Pin_6)
#define ALT_OFF_RESET()       GPIO_ResetBits(GPIOD, GPIO_Pin_6)
#define APU_RUN_SET()         GPIO_SetBits(GPIOD, GPIO_Pin_2)
#define APU_RUN_RESET()       GPIO_ResetBits(GPIOD, GPIO_Pin_2)
#define APU_START_SET()       GPIO_SetBits(GPIOD, GPIO_Pin_3)
#define APU_START_RESET()     GPIO_ResetBits(GPIOD, GPIO_Pin_3)

// pwm1 -> PWM_CCU_HEAT_AC
// pwm2 -> PWM_CCU_FAN_OUTLET
u8 test_mode_flag = 0;
u8 test_step = 0;
u8 test_io_check_finished_flag = 0;

/* ---------- I n t e r n a l   D a t a ---------- */
static u8 test_eeprom_data[8] = {'2', '0', '2', '2', '0', '7', '0', '8'};
static u8 test_eeprom_backup[8];

/* ---------- G l o b a l   D a t a ---------- */
/* ----------------- F u n c t i o n s ----------------- */
void test_read_all_adc_channel(u16* buff);
u8 test_rtc_read(u8* buff);

/******************************************************************************/
/*!

<!-- FUNCTION NAME: TestMode -->

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->

*/
/******************************************************************************/
void test_gpio_init(){
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    ALT_OFF_RESET();
    APU_RUN_RESET();
    APU_START_RESET();
    pwm1_on();
    pwm1(0);
    pwm2_on();
    pwm2(0);
    LCD_BKLT_1;
}

void tset_show_title(){
    lcd_cla( 0, 131, 0, 7 );
    lcd_position(0,0,small);
    printf("DCP FIRMWARE TEST MODE");
    lcd_position(0,1,small);
    printf("25-737-51A");
}

void test_show_main_view(){
    //static u8 refresh_time = 0;
    u16 adc_raw_buff[5];
    //u8 buff[8];
    //u8 i;
    float ccu, apu, an_temp, an_12vdc;
    //UART_printf("refreshed\r\n");
    
    test_read_all_adc_channel(adc_raw_buff);
    //for(i = 0;i < 4; i++){
    //    UART_printf("%d\r\n", adc_raw_buff[i]);
    //}
    ccu = adc_raw_buff[0] * 5.0 / 1023;
    apu = adc_raw_buff[1] * 5.0 / 1023;
    an_temp = adc_raw_buff[3] * 5.0 / 1023;
    an_12vdc = adc_raw_buff[2] * 5.0/ 1023;
    lcd_position(0,3,small);
    printf("CCU_VAC_BR = %.2fV", ccu);
    lcd_position(0,4,small);
    printf("APU_GLOW_FAULT = %.2fV", apu);
    lcd_position(0,5,small);
    printf("DCP_AN_TEMP = %.2fV", an_temp);
    lcd_position(0,6,small);
    printf("DCP_AN_12VDC = %.2fV", an_12vdc);
    lcd_position(0,7,small);
    printf("00:00:00");
    if(apu > 2.8){
        //pwm2_on();
        pwm2(100);
        //UART_printf("fan pwm outlet 5V\r\n");
    } else if(apu > 1.8){
        //pwm2_on();
        pwm2(80);
        //UART_printf("fan pwm outlet 4V\r\n");
    } else if(apu > 0.8){
        //pwm2_on();
        pwm2(40);
        //UART_printf("fan pwm outlet 2V\r\n");
    }
    if(ccu > 2.3){
        //pwm1_on();
        pwm1(100);
        //UART_printf("heat ac pwm 5V\r\n");
    } else if(ccu > 1.3){
        //pwm1_on();
        pwm1(60);
        //UART_printf("heat ac pwm 3V\r\n");
    } else if(ccu > 0.3){
        //pwm1_on();
        pwm1(20);
        //UART_printf("heat ac pwm 1V\r\n");
    }
    if(ccu > 2.3 && an_temp < 4){
        if(an_temp > 2.8){
            ALT_OFF_RESET();
            APU_START_RESET();
            APU_RUN_SET();
            //UART_printf("ALT_RUN = 1 else = 0\r\n");
        } else if(an_temp > 0.8){
            if(ccu > 2.8){
                ALT_OFF_SET();
                APU_START_RESET();
                APU_RUN_RESET();
                //UART_printf("ALT_OFF = 1 else = 0\r\n");
            } else {
                ALT_OFF_RESET();
                APU_START_SET();
                APU_RUN_RESET();
                //UART_printf("APU_START = 1 else = 0\r\n");
            }
        }
    }
    if(/*refresh_time > 5 || */(ccu > 3.8 && apu > 3.8)){
        test_io_check_finished_flag = 1;
        //pwm2(0);
        //pwm1(0);
        //refresh_time = 0;
        return;
    }
    //refresh_time += 1;
    
}

void test_display_all_icons(){
    u8 i = 0;
    UART_printf("show all icons\r\n");
    lcd_cla( 0, 131, 0, 7);
    iconMakeVisible(true);
    for(i = 0; i < 16; i++){
        if(i == 2 || i == 5 || (i <12 && i > 7)) continue;
        iconControl(i, ICON_ON);
        iconShow(i);
    }
}

void test_hide_icons(u8 key){
    switch(key){
        case 0:
            iconErase(4);
            iconErase(1);
            break;
        case 1:
            iconErase(13);
            break;
        case 2:
            iconErase(3);
            break;
        case 3:
            iconErase(0);
            break;
        case 4:
            iconErase(12);
            break;
        case 5:
            iconErase(7);
            break;
        case 6:
            iconErase(6);
            break;
        case 7:
            iconErase(15);
            break;
        case 8:
            iconErase(14);
            break;
        default:
            break;
        
    }
}

u8 test_read_keypad(){
    u8 key;
    u8 row, column;
#if 1
    u16 PortB, PortB_out;
    u16 tmp;
    key = 0xff;
    PortB = ReadOutputLatch(GPIOB);
    for(column = 0; column < 3; column++){
        PortB_out = PortB | 0x0038;
        tmp = 1 << (column + 3);
        tmp = ~tmp;
        PortB_out &= tmp;
        WritePort(GPIOB, PortB_out);
        //UART_printf("%x\r\n", PortB);
        DELAY_Ms(5);
        for(row = 0; row < 3; row++){
            if(!GPIO_ReadInputDataBit(GPIOB, 1 << row)){
                DELAY_Ms(5);
                if(!GPIO_ReadInputDataBit(GPIOB, 1 << row)){
                    while(!GPIO_ReadInputDataBit(GPIOB, 1 << row));
                    UART_printf("key %d %d\r\n", row, column);
                    key = column + row * 3;
                    return key;
                }
            }
        }
    }
 #else
    u8 i, j; 
    u8 cnt[3];
    u16 PortB, out, tmp;
    u16 read;
    key = 0xff;
    PortB = ReadOutputLatch(GPIOB);
    //UART_printf("scan\r\n");
    for(column = 0; column < 3; column++){
        out = PortB | 0x0038;
        tmp = 1 << (column + 3);
        memset(cnt, 0, 3);
        for(i = 0; i < 3; i++){
            out = out & (~tmp);
            WritePort(GPIOB, out);
            //UART_printf("out %x\r\n", out&0x0038);
            //DELAY_Us(100);
            DELAY_Ms(5);
            for(row = 0; row < 3; row++){
                if(!GPIO_ReadInputDataBit(GPIOB, 1 << row)){
                    cnt[row] += 1;
                }
            }
            for(j = 0; j < 3; j++){
                if(cnt[j] == 3){
                    row = j;
                    key = column + row * 3; 
                    //UART_printf("key %d %d\r\n", row, column);
                }
            }
            out = PortB | 0x0038;
            WritePort(GPIOB, out);
            //DELAY_Us(100);
            DELAY_Ms(5);
        }
        
    }
 #endif
    return key;
}

u8 test_wait_can_cmd(u8* data){
    //static u16 duty1 = 0;
    //static u16 duty2 = 0;
    //static u8 toggle = 0;
    u8 cmd_type = 0xff;
    can_Msg_Struct* command;
    command = can_ReadRxQueue();
    if(command != NULL && data != NULL){
        if(command->Id != 0x18ea8080){
            can_PullRxQueue();
            return cmd_type;
        }
        cmd_type = command->Data[0];
        memcpy(data, command->Data, command->Dlc);
        can_PullRxQueue();
        //UART_printf("command %d\r\n", cmd_type);
#if 0
        if(cmd_type == 8){
            duty1 += 20;
            if(duty1 > 100){
                duty1 = 0;
                pwm1(duty1);
                pwm1_off();
                UART_printf("pwm1 off\r\n");
            } else if(duty1 > 0){
                pwm1_on();
                pwm1(duty1);
                UART_printf("pwm1 duty %d\r\n", duty1);
            }
        }
        if(cmd_type == 9){
            duty2 += 20;
            if(duty2 > 100){
                duty2 = 0;
                pwm2(duty2);
                pwm2_off();
                UART_printf("pwm2 off\r\n");
            } else if(duty2 > 0){
                pwm2_on();
                pwm2(duty2);
                UART_printf("pwm2 duty %d\r\n", duty2);
            }
        }
        if(cmd_type == 7){
            toggle = !toggle;
            if(toggle){
                ALT_OFF_SET();
                UART_printf("ALT_OFF = 1\r\n");
            } else {
                ALT_OFF_RESET();
                UART_printf("ALT_OFF = 0\r\n");
            }
        }
#endif
    }
    return cmd_type;
}

u8 test_can_send_ack(u32 id, u8* data){
    can_Msg_Struct* message;
    message = can_ReadTxQueue();
    if(message == NULL){
        message = can_WriteTxQueue();
        message->Id = id;
        message->Ide = 1;   // Extended ID Type
        message->Rtr = 0;
        message->Dlc = 8;
        memcpy(message->Data, data, 8);
        can_PushTxQueue();
        return 1;
    }
    return 0;
}

void test_rtc_write(u8* buff){
// peripheral (rtc write) address 0xde
    buff[0] = 0x02;
    I2C_SetDeviceAddr(I2C1, 0xde);
    EEPROM_WriteBuff_Sub2B(I2C1,  0x3f, buff, 1);
    buff[0] = 0x06;
    EEPROM_WriteBuff_Sub2B(I2C1,  0x3f, buff, 1);
    
    buff[0] = 0x00;     //ss
    buff[1] = 0x00;     //mm
    buff[2] = 0x80;     //hh
    buff[3] = 0x08;     //dd
    buff[4] = 0x07;     //MM
    buff[5] = 0x22;     //YY
    buff[6] = 0x05;     //day of week
    buff[7] = 0x20;     //YY
    EEPROM_WriteBuff_Sub2B(I2C1,  0x30, buff, 8);
}

u8 test_rtc_read(u8* buff){
// peripheral (rtc read) address 0xdf
    //u8 i = 0;
    if(buff == NULL) return 0;
    memset(buff, 0, 8);
    I2C_SetDeviceAddr(I2C1, 0xdf);
    EEPROM_ReadBuff_Sub2B(I2C1,  0x30, buff, 8);
    if(buff[7] == 0x20 && buff[5] == 0x22 && buff[4] == 0x07 && buff[3] == 0x08 && buff[2] == 0x80){
        return 1;
    }
    return 0;
}

void test_eeprom_write(u8* data){
// peripheral (eeprom write) address 0xa6
    I2C_SetDeviceAddr(I2C1, 0xa6);
    EEPROM_WriteBuff_Sub2B(I2C1,  0x00, data, 8);
}

u8 test_eeprom_read(u8* data){
// peripheral (eeprom read) address 0xa7
    u8 i;
    if(data == NULL) return 0;
    memset(data, 0, 8);
    I2C_SetDeviceAddr(I2C1, 0xa7);
    EEPROM_ReadBuff_Sub2B(I2C1,  0x00, data, 8);
    for(i = 0; i < 8; i++){
        if(data[i] != test_eeprom_data[i]){
            return 0;
        }
    }
    return 1;
}

void test_read_vref(u8* buff){
    u16 adc_value;
    u16 voltage;
    SetADCChannel(2);
    adc_value = Read_ADC();
    voltage = adc_value * 500 / 1023;
    UART_printf("12vdc %d %dV\r\n", adc_value, voltage);
    memcpy(buff, &voltage, sizeof(voltage));
}

void test_read_temperature(u8* buff){
    u16 adc_value;
    u16 voltage;
    SetADCChannel(3);
    adc_value = Read_ADC();
    voltage = adc_value * 500 / 1023;
    UART_printf("temperatrue %d %dV\r\n", adc_value, voltage);
    memcpy(buff, &voltage, sizeof(voltage));
}

void test_read_all_adc_channel(u16* buff){
    u8 i;
    for(i = 0; i < 4; i++){
        SetADCChannel(i);
        buff[i] = Read_ADC();
    }
}

u8 test_mode_process(void){
    static u16 key_pressed_flag = 0;
    static u8 add_can_tx_queue_flag = 0;
    u8 test_can_rx_data[8];
    u8 test_can_tx_data[8];
    //UART_printf("test mode step %d\r\n", test_step);
    if(test_mode_flag){
        //if(test_step != STEP_WAIT_KEYPAD_TEST){
        //    test_read_keypad();
        //}
        switch(test_step){
            case STEP_IO_CHECK:
            {
                test_io_check_finished_flag = 0;
                test_show_main_view();
                test_step = STEP_WAIT_IO_CHECK_FINISHED;
                break;
            }
            case STEP_WAIT_IO_CHECK_FINISHED:
            {
                static u16 count = 0;
                if(test_io_check_finished_flag){
                    count = 0;
                    test_step = STEP_DISPLAY_ICONS;
                } else {
                    if(count == 0x7fff/*65535*/) {
                        count = 0;
                        test_show_main_view();
                    } else {
                        count += 1;
                    }
                }
                break;
            }
            case STEP_DISPLAY_ICONS:
            {
                LCD_BKLT_0;
                test_display_all_icons();
                key_pressed_flag = 0;
                test_step = STEP_WAIT_KEYPAD_TEST;
                break;
            }
            case STEP_WAIT_KEYPAD_TEST:
            {
                u8 key = test_read_keypad();
                if(/*1||*/key < 9){
                    test_hide_icons(key);
                    key_pressed_flag |= (1 << key);
                    if(/*1||*/key_pressed_flag == 0x1ff){
                        //DELAY_Ms(3000);
                        test_step = STEP_WAIT_CAN_PING;
                        UART_printf("keypad check finished\r\n");
                        //test_show_main_view();
                        lcd_cla( 0, 131, 0, 7 );
                        //pwm2_on();
                        pwm2(60);
                        LCD_BKLT_1;
                    }
                }
                DELAY_Ms(10);
                break;
            }
            case STEP_WAIT_CAN_PING:
            {
                u8 cmd = test_wait_can_cmd(test_can_rx_data);
                if(cmd == 0){
                    test_step = STEP_CAN_PING_RESPONSE;
                    add_can_tx_queue_flag = 0;
                }
                break;
            }
            case STEP_CAN_PING_RESPONSE:
            {
                memset(test_can_tx_data, 0xff, sizeof(test_can_tx_data));
                //test_can_tx_data[0] = 1;
                //test_can_tx_data[1] = 0;
                if(!add_can_tx_queue_flag) {
                    add_can_tx_queue_flag = test_can_send_ack(0x18ff0080, test_can_tx_data);
                }
                if(can_ReadTxQueue() == NULL){
                    test_step = STEP_WAIT_CAN_RTC;
                    add_can_tx_queue_flag = 0;
                } else{
                    can_process();
                }
                break;
            }
            case STEP_WAIT_CAN_RTC:
            {
                u8 cmd = test_wait_can_cmd(test_can_rx_data);
                if(cmd == 2){
                    test_rtc_write(test_can_rx_data);
                    test_step = STEP_CAN_RTC_RESPONSE;
                }
                break;
            }
            case STEP_CAN_RTC_RESPONSE:
            {
                u16 res;
                if(!add_can_tx_queue_flag) {
                    res = test_rtc_read(test_can_tx_data);
                    memset(test_can_tx_data, 0xff, sizeof(test_can_tx_data));
                    memcpy(test_can_tx_data, &res, 2);
                    add_can_tx_queue_flag = test_can_send_ack(0x18ff0280, test_can_tx_data);
                }
                if(can_ReadTxQueue() == NULL){
                    test_step = STEP_WAIT_CAN_EEPROM;
                    add_can_tx_queue_flag = 0;
                } else{
                    can_process();
                }
                break;
            }
            case STEP_WAIT_CAN_EEPROM:
            {
                u8 cmd = test_wait_can_cmd(test_can_rx_data);
                if(cmd == 3){
                    test_eeprom_read(test_eeprom_backup);
                    test_eeprom_write(test_eeprom_data);
                    test_step = STEP_CAN_EEPROM_RESPONSE;
                }
                break;
            }
            case STEP_CAN_EEPROM_RESPONSE:
            {
                u16 res;
                if(!add_can_tx_queue_flag){
                    res = test_eeprom_read(test_can_tx_data);
                    test_eeprom_write(test_eeprom_backup);
                    memset(test_can_tx_data, 0xff, sizeof(test_can_tx_data));
                    memcpy(test_can_tx_data, &res, 2);
                    add_can_tx_queue_flag = test_can_send_ack(0x18ff0380, test_can_tx_data);
                }
                if(can_ReadTxQueue() == NULL){
                    test_step = STEP_WAIT_CAN_READ_12VDC;
                    add_can_tx_queue_flag = 0;
                } else{
                    can_process();
                }
                break;
            }
            case STEP_WAIT_CAN_READ_12VDC:
            {
                u8 cmd = test_wait_can_cmd(test_can_rx_data);
                if(cmd == 1){
                    test_step = STEP_CAN_READ_12VDC_RESPONSE;
                }
                break;
            }
            case STEP_CAN_READ_12VDC_RESPONSE:
            {
                memset(test_can_tx_data, 0xff, sizeof(test_can_tx_data));
                if(!add_can_tx_queue_flag){
                    test_read_vref(test_can_tx_data);
                    add_can_tx_queue_flag = test_can_send_ack(0x18ff0180, test_can_tx_data);
                }
                if(can_ReadTxQueue() == NULL){
                    test_step = STEP_WAIT_CAN_READ_TEMP;
                    add_can_tx_queue_flag = 0;
                } else{
                    can_process();
                }
                break;
            }
            case STEP_WAIT_CAN_READ_TEMP:
            {
                u8 cmd = test_wait_can_cmd(test_can_rx_data);
                if(cmd == 4){
                    test_step = STEP_CAN_READ_TEMP_RESPONSE;
                }
                break;
            }
            case STEP_CAN_READ_TEMP_RESPONSE:
            {
                memset(test_can_tx_data, 0xff, sizeof(test_can_tx_data));
                if(!add_can_tx_queue_flag){
                    test_read_temperature(test_can_tx_data);
                    add_can_tx_queue_flag = test_can_send_ack(0x18ff0480, test_can_tx_data);
                }
                if(can_ReadTxQueue() == NULL){
                    test_step = STEP_END;
                    add_can_tx_queue_flag = 0;
                } else{
                    can_process();
                }
                break;
            }
            case STEP_END:
            {
                ALT_OFF_RESET();
                APU_START_RESET();
                APU_RUN_RESET();
                pwm1(0);
                pwm1_off();
                pwm2(0);
                pwm2_off();
                test_step = 0;
                test_mode_flag = 0;
                lcd_cla( 0, 131, 0, 7 );
                UART_printf("finish test\r\n");
                break;
            }
            default:
                break;
        }
    }
    return test_mode_flag;
}

void TestMode(void)
{
    //u8 buff[8];
    if(!test_mode_flag){
        UART_printf("start test mode\r\n");
        tset_show_title();
        test_gpio_init();
        test_mode_flag = 1;
        test_step = 0;
        
        //test_rtc_write(buff);
        //DELAY_Ms(2000);
        //test_rtc_read(buff);
    }
}
