#include "all_macro1.h"
#include "declaration.h"

extern u32  save_pass,user_pass,uhour,umin,dur,flag,temppause,tempTime;
extern u8 pause;

// this function used to check the password is correct or not
//if same return 1 if not then return 0.
u8 password(void)
{
	u32 temp=0,count=4,temp1=0;
	cmdLcd(0x01);
	strLcd("enter the pin:");
	
	while(count)
	{
	   temp1=Key_Scan();
	 if((temp1>='0' && temp1<='9')|| temp1=='c' || temp1=='+')
	 { 
		  if(temp1=='c')  
	  {
		return 'c';
	  }
		if(temp1=='+' && (count<4 && count>0))//these block is used for backspace operation
		{
			again:cmdLcd(0x10); //command for move cursour one step back
			charLcd(' ');//and before character is overwritten withspace
			cmdLcd(0x10);//again one step move backward
			temp/=10;//last digit remove logic
			count++;
		}
		else if(temp1>='0' && temp1<='9')
		{
		 count--;
	   charLcd('*');
		 temp=temp*10+temp1-'0';
		}
	}
 }
	 while((temp1=Key_Scan())!='=')
  {
		if(temp1=='+')
		{
			goto again;// it takes it to again into above while loop
		}
		 if(temp1=='c')  //it is used for go to previous function
	 {
		return 'c';
	 }
 }	 
	if(temp==save_pass)
	{ 
		cmdLcd(0xc0);
		strLcd("valid password");
		delay_ms(1000);
		return 1;
	}
	return 0;
}
//this function use to set the RTC time
void Time(void)
{
	u8 key;
	s32 temp;
	while(1)
	{
		cmdLcd(0x01);
		strLcd("1.HOUR");
		cmdLcd(0xc0);
		strLcd("2.MINUTE");
		cmdLcd(0x94);
		strLcd("3.EXIT");
		key=Key_Scan();
		switch(key)
		{
			case '1':cmdLcd(0x01);
					strLcd("Enter Hour :");
					cmdLcd(0xc0);
					temp=ReadNum();
					if(temp=='c')
	        {
		         break;
	        }
					if(temp<0||temp>=24)
					{
						cmdLcd(0x01);
						strLcd("invalid Hours");
						delay_ms(1000);
					}
					else
					{
						HOUR=temp;
						SEC=0;
					}
					break;
			case '2':cmdLcd(0x01);
					strLcd("Enter Minutes :");
					cmdLcd(0xc0);
					temp=ReadNum();
					 if(temp=='c')
	         {
		         break;
	         }
					if(temp<0||temp>=60)
					{
						cmdLcd(0x01);
						strLcd("invalid Minutes");
						delay_ms(1000);
					}
					else
					{
						MIN=temp;
						SEC=0;
					}
					break;
			case '3':return;
		}
	}
}
// this function is used to set the RTC DATE
void DATE(void)
{
	u8 key;
	s32 temp;
	while(1)
	{
		cmdLcd(0x01);
		strLcd("1.DATE");
		cmdLcd(0xc0);
		strLcd("2.MONTH");
		cmdLcd(0x94);
		strLcd("3.YEAR");
		cmdLcd(0xD4);
		strLcd("4.EXIT");
		key=Key_Scan();
		switch(key)
		{
			case '1':cmdLcd(0x01);
					strLcd("Enter DATE");
					cmdLcd(0xc0);
					temp=ReadNum();
					 if(temp=='c')
	         {
		         break;
	         }
					if(temp<1 || temp>31)
					{
						cmdLcd(0x01);
						strLcd("invalid DATE");
						delay_ms(1000);
					}
					else
					{
						DOM=temp;
				    }
					 break;
			case '2':cmdLcd(0x01);
					strLcd("Enter MONTH");
					cmdLcd(0xc0);
					temp=ReadNum();
					if(temp=='c')
	         {
		         break;
	         }
					if(temp<1 || temp>12)
					{
						cmdLcd(0x01);
						strLcd("invalid MONTH");
						delay_ms(1000);
					}
					else
					{
						MONTH=temp;
					}
					break;
			case '3':cmdLcd(0x01);
					strLcd("Enter YEAR");
					cmdLcd(0xc0);
					temp=ReadNum();
					 if(temp=='c')
	         {
		         break;
	         }
					if(temp<1 || temp>4055)
					{
						cmdLcd(0x01);
						strLcd("invalid YEAR");
						delay_ms(1000);
					}
					else
					{
						YEAR=temp;
					}
					 break;
			case '4':break;
		}
		if(key=='4')
			break;
	}
}

// this function is used to display the RTC setting  TIME and DATE
void edit_time(void)
{
	u8 key;
	while(1)
	{
    cmdLcd(0x01);
    strLcd("1.TIME");
	cmdLcd(0xc0);
	strLcd("2.DATE");
	cmdLcd(0x94);
	strLcd("3.EXIT");
	key=Key_Scan();
	switch(key)
	{
		case '1':Time();
		    //delay_ms(150);
				break;
		case '2':DATE();
				 break;
		case '3':return;          
	}
 }
}
// this function is used for get the exam start time
void start_time(void)
{
	u8 key;
	while(1)
	{
		cmdLcd(0x01);
		strLcd("1.HOUR");
		cmdLcd(0xc0);
		strLcd("2.MINUTE");
		cmdLcd(0x94);
		strLcd("3.EXIT");
		key=Key_Scan();
		switch(key)
		{	
			case '1':cmdLcd(0x01);
					strLcd("Enter Hours:");
					cmdLcd(0xc0);
					uhour=ReadNum();
			    if(uhour=='c')
	        {
		         break;
	        }
					if(uhour<HOUR || uhour>23)
					{
						cmdLcd(0x01);
						strLcd("Invalid Hour");
						delay_ms(1000);
						uhour=0;
					}
					break;
			case '2':cmdLcd(0x01);
					strLcd("Enter Minutes:");
					cmdLcd(0xc0);
					umin=ReadNum();
					 if(umin=='c')
	         {
		         break;
	         }
					if((uhour<=HOUR)&&((umin<MIN)||(umin>59)))
					{
						cmdLcd(0x01);
						strLcd("Invalid Minutes");
						delay_ms(1000);
						umin=0;
					}
					break;
			case '3':return;
		}
	}
}

//It is used to set the duration of the exam  
void duration(void)
{
	u8 key;
	while(1)
	{
		cmdLcd(0x01);
		strLcd("1.DURATION");
		cmdLcd(0xc0);
		strLcd("2.EXIT");
		key=Key_Scan();
		switch(key)
		{
			case '1':cmdLcd(0x01);
					strLcd("Enter duration:");
					cmdLcd(0xc0);
					dur=ReadNum();
			    if(dur=='c')
	         {
		         break;
	         }
					if(dur>99||dur<30)  // this condition is for minimum 30 and maximum 99 minutes exam
					{
						cmdLcd(0x01);
						strLcd("Invalid duration");
						delay_ms(1000);
						dur=0;
					}
					tempTime=dur;
					break;
			case '2':return;
		}
	}
}

//This is used to display the options of the exam time setting
void exam_time(void)
{
	u8 key;
	while(1)
	{
	cmdLcd(0x01);
	strLcd("1.EXAM TIME");
	cmdLcd(0xc0);
	strLcd("2.DURATION");
	cmdLcd(0x94);
	strLcd("3.EXIT");
	key=Key_Scan();
	switch(key)
	{
	    case '1':start_time();
				break;
		case '2':duration();
				break;
		case '3':return;
	}
  }
}

//It provides to reset the password
void edit_password(void)
{
	s32 key;
	cmdLcd(0x01);
	strLcd("Enter New Password");
	cmdLcd(0xc0);
	strLcd("4 digit num: ");
	key=ReadNum();
	cmdLcd(0x94);
	if((key>=1000)&&(key<=9999)) //it checks password is in range 4 digits or not
	{
		save_pass=key;
		strLcd("updated sucessfully");
	}
	if(key=='c')
	{
		return;
	}
	else
	{
		strLcd("invalid new pass");
	}
	delay_ms(1000);
	cmdLcd(0x01);
}

//these function called happen from main after EINT0 ocuurs.
void EINT0_RISE(void)
{
	u8 key,temp;
	s8 count=2;// count is used for number of chances for password re-entry to user
	do
	{
		 temp=password();// it return status of the password function 
		if(temp=='c')//it for return to calling function
    {
	  	return;
    }
		if(temp==1) //1 indicate password correct
    {
	  	break; //it terminates from do-while block
    }
		
		if(temp==0)// temp is 0 means password is not matched
		{
			cmdLcd(0x01);
			count--;
			strLcd("you have ");
			u32Lcd(count);
			strLcd(" attempts"); 
			cmdLcd(0xc0);
			strLcd("wait 3 seconds");
			delay_ms(3000);	
			if(count==0) //if number of attempts complete return to calling function
			{
			   return;
			}
			
	    }
	}while(count);
//after password corrects give access to edit rtc,exam and password reset settings for user
	while(1)
	{
		cmdLcd(0x01);
		strLcd("1.EDIT RTC TIME");
		cmdLcd(0xc0);
		strLcd("2.EDIT EXAM TIME");
		cmdLcd(0x94);
		strLcd("3.EDIT PASSWORD");
		cmdLcd(0xd4);
		strLcd("4.EXIT");
		key=Key_Scan();
		switch(key)
		{
			case '1':edit_time();
					 break;
			case '2':exam_time();
					 break;
			case '3': edit_password();
					 break;
			case '4':break;
		}
	    if(key=='4')
			break;
	}
	
}

//we provide some default value for rtc time and date before editing its values
void Default_Rtc_time(void)
{
	CCR=1<<1; //reset 
	PREINT=PREINT_VALUE;
	PREFRAC=PREFRAC_VALUE;
	CCR=1<<0;//ctc is enable
	SEC=40;
	MIN=59;
	HOUR=12;
	DOM=12;
	MONTH=6;
	YEAR=2026;
}

//this function is used for displaying current time 
void Rtc_Time_Display(void)
{
	u8 c1=0;
	for(c1=1;c1<50;c1++)
			disp_2_mux_segs(dur);
	//HH:MM:SS
		charLcd(HOUR/10+'0');
		charLcd(HOUR%10+'0');
		charLcd(':');
		charLcd(MIN/10+'0');
		charLcd(MIN%10+'0');
		charLcd(':');
		charLcd(SEC/10+'0');
		charLcd(SEC%10+'0');
		strLcd("            ");
}

//this function is used for displaying date 
void Rtc_Date_Display(void)
{
	u8 c1=0;
	for(c1=1;c1<50;c1++)
			disp_2_mux_segs(dur);
   	//day/month/year
		charLcd(DOM/10+'0');
		charLcd(DOM%10+'0');
		charLcd('/');
		charLcd(MONTH/10+'0');
		charLcd(MONTH%10+'0');
		charLcd('/');
		u32Lcd(YEAR);
		strLcd("          ");
}

