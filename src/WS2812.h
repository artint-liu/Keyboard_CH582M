#pragma once

#define NUM_PIXEL 8

void WS2812_Init();
void WS2812_Show();
void WS2812_Clear();
void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
