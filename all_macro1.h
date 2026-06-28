//--------------------------------all_macros.h-----------------------------------

#include <LPC21xx.h>
#include <stdlib.h>

//-------------------types.h-------------------
typedef  unsigned char u8;
typedef  char          s8;
typedef  unsigned int  u32;
typedef  int           s32;
typedef float          f32;

//-----------------------#include "defines.h"-----------------
//define.h
#define SETBIT(WORD,BIT) (WORD|=1<<BIT)
#define CLRBIT(WORD,BIT) (WORD&=~(1<<BIT))
#define CPLBIT(WORD,BIT) (WORD^=(1<<BIT))
#define WRITEBIT(WORD,BITPOS,BITLEVEL) WORD=((WORD&~(1<<BITPOS))|(BITLEVEL<<BITPOS))
#define READBIT(WORD,BITPOS) ((WORD>>BITPOS)&1)
#define READWRITEBIT(DWORD,DBIT,SWORD,SBIT) DWORD=((DWORD&~(1<<DBIT))|(((SWORD>>SBIT)&1)<<DBIT))
#define WRITENIBBLE(WORD,BITSTARTPOS,VALUE) WORD=((WORD&~(15<<BITSTARTPOS))|(VALUE<<BITSTARTPOS))
#define READNIBBLE(WORD,BITSTARTPOS) ((WORD>>BITSTARTPOS)&15)
#define WRITEBYTE(WORD,BITSTARTPOS,BYTE) WORD=((WORD&~((u32)255<<BITSTARTPOS))|((u32)BYTE<<BITSTARTPOS))
#define READBYTE(WORD,BITSTARTPOS) ((WORD>>BITSTARTPOS)&255)
#define WRITENBITS(WORD,BITSTARTPOS,NBITS,VAL) WORD=((WORD&~(((1<<NBITS)-1)<<BITATARTPOS))|(VAL<<BITSTARTPOS))

//------------------------------#include "RTC_defines.h"-------------------------

#define PCLK1 15000000
#define PREINT_VALUE ((PCLK1/32768)-1)
#define PREFRAC_VALUE (PCLK1-((PREINT_VALUE+1)*32768))


//---------------------------------#include "lcd_defines.h"--------------------
//lcd_defines.h


//HD44780 Commands
#define CLEAR_LCD            0x01
#define RET_CUR_HOME         0x02
#define SHIFT_CUR_RIGHT      0x06
#define SHIFT_CUR_LEFT       0x07
#define DSP_OFF              0x08
#define DSP_ON_CUR_OFF       0x0c
#define DSP_ON_CUR_ON        0x0E
#define DSP_ON_CUR_BLK       0x0F
#define SHIFT_DSP_LEFT       0x10
#define SHIFT_DSP_RIGHT      0x14
#define MODE_8BIT_1LINE      0x30
#define MODE_4BIT_1LINE      0x20
#define MODE_8BIT_2LINE      0x38
#define MODE_4BIT_2LINE      0x28
//assuming 2x16/16x2 lcd panel/screen
#define GOTO_LINE1_POS0      0x80
#define GOTO_LINE2_POS0      0xC0
//assuming 4x20/20x4 lcd panel/screen
#define GOTO_LINE3_POS0      0x94
#define GOTO_LINE4_POS0      0xD4

#define GOTO_CGRAM_START     0x40
#define LCD_DATA 8 //p0.8 to p0.15
#define LCD_RS 16  //p0.16 as RS
#define LCD_EN 17  //p0.17 as EN

//---------------------------------#include "kpm_defines.h"--------------------
//kpm_defines.h

//row cfg pins
#define ROW0 16
#define ROW1 17
#define ROW2 18
#define ROW3 19

//colomn  pins
#define COL0 20
#define COL1 21
#define COL2 22
#define COL3 23

//#include "kpm.h"
//#include "delay.h"
//----------------------#include "seg_defines.h"----------------------
//seg_defines.h
#define ca7seg_2_mux 24//p1.24 - p1.31
#define DSEL1 19 //p0.19
#define DSEL2 20//p0.20

//#include "seg.h"

//----------------------#include "adc_defines.h"------------------------

//adc clk defines
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define ADCLK (3000000)
#define DIVIDER ((PCLK/ADCLK)-1)

//ADC SFRS
//ADCR
#define CLKDIV_VALUE (DIVIDER<<8)
#define PDN_BIT (1<<21)
#define START_CONV (1<<24)

//ADDR
#define RESULT 6
#define DONE_BIT 31

//channel defines
#define CH0 (1<<0)
#define CH1 (1<<1)
#define CH2 (1<<2)
#define CH3 (1<<3)

//#include "adc.h"

//#include "lm35.h"
//---------------------lED and BUZZER----------------------
#define buzzer 5
#define LED1 27
#define LED2 28
#define LED3 29

