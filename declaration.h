#include "all_macro1.h"
//----------------lcd.h-----------------------

void writeLcd(u8);
void cmdLcd(u8);
void InitLcd(void);
void charLcd(u8);
void strLcd(s8*);
void u32Lcd(u32);
void s32Lcd(s32);
void f32Lcd(f32,u32);
void BuildCGram(u8*,u32);

//------------------kpm.h------------------------

void Kpm_init(void);
u32 colscan(void);
u32 Row_Check(void);
 u32 Col_Check(void);
 u32 Key_Scan(void);
u32 ReadNum(void);

//------------------delay.h-----------------------
void delay_us(u32);
void delay_ms(u32);
void delay_s(u32);

//------------------seg.h-------------------------

void init_7segs(void);
void disp_2_mux_segs(u8);

//------------------adc.h--------------------------

void Init_ADC(void);
void Read_ADC(u32 CHNO,u32* AdcVal,f32* eAR);

//------------------lm35.h-------------------------
f32 Read_LM35DegC(void);
f32 Read_LM35DegF(void);

