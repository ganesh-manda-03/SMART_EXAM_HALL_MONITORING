// project.c
// Embedded firmware for LPC-series (LPC2148-style) microcontroller.
// Contains drivers for: software delays, 16x2 character LCD (4/8-bit
// parallel interface), 2-digit multiplexed 7-segment display, 4x4
// matrix keypad, ADC, and an LM35 temperature sensor.

#include "all_macro1.h"

//==================================================================
// Section: Delay utilities (delay_def.c)
// Simple busy-wait (software) delays calibrated for the CPU clock.
// NOTE: the multiplier constants (12 / 12000 / 12000000) are tuned
// for a specific clock speed and compiler optimization level; they
// are not portable and should be replaced with a hardware timer
// based delay for accurate/production timing.
//==================================================================

// Busy-wait delay of approximately 'i' microseconds
void delay_us(u32 i)
{
	 i*=12;
	while(i--);
}

// Busy-wait delay of approximately 'i' milliseconds
void delay_ms(u32 i)
{
	 i*=12000;
	while(i--);
}

// Busy-wait delay of approximately 'i' seconds
void delay_s(u32 i)
{
	 i*=12000000;
	while(i--);
}

//==================================================================
// Section: Character LCD driver (lcd.c)
// Drives a standard HD44780-compatible 16x2 LCD in 8-bit mode using
// separate RS (register select) and EN (enable) control lines.
//==================================================================

// Low-level byte write: puts 'byte' on the data bus and pulses the
// Enable (EN) line high->low so the LCD latches the data/command.
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

// Sends a command/instruction byte to the LCD (RS = 0 selects the
// instruction register instead of the data register).
void cmdLcd(u8 opcode)
{
  //clear rs pin for cmd register select
  IOCLR0=1<<LCD_RS;
  //write to cmd register via d0 to d7
  writeLcd(opcode);
}

// One-time LCD initialization sequence: configures the relevant
// GPIO pins as outputs and issues the standard HD44780 power-on
// initialization sequence (function set, display on, clear, entry
// mode) as per the datasheet.
void InitLcd(void)
{
 //cfg data,rs,rw,en as gpio output pins
 IODIR0|=((0xff<<LCD_DATA)|(1<<LCD_RS)|/*(1<<LCD_RW)|*/(1<<LCD_EN));
 delay_ms(15);          // wait for LCD power-on stabilization
 cmdLcd(0x30);           // function set (wake-up #1)
 delay_ms(4);
 delay_us(100);
 cmdLcd(0x30);           // function set (wake-up #2)
 delay_us(100);
 cmdLcd(0x30);           // function set (wake-up #3)
 cmdLcd(0x38);           // 8-bit mode, 2 lines, 5x8 font
 cmdLcd(DSP_ON_CUR_BLK);  // display ON, cursor blinking
 cmdLcd(CLEAR_LCD);       // clear display, reset cursor
 cmdLcd(SHIFT_CUR_RIGHT); // entry mode: auto-increment cursor right
}

// Writes a single printable character to the LCD at the current
// cursor position (RS = 1 selects the data register).
void charLcd(u8 asciival)
{
 //set rs pin for data register select
 IOSET0=1<<LCD_RS;
 //write to in dram via data reg via data pins
 writeLcd(asciival);
}

// Writes a null-terminated string to the LCD, one character at a
// time, starting at the current cursor position.
void strLcd(s8* str)
{
  while(*str)
  charLcd(*str++);
}

// Converts an unsigned 32-bit integer to ASCII and prints it on the
// LCD (no leading zeros, prints "0" for a value of 0).
void u32Lcd(u32 num)
{
  u8 a[10];   // holds decimal digits, least-significant first
  s32 i=0;
	 if(num==0)
  {
   charLcd('0');
  }
  else
  {
    // extract digits in reverse order
    while(num>0)
        {
          a[i++]=(num%10)+48;
          num/=10;
        }
        i--;
        // print digits in correct (most-significant-first) order
        for(;i>-1;i--)
        {
        charLcd(a[i]);
        }
  }
}

// Converts a signed 32-bit integer to ASCII and prints it on the
// LCD, prefixing a '-' sign for negative values.
void s32Lcd(s32 num)
{
 if(num<0)
 {
   charLcd('-');
   num=-num;
 }
 u32Lcd(num);
}

// Prints a floating-point value on the LCD with 'ndp' digits after
// the decimal point.
// NOTE: 'num' is read (in the sign check) before it is assigned a
// value, and it is declared as u32 (unsigned) so the "num<0" check
// can never be true - this looks like a bug in the original code
// and should be reviewed (likely intended to check fnum<0 instead).
void f32Lcd(f32 fnum,u32 ndp)
{
 u32 num;
 s32 i;
 if(fnum<0.0)
 {
    charLcd('-');
        fnum=-fnum;
 }
 num=fnum;           // truncate to get the integer part
 u32Lcd(num);         // print integer part
 charLcd('.');
 // print 'ndp' fractional digits by repeatedly scaling by 10
 for(i=0;i<ndp;i++)
 {
  fnum=(fnum-num)*10;
  num=fnum;
  charLcd(num+48);
 }
}

// Loads custom character bitmap data ('nbytes' bytes from 'str')
// into the LCD's CGRAM (Character Generator RAM), then returns the
// cursor to the start of line 1 (DDRAM) so normal printing resumes.
void BuildCGram(u8 *str,u32 nbytes)
{
   u32 i;
   cmdLcd(GOTO_CGRAM_START); //go to cgram
   IOSET0=1<<LCD_RS;
   for(i=0;i<nbytes;i++)
   {
      writeLcd(str[i]);
   }
   //goto ddram
   cmdLcd(GOTO_LINE1_POS0);//goto line1 pos0
}

//==================================================================
// Section: 2-digit multiplexed 7-segment display driver (seg.c)
// Drives two common-anode 7-segment displays that share a single
// segment data bus, using digit-select (anode) lines DSEL1/DSEL2
// and time-division multiplexing (persistence of vision).
//==================================================================

// Lookup table: index = digit value (0-9), value = common-anode
// segment pattern (active-low) for that digit.
u8 segLUT[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x98};

// Configures the segment data port and digit-select pins as GPIO
// outputs.
void init_7segs(void)
{
  WRITEBYTE(IODIR1,ca7seg_2_mux,0xFF);
  //WRITENBITS(IODIR0,DSEL1,16,17);
  IODIR0|=1<<DSEL1 | 1<<DSEL2;
}

u8 scount;  // free-running counter used to alternate/refresh digits

// Must be called periodically (e.g. from a timer ISR or main loop)
// to refresh the 2-digit display. Alternates between driving the
// tens digit and the units digit of 'num' so both appear lit due
// to persistence of vision.
void disp_2_mux_segs(u8 num)
{
   
        scount++; 
			 if(scount==200)
			 {
        scount=0;
			 }				

        if (scount % 2 == 0)
        {
            /* TENS digit */
            IOCLR0 = (1 << DSEL1);               /* units anode OFF         */
   
					 WRITEBYTE(IOPIN1,ca7seg_2_mux,segLUT[num%10]);
     
            IOSET0 = (1 << DSEL2);               /* tens anode ON  */
        }
        else
        {
            /* UNITS digit */
            IOCLR0 = (1 <<DSEL2);                           /* tens anode OFF */
            WRITEBYTE(IOPIN1,ca7seg_2_mux,segLUT[num/10]);  /* units pattern */
            IOSET0 = (1 << DSEL1);                          /* units anode ON */
        }

      

}


//==================================================================
// Section: 4x4 matrix keypad driver (kpm.c)
// Scans a 4x4 matrix keypad by driving one row low at a time and
// reading the column lines to detect which key is pressed.
//==================================================================

//u32 KpmLut[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};

// Maps [row][col] scan coordinates to the corresponding keypad
// character/symbol.
u8 KpmLut[4][4]={{'7','8','9','/'},{'4','5','6','*'},{'1','2','3','-'},{'c','0','=','+'}};

// Configures the row output pins as GPIO outputs.
void Kpm_init(void)
{
   IODIR1|=15<<ROW0;
}

// Quick check of whether any key is currently pressed on the
// currently-driven row. Returns 0 if at least one column line is
// pulled low (key pressed), otherwise returns 1 (no key pressed).
u32 colscan(void)
{
   if(((IOPIN1>>COL0)&15)<15)
   {
     return 0;
         }
         return 1;
}

// Drives each row low in turn until a pressed key pulls a column
// line low, and returns the index (0-3) of that row.
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
        IOCLR1=15<<ROW0;   // release all rows
        return rno;
}

// Reads the column lines and returns the index (0-3) of the column
// that is pulled low (the pressed key's column).
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

// Blocking key-scan routine: waits for a key press, determines its
// row/column, looks up the corresponding character in KpmLut, waits
// for key release (with a debounce delay), and returns the key
// character.
u32 Key_Scan(void)
{
   u32 key,cno,rno;
   while(colscan());          // wait until a key is pressed
    rno=Row_Check();
        cno=Col_Check();
        key=KpmLut[rno][cno];
	  while(!colscan());       // wait until key is released
	      delay_ms(100);        // debounce delay
        return key;
}

// Reads a multi-digit numeric entry from the keypad, echoing each
// digit to the LCD. 'c' cancels entry, '+' acts as backspace
// (removes the last entered digit), and '=' confirms/returns the
// entered value.
u32 ReadNum(void)
{
  u8 key;
	s8 count=0;   // number of digits currently entered
  u32 sum=0;    // accumulated numeric value
  while(1)
  {
    
		key=Key_Scan();
		if(key=='c')
		{
			return 'c';   // cancel entry
		}
   if(key>='0' && key<='9' && count<4)   // limit to 4 digits
   {
		 count++;
      sum=(sum*10)+(key-48);
		// cmdLcd(0xc0);
     charLcd(key);
   }
	 if((key=='+') && (count>0))            // backspace last digit
	 {
		 count--;
		 cmdLcd(0x10);      // move cursor left
		 charLcd(' ');      // erase last digit on screen
		 sum=sum/10;
		 cmdLcd(0x10);      // move cursor left again
	 }
   if(key=='=')
    {
          return sum;      // confirm entry
        }
   }
 }

//==================================================================
// Section: ADC driver (adc.c)
// Configures and reads the on-chip Analog-to-Digital Converter.
//==================================================================

// Configures the ADC pin function select and powers up/enables the
// ADC with the configured clock divider.
void Init_ADC(void)
{
	PINSEL1=0x10000000;
	ADCR=(PDN_BIT|CLKDIV_VALUE);
}

// Performs a single ADC conversion on channel 'CHN0'.
// Outputs: *AdcVal = raw 10-bit ADC result (0-1023)
//          *eAR    = corresponding voltage in volts (0 - 3.3V)
void Read_ADC(u32 CHN0,u32* AdcVal,f32* eAR)
{
	ADCR&=~(255<<0);            // clear previous channel selection
	ADCR|=(CHN0|START_CONV);    // select channel and start conversion
	delay_us(3);
	while(((ADDR>>DONE_BIT)&1)==0);  // wait for conversion to complete
	ADCR&=~(START_CONV);
	*AdcVal=((ADDR>>RESULT)&1023);
	*eAR=(3.3/1024)*(*AdcVal);   // convert raw code to voltage
		
}

//==================================================================
// Section: LM35 temperature sensor driver (lm35.c)
// The LM35 outputs 10mV per degree Celsius, so temperature (C) is
// simply the sensor voltage (in volts) x 100.
//==================================================================

// Reads the LM35 sensor (on ADC channel CH3) and returns the
// temperature in degrees Celsius.
f32 Read_LM35DegC(void)
{
	u32 val;
	f32 eAR;
	Read_ADC(CH3,&val,&eAR);
	return (eAR*100);
}

// Reads the LM35 sensor and returns the temperature converted to
// degrees Fahrenheit.
f32 Read_LM35DegF(void)
{
	 f32 temp;
	temp=Read_LM35DegC();
	return ((temp*1.8)+32);
}
