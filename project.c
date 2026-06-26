//project.c
#include "all_macro1.h"

//---------------------------------------delay_def.c----------------------------------------------
//#include "types.h"
void delay_us(u32 i)
{
	 i*=12;
	while(i--);
}
void delay_ms(u32 i)
{
	 i*=12000;
	while(i--);
}
void delay_s(u32 i)
{
	 i*=12000000;
	while(i--);
}

//--------------------------------------------lcd.c----------------------------------------------

//#include "types.h"
//#include "lcd_defines.h"
//#include "defines.h"
//#include "delay.h"
void writeLcd(u8 byte)
{
 //write to data pins
 WRITEBYTE(IOPIN0,LCD_DATA,byte);
 //select write operation
// IOCLR0=1<<LCD_RW;
 //provide high to low enable pulse(for latching purpose)
 IOSET0=1<<LCD_EN;
 delay_us(1);
 IOCLR0=1<<LCD_EN;
 delay_ms(2);
}
void cmdLcd(u8 opcode)
{
  //clear rs pin for cmd register select
  IOCLR0=1<<LCD_RS;
  //write to cmd register via d0 to d7
  writeLcd(opcode);
}
void InitLcd(void)
{
 //cfg data,rs,rw,en as gpio output pins
 IODIR0|=((0xff<<LCD_DATA)|(1<<LCD_RS)|/*(1<<LCD_RW)|*/(1<<LCD_EN));
 delay_ms(15);
 cmdLcd(0x30);
 delay_ms(4);
 delay_us(100);
 cmdLcd(0x30);
 delay_us(100);
 cmdLcd(0x30);
 cmdLcd(0x38);
 cmdLcd(0x0F);
 cmdLcd(0x01);
 cmdLcd(0x06);
}


void charLcd(u8 asciival)
{
 //set rs pin for data register select
 IOSET0=1<<LCD_RS;
 //write to in dram via data reg via data pins
 writeLcd(asciival);
}

void strLcd(s8* str)
{
  while(*str)
  charLcd(*str++);
}

void u32Lcd(u32 num)
{
  u8 a[10];
  s32 i=0;
	 if(num==0)
  {
   charLcd('0');
  }
  else
  {
    while(num>0)
        {
          a[i++]=(num%10)+48;
          num/=10;
        }
        i--;
        for(;i>-1;i--)
        {
        charLcd(a[i]);
        }
  }
}

void s32Lcd(s32 num)
{
 if(num<0)
 {
   charLcd('-');
   num=-num;
 }
 u32Lcd(num);
}
void f32Lcd(f32 fnum,u32 ndp)
{
 u32 num;
 s32 i;
 if(num<0.0)
 {
    charLcd('-');
        fnum=-fnum;
 }
 num=fnum;
 u32Lcd(num);
 charLcd('.');
 for(i=0;i<ndp;i++)
 {
  fnum=(fnum-num)*10;
  num=fnum;
  charLcd(num+48);
 }
}

void BuildCGram(u8 *str,u32 nbytes)
{
   u32 i;
  cmdLcd(0x40); //go to cgram
   IOSET0=1<<LCD_RS;
   //IOCLR0=1<<LCD_RW;
   for(i=0;i<nbytes;i++)
   {
      writeLcd(str[i]);
   }
   //goto ddram
   cmdLcd(0x80);//goto line1 pos0
}

//-----------------------------------------------seg.c-------------------------------------

//#include <LPC21xx.h>
//#include "delay.h"
//#include "defines.h"
//#include "types.h"
//#include "seg_defines.h"
u8 segLUT[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x98};

void init_7segs(void)
{
  WRITEBYTE(IODIR1,ca7seg_2_mux,0xFF);
  //WRITENBITS(IODIR0,DSEL1,16,17);
  IODIR0|=1<<DSEL1 | 1<<DSEL2;
}
void disp_2_mux_segs(u8 num)
{
  WRITEBYTE(IOPIN1,ca7seg_2_mux,segLUT[num/10]);
  IOSET0=1<<DSEL1;
  delay_ms(1);
  IOCLR0=1<<DSEL1;
  WRITEBYTE(IOPIN1,ca7seg_2_mux,segLUT[num%10]);
  IOSET0=1<<DSEL2;
  delay_ms(1);
  IOCLR0=1<<DSEL2;
}


//--------------------------------------------kpm.c--------------------------------------------
//#include "kpm_defines.h"
//#include <lpc21xx.h>
//#include "types.h"
//#include "lcd.h"

//u32 KpmLut[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
u8 KpmLut[4][4]={{'7','8','9','/'},{'4','5','6','*'},{'1','2','3','-'},{'c','0','=','+'}};
//inti the pins
void Kpm_init(void)
{
   IODIR1|=15<<ROW0;
}

//colscan function
u32 colscan(void)
{
   if(((IOPIN1>>COL0)&15)<15)
   {
     return 0;
         }
         return 1;
}


//row_check function
u32 Row_Check(void)
{
   int rno;
   for(rno=0;rno<4;rno++)
   {
    IOPIN1=(IOPIN1&~(0xFF<<ROW0))|((~(1<<rno))<<ROW0);
		 if(colscan()==0)
            {
                  break;
                }
        }
        IOCLR1=15<<ROW0;
        return rno;
}

 //col_check function
 u32 Col_Check(void)
 {
   int cno;
   for(cno=0;cno<4;cno++)
   {
      if(((IOPIN1>>(COL0+cno))&1)==0)
          {
            break;
          }
   }
   return cno;
 }

 //keyscan function
u32 Key_Scan(void)
{
   u32 key,cno,rno;
   while(colscan());
    rno=Row_Check();
        cno=Col_Check();
        key=KpmLut[rno][cno];
	  while(!colscan());
	      delay_ms(100);
        return key;
}

u32 ReadNum(void)
{
  u8 key;
	s8 count=0;
  u32 sum=0;
  while(1)
  {
    
		key=Key_Scan();
		if(key=='c')
		{
			return 'c';
		}
   if(key>='0' && key<='9' && count<4)
   {
		 count++;
      sum=(sum*10)+(key-48);
		// cmdLcd(0xc0);
     charLcd(key);
   }
	 if((key=='+') && (count>0))
	 {
		 count--;
		 cmdLcd(0x10);
		 charLcd(' ');
		 sum=sum/10;
		 cmdLcd(0x10);
	 }
   if(key=='=')
    {
          return sum;
        }
   }
 }
//----------------------------------------------------adc.c-----------------------------------------

//#include "adc_defines.h"
//#include <LPC21xx.h>
//#include "delay.h"

void Init_ADC(void)
{
	PINSEL1=0x10000000;
	ADCR=(PDN_BIT|CLKDIV_VALUE);
}

void Read_ADC(u32 CHN0,u32* AdcVal,f32* eAR)
{
	ADCR&=~(255<<0);
	ADCR|=(CHN0|START_CONV);
	delay_us(3);
	while(((ADDR>>DONE_BIT)&1)==0);
	ADCR&=~(START_CONV);
	*AdcVal=((ADDR>>RESULT)&1023);
	*eAR=(3.3/1024)*(*AdcVal);
		
}

//---------------------------------------------lm35.c----------------------------------
//#include "types.h"
//#include "adc.h"
//#include "adc_defines.h"

f32 Read_LM35DegC(void)
{
	u32 val;
	f32 eAR;
	Read_ADC(CH3,&val,&eAR);
	return (eAR*100);
}

f32 Read_LM35DegF(void)
{
	 f32 temp;
	temp=Read_LM35DegC();
	return ((temp*1.8)+32);
}

