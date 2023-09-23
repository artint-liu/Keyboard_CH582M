/*
 * WS2812.c
 *
 *  Created on: 2023年9月12日
 *      Author: Admin
 */
#include "CH58x_common.h"
#include "WS2812.h"


#define HIGH GPIOB_SetBits(GPIO_Pin_8)
#define LOW GPIOB_ResetBits(GPIO_Pin_8)

// DST: Delay Short Time
// DLT: Delay Long Time

#if FREQ_SYS == 60000000
#define DST __nop(); __nop(); __nop(); __nop()
#define DLT DST; DST; DST
//__nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop()
#else
#error Not Implement
#endif

#define T0 HIGH; DST; LOW; DLT
#define T1 HIGH; DLT; LOW; DST

#define RESET LOW; DelayUs(500)

static uint8_t pixels[NUM_PIXEL * 3] = {
        0x20, 0, 0, 0x20, 0, 0, 0x20, 0, 0, 0x20, 0, 0,
        0x20, 0, 0, 0x20, 0, 0, 0x20, 0, 0, 0x20, 0, 0,};


void WS2812_Init()
{
    GPIOB_ModeCfg(GPIO_Pin_8, GPIO_ModeOut_PP_5mA);
}

void OutByte(uint8_t c)
{
    for(int i = 0; i < 8; i++)
    {
        if((0x80 >> i) & c)
        {
            T1;
        }
        else
        {
            T0;
        }
    }
}

// 实际pixels字节长度应该是count*3
void Show(const uint8_t* pixels, uint32_t count)
{
    uint32_t n = 0;
    for(uint32_t i = 0; i < count; i++)
    {
        OutByte(pixels[n++]);
        OutByte(pixels[n++]);
        OutByte(pixels[n++]);
    }
    RESET;
}

void WS2812_Show()
{
    Show(pixels, NUM_PIXEL);
}

void WS2812_Clear()
{
    for(int i = 0; i < NUM_PIXEL; i++)
    {
        pixels[i * 3 + 0] = 0;
        pixels[i * 3 + 1] = 0;
        pixels[i * 3 + 2] = 0;
    }
}


void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if(index < NUM_PIXEL)
    {
        pixels[index * 3 + 0] = g;
        pixels[index * 3 + 1] = r;
        pixels[index * 3 + 2] = b;
    }
}

//void DelayUs(uint16_t t)
//{
//    uint32_t i;
//#if(FREQ_SYS == 80000000)
//    i = t * 20;
//#elif(FREQ_SYS == 60000000) // 66ns
//    i = t * 15;
//#elif(FREQ_SYS == 48000000)
//    i = t * 12;
//#elif(FREQ_SYS == 40000000)
//    i = t * 10;
//#elif(FREQ_SYS == 32000000)
//    i = t << 3;
//#elif(FREQ_SYS == 24000000)
//    i = t * 6;
//#elif(FREQ_SYS == 16000000)
//    i = t << 2;
//#elif(FREQ_SYS == 8000000)
//    i = t << 1;
//#elif(FREQ_SYS == 4000000)
//    i = t;
//#elif(FREQ_SYS == 2000000)
//    i = t >> 1;
//#elif(FREQ_SYS == 1000000)
//    i = t >> 2;
//#else
//    i = t << 1;
//#endif
//    do
//    {
//        __nop();
//    } while(--i);
//}
