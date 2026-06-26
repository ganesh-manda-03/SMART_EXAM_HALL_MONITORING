//Smart Exam Hall Monitoring and Management System
//all defines and macros are present here
#include "all_macro1.h"
#include "project_declaration.h"
#include "declaration.h"
// EINT0 channel no 
#define EINT0_CH 14
// EINT2 channel no
#define EINT2_CH 16
// save_pass for saving default password and for saving modified password
//user_pass is for when user type his password then this password stored here and for comparing with save password
//uhour for exam start hours,umin means exam start min
//dur for saving the exam duration
//flag0 is used for when eint0 occors it will increment
//flag2 is used for when eint2 occors it will increment
//temppause used for when we go in interupt two these update with current minute of rtc
//tempTime store the duration of exam
u32 user_pass=0,save_pass=1234,uhour,umin,dur=0,flag0=0,flag2=0,temppause=0,tempTime;
//pause is used to store the pausing time when we pause the time during exam
u8 pause=0;
//tempc is store the temperature from lm35 in degree celcius
f32 tempc;
//store the temperory hour and minutes for calculation purpose. 
u8 tempH,tempM;

// running total-minutes reference point, captured once when exam starts/resumes
u32 examStartTotalMin = 0;

//---------------EINT0---------------------
void EINT0_ISR(void)__irq
{
	flag0=1;
  VICVectAddr=0;
	EXTINT=1<<0;
}
//------------------EINT2----------------------
//EINT2 for pause the exam duration
// only acts if an exam is currently running (examRunning flag, see main)
extern u8 examRunning; // set in main when uhour==HOUR && umin==MIN first triggers

void EINT2_ISR(void)__irq
{
	if(examRunning)   // Bug 6 fix: ignore pause button if exam hasn't started
	{
		cmdLcd(0x01);
		strLcd("in eint2");
		delay_ms(1000);
		flag2++;
		if(flag2%2==0)
		{
			// resuming: add the paused duration (in total minutes) to pause accumulator
			u32 nowTotalMin = (u32)HOUR*60 + MIN;     // Bug 1 fix: total minutes, no wrap
			pause += (nowTotalMin - temppause);
		}
		else
		{
			// pausing: record current total-minute timestamp
			temppause = (u32)HOUR*60 + MIN;            
		}
	}
	VICVectAddr=0;
	EXTINT=1<<2;
}

//-------Iitialization of EINT0--------------
void Init_ENIT0(void)
{
	//enable the intrrupt channel no 0
	VICIntEnable=1<<EINT0_CH;
	//give the address to the intrrupt at sloat 0
	VICVectAddr0=(u32)EINT0_ISR;
	//set the cntol pins at sloat 0
	VICVectCntl0=(1<<5)|(EINT0_CH);
	//for setting the mode
	//here we set the mode for intrrupt 0
	//we give here edge triggring and falling edge for intrrupt 0
	EXTMODE=1<<0;
}
void Init_ENIT2(void)
{
	//enable the intrrupt channel no 2
	VICIntEnable=1<<EINT2_CH;
	//give the address to the intrrupt at sloat 1
	VICVectAddr1=(u32)EINT2_ISR;
	//set the cntol pins at sloat 1
	VICVectCntl1=(1<<5)|(EINT2_CH);
	//for setting the mode
	//here we set the mode for intrrupt 2
	//we give here edge triggring 
	EXTMODE=1<<2;
	//falling edge for intrrupt 2
	EXTPOLAR=0<<2;
}
//for comparing with exam time and current time
u8 examRunning=0;

int main()
{
	u32 c=0,c1=0;
	u32 nowTotalMin, elapsed;
	//we have select the EINT0 and EINT1 from pinselect block.
	PINSEL0=0x0000C00C;
	//here we give the direction for buzzer and LEDs
	IODIR0|=(1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<buzzer);
	//initialize EINT0 and EINT2
	Init_ENIT0();
	Init_ENIT2();
	//Initialize LCD
	InitLcd();
	//Initialize Keypad
	Kpm_init();
	//initialize 2 mux 7 segment active anode
	init_7segs();
	//initialize Analog to digital converter
	Init_ADC();
	//in this function we gave the default TIME AND DATE
	Default_Rtc_time();
	cmdLcd(0x0c);
	cmdLcd(0x94);
	strLcd("   SYSTEM LOADING  ");
	delay_ms(1000);
	cmdLcd(0x01);
	while(1)
	{
		
		//we calling the function here when EINT0 occours
		 if((flag0!=0))
		 {
			 EINT0_RISE();
			 flag0=0;
			 cmdLcd(0x01);

			 // if exam settings were edited via EINT0, reset session state
			 //so a fresh exam starts clean (only reset if exam not already running)
			 if(!examRunning)
			 {
				 pause=0;
				 temppause=0;
				 flag2=0;
				 dur=0;
				 c=0;
			 }
		 }
	/*here we are checking when the exam time mached with real time then exam starts 
	and also starts the 7 segmet to show how much time remaining*/
		if((uhour==HOUR && umin==MIN) || c)
		{
			if(uhour==HOUR && umin==MIN)
			VICIntEnClr=1<<EINT0_CH;    //after exam start we disable the EINT0_ISR for rtc editing
			if(!c)
			{
				// capture start reference ONCE, exactly when exam begins
				examStartTotalMin = (u32)HOUR*60 + MIN;
				for(c1=1;c1<200;c1++)
			    disp_2_mux_segs(dur);
				examRunning = 1;
				pause = 0;
				flag2 = 0;
			}
			c=1;

			if((flag2%2)==0)
			{
				nowTotalMin = (u32)HOUR*60 + MIN;
				elapsed = (nowTotalMin - examStartTotalMin) - pause; // total minutes elapsed, pause excluded

				// explicit clamp instead of relying on unsigned wrap to hit exactly 0
				if(elapsed >= tempTime)
					dur = 0;
				else
					dur = tempTime - elapsed;

				//for(c1=1;c1<200;c1++)
					//disp_2_mux_segs(dur);  // this function display remaining time to end exam
			}
			else if((flag2%2)==1)
			{ 
				// paused: just keep displaying last computed dur, don't recalc
				for(c1=1;c1<50;c1++)
					disp_2_mux_segs(dur);
			}

			// clear all LED bits before setting the relevant one
			IOPIN0 &= ~((1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<buzzer));

			if(dur==0)
			{
				
				IOPIN0 |= (1<<buzzer);
				delay_ms(3000);
				VICIntEnable=1<<EINT0_CH;//after exam finished again we enable the EINT0_ISR for edit settings
				examRunning = 0;   // exam finished, stop running state
				c = 0;             // allow a fresh exam to start later
			}
			else if(dur<=5)
			{
				IOPIN0 |= (1<<LED1);
			}
			else if(dur<=10)
			{
				IOPIN0 |= (1<<LED2);
			}
			else if(dur<=15)
			{
				IOPIN0 |= (1<<LED3);
			}
		}
		for(c1=1;c1<100;c1++)
			disp_2_mux_segs(dur);
    cmdLcd(0xd4);
		strLcd("Duration ");
		u32Lcd(dur);
		cmdLcd(0x80);
		Rtc_Time_Display();
		cmdLcd(0xc0);
	  Rtc_Date_Display();
		for(c1=1;c1<100;c1++)
			disp_2_mux_segs(dur);
		cmdLcd(0x94);	
		strLcd("temp ");
		f32Lcd(Read_LM35DegC(),2);
		charLcd(0xdf);
		charLcd('C');
		if(flag2%2)
		{
			strLcd("  pause");
		}
		for(c1=1;c1<100;c1++)
			disp_2_mux_segs(dur);
	}		
}


