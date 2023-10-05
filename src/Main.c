/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : ����1�շ���ʾ
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH58x_common.h"
#include "WS2812.h"
#include "HID_Key.h"

uint8_t TxBuff[] = "This is a tx exam\r\n";
uint8_t RxBuff[100];
uint8_t trigB;

int InitUSBHID();
void DevHIDKeyClear();
void DevHIDKeyReport(uint8_t key);
void DevHIDKeyReportKeys(uint8_t* keys);
void DevHIDKeyReportEnd();
void USB_DevTransProcess( void );

// LED     PB0
// DIN     PB8
// LATCH   PB7
// PWR BTN PB17
// PWR LVL PA3

// SCAN_EN   PA12
// SCAN_CLK  PA13
// SCAN_PL   PA14
// SCAN_MISO PA15

// bit 7   bit 6   bit 5   bit 4   bit 3   bit 2   bit 1   bit 0
// R Win   R Shift R Alt   R Ctrl  L Win   L Shift L Alt   L Ctrl

#define MOD_RWin    (1 << 7)
#define MOD_RAlt    (1 << 6)
#define MOD_RShift  (1 << 5)
#define MOD_RCtrl   (1 << 4)
#define MOD_LWin    (1 << 3)
#define MOD_LAlt    (1 << 2)
#define MOD_LShift  (1 << 1)
#define MOD_LCtrl   (1 << 0)

uint8_t keydef[8][6] = {
        {MOD_LCtrl, HID_KEY_S, 0},
        {0, HID_KEY_F7, 0},
        {MOD_LShift, HID_KEY_BRACKET_LEFT, 0},
        {MOD_LShift, HID_KEY_BRACKET_RIGHT, 0},
        {MOD_LCtrl, HID_KEY_Z, 0},
        {MOD_LCtrl, HID_KEY_X, 0},
        {MOD_LCtrl, HID_KEY_C, 0},
        {MOD_LCtrl, HID_KEY_V, 0},
};

uint8_t dbPrevKeys = 0;

uint8_t ReadScan()
{
    uint8_t db = 0;

    GPIOA_ResetBits(GPIO_Pin_14);
    GPIOA_SetBits(GPIO_Pin_14);

    GPIOA_ResetBits(GPIO_Pin_12);
#if 1
    for(int i = 0; i < 8; i++)
    {
        GPIOA_ResetBits(GPIO_Pin_13);
        db |= ((!!GPIOA_ReadPortPin(GPIO_Pin_15)) << i);
        GPIOA_SetBits(GPIO_Pin_13);
    }
#else
    db = SPI0_MasterRecvByte();
#endif
    GPIOA_SetBits(GPIO_Pin_12);
    return db;
}


/*********************************************************************
 * @fn      main
 *
 * @brief   ������
 *
 * @return  none
 */
int main()
{
    uint8_t len;

    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if 1
    InitUSBHID();
#else
    DebugInit();

    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;
    pEP2_RAM_Addr = EP2_Databuf;
    pEP3_RAM_Addr = EP3_Databuf;

    USB_DeviceInit();

    PFIC_EnableIRQ( USB_IRQn );

    while( 1 )
    {
      DevHIDKeyReport(4);
      DelayMs(10);
      DevHIDKeyReport(0);
      DelayMs(5000);
    }
#endif

    // SPI0
    GPIOA_SetBits(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
    GPIOA_ModeCfg(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, GPIO_ModeOut_PP_5mA);
    GPIOA_SetBits(GPIO_Pin_15);
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_Floating);
    //GPIOA_ModeCfg(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15, GPIO_ModeIN_PU);
#if 0
    SPI0_MasterDefInit();

    SPI0_DataMode( Mode0_HighBitINFront ); //582m ֻ֧��0��3ģʽ
    //SPI0_CLKCfg( 100 );  //�Ƚ���SPI����
#endif


    /* ���ô���1��������IO��ģʽ�������ô��� */
//    GPIOA_SetBits(GPIO_Pin_9);
//    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);      // RXD-������������
//    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA); // TXD-�������������ע������IO������ߵ�ƽ
//    UART1_DefInit();

    WS2812_Init();
    GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeOut_PP_5mA);

//#if 1 // ���Դ��ڷ����ַ���
//    UART1_SendString(TxBuff, sizeof(TxBuff));
//
//#endif
//
//#if 1 // ��ѯ��ʽ���������ݺ��ͳ�ȥ
//    while(1)
//    {
//        len = UART1_RecvString(RxBuff);
//        if(len)
//        {
//            UART1_SendString(RxBuff, len);
//        }
//    }
//
//#endif

//#if 0 // �жϷ�ʽ���������ݺ��ͳ�ȥ
//    UART1_ByteTrigCfg(UART_7BYTE_TRIG);
//    trigB = 7;
//    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
//    PFIC_EnableIRQ(UART1_IRQn);
//#endif

    for(int i = 0 ; i <5 ; i++)
    {
        GPIOB_SetBits(GPIO_Pin_0);
        DelayMs(100);
        GPIOB_ResetBits(GPIO_Pin_0);
        DelayMs(100);
    }

    WS2812_Show();
    DelayMs(200);

    for(int i = 0; i < NUM_PIXEL; i++)
    {
        WS2812_Clear();
        WS2812_SetPixel(i, 20, 20, 20);
        WS2812_Show();
        DelayMs(50);
    }

    WS2812_Clear();
    WS2812_Show();

    while(1)
    {
#if 1
        DelayMs(10);
        uint8_t db = ReadScan() ^ 0xff;
        if(db != dbPrevKeys)
        {
            dbPrevKeys = db;

            WS2812_Clear();
            DevHIDKeyClear();

            for(int i = 0; i < NUM_PIXEL; i++)
            {
                if((0x80 >> i) & db)
                {
                    WS2812_SetPixel(i, 0, 20, 0);
                    DevHIDKeyReportKeys(keydef[i]);
                }
            }

            DevHIDKeyReportEnd();
            WS2812_Show();
        }
#else
        DevHIDKeyReport(4);
        DelayMs(10);
        DevHIDKeyReport(0);
        DelayMs(5000);
#endif
    }
}

/*********************************************************************
 * @fn      UART1_IRQHandler
 *
 * @brief   UART1�жϺ���
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler(void)
{
    volatile uint8_t i;

    switch(UART1_GetITFlag())
    {
        case UART_II_LINE_STAT: // ��·״̬����
        {
            UART1_GetLinSTA();
            break;
        }

        case UART_II_RECV_RDY: // ���ݴﵽ���ô�����
            for(i = 0; i != trigB; i++)
            {
                RxBuff[i] = UART1_RecvByte();
                UART1_SendByte(RxBuff[i]);
            }
            break;

        case UART_II_RECV_TOUT: // ���ճ�ʱ����ʱһ֡���ݽ������
            i = UART1_RecvString(RxBuff);
            UART1_SendString(RxBuff, i);
            break;

        case UART_II_THR_EMPTY: // ���ͻ������գ��ɼ�������
            break;

        case UART_II_MODEM_CHG: // ֻ֧�ִ���0
            break;

        default:
            break;
    }
}

__INTERRUPT
__HIGH_CODE
void USB_IRQHandler( void ) /* USB�жϷ������,ʹ�üĴ�����1 */
{
  USB_DevTransProcess();
}
