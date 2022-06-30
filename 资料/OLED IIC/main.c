#include "msp430g2553.h"
#include"I2C_OLED.H"
#include"zimo.h"
int main(void)
{
   system_clock();
   I2C_OLED_Init();
   while(1)
   {
	   OLED_All(0);
	   delay_ms(1000);
	   OLED_P16x16Ch(0,0,1);
	   OLED_P16x16Ch(32,0,2);
	   OLED_P16x16Ch(64,0,3);
	   OLED_P16x16Ch(96,0,4);
//	   OLED_P6x8Str(32,2,6);

	   delay_ms(2000);
//
//       OLED_P8x16Str(0,0,0);
//
	   delay_ms(2000);
	   Draw_BMP(lan);
	   delay_ms(2000);
   }
}
