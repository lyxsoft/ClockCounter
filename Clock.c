/*
	(*)	Clock Mode

		LyxSoft 06.10.2020
*/

#include "Clock.h"
#include "DS1302.h"
#include "LEDDisplay.h"
#include "SysTimer.h"
#include "NTC.h"

static DS1302Time nCurrentTime; 

static unsigned char nClockMode;
#define MODE_CLOCK				0
#define MODE_TEMP				1
#define MODE_DATE				2
#define MODE_WEEK				3
#define MODE_YEAR				4
#define MODE_SECOND				5
#define MODE_MAX				5

#define TIME_OTHERMODE			10

static unsigned char nTemp, nTempL;
static unsigned char nModeTime;

#define BCDInc(nBCD, nMax, nMin)	{nBCD = nBCD >= nMax ? nMin : (((nBCD & 0xF) >= 9) ? (nBCD & 0xF0) + 0x10 : nBCD + 1);}
#define BCDDec(nBCD, nMax, nMin)	{nBCD = nBCD <= nMin ? nMax : (((nBCD & 0xF) == 0) ? nBCD - 0x10 + 0x9: nBCD - 1);}


void InitClock ()
{
	D1302ClockInit ();
	nClockMode = MODE_CLOCK;
	nModeTime = 0;

	if (!DS1302ReadTime (&nCurrentTime))	//Read Time
		return;
	nCurrentTime.Hour = BCDHour24 (nCurrentTime.Hour);
	nTemp = ReadTemperature (&nTempL);
}

void RestartClock ()
{
	nFocusFlags = FOCUS_ALL;
	ClockOnTimer ();

	//Init Display
	bSound = 0;
	SetOnTimer (ClockOnTimer);
}

void IncClock ()
{
	if (nFocusFlags == FOCUS_ALL)
		return;

	switch (nClockMode)
	{
	case MODE_CLOCK:
		if (nFocusFlags == FOCUS_HIGH)
		{
			//Inc Hours
			BCDInc (nCurrentTime.Hour, 0x23, 0);
			DisplayDecNumHBCD (nCurrentTime.Hour);
			//DS1302WriteByte (DS1302_HOUR_WRITE, nCurrentTime.Hour);
		}
		else
		{
			//Inc Minutes
			BCDInc (nCurrentTime.Minute, 0x59, 0);
			DisplayDecNumLBCD (nCurrentTime.Minute);
			//DS1302WriteByte (DS1302_MINUTE_WRITE, nCurrentTime.Minute);
		}
		DotOn ();
		break;
	case MODE_TEMP:
		break;
	case MODE_DATE:
		if (nFocusFlags == FOCUS_HIGH)
		{
			//Inc Month
			BCDInc (nCurrentTime.Month, 0x12, 1);
			DisplayDecNumHBCD (nCurrentTime.Month);
			//DS1302WriteByte (DS1302_HOUR_WRITE, nCurrentTime.Hour);
		}
		else
		{
			//Inc Day
			BCDInc (nCurrentTime.Day, 0x31, 1);
			DisplayDecNumLBCD (nCurrentTime.Day);
			//DS1302WriteByte (DS1302_MINUTE_WRITE, nCurrentTime.Minute);
		}
		DotOn ();
		break;
	case MODE_WEEK:
		//Inc Week
		BCDInc (nCurrentTime.Week, 0x7, 1);
		DisplayDecNumLBCD (nCurrentTime.Week);
		break;
	case MODE_YEAR:
		//Inc Year
		BCDInc (nCurrentTime.Year, 0x99, 0);
		DisplayDecNumLBCD (nCurrentTime.Year);
		break;
	case MODE_SECOND:
		break;
	default:
		return;
	}
	DS1302WriteTime (&nCurrentTime);//Write Time	
}

void DecClock ()
{
	if (nFocusFlags == FOCUS_ALL)
		return;

	switch (nClockMode)
	{
	case MODE_CLOCK:
		if (nFocusFlags == FOCUS_HIGH)
		{
			//Dec Hours
			BCDDec (nCurrentTime.Hour, 0x23, 0);
			DisplayDecNumHBCD (nCurrentTime.Hour);
		}
		else
		{
			//Inc Minutes
			BCDDec (nCurrentTime.Minute, 0x59, 0);
			DisplayDecNumLBCD (nCurrentTime.Minute);
		}
		DotOn ();
		break;
	case MODE_TEMP:
		break;
	case MODE_DATE:
		if (nFocusFlags == FOCUS_HIGH)
		{
			//Dec Month
			BCDDec (nCurrentTime.Month, 0x12, 1);
			DisplayDecNumHBCD (nCurrentTime.Month);
		}
		else
		{
			//Dec Day
			BCDDec (nCurrentTime.Day, 0x31, 1);
			DisplayDecNumLBCD (nCurrentTime.Day);
		}
		DotOn ();
		break;
	case MODE_WEEK:
		//Dec Week
		BCDDec (nCurrentTime.Week, 0x7, 1);
		DisplayDecNumLBCD (nCurrentTime.Week);
		nData [LED_2] = LED_BLANK;
		break;
	case MODE_YEAR:
		//Dec Year
		BCDDec (nCurrentTime.Year, 0x99, 0);
		DisplayDecNumLBCD (nCurrentTime.Year);
		break;
	case MODE_SECOND:
		break;
	default:
		return;
	}
	DS1302WriteTime (&nCurrentTime);//Write Time	
}

void SetClock ()
{
	if (nFocusFlags == FOCUS_ALL)
		nFocusFlags = FOCUS_HIGH;
	else if (nFocusFlags == FOCUS_HIGH)
		nFocusFlags = FOCUS_LOW;
	else
		nFocusFlags = FOCUS_ALL;

	switch (nClockMode)
	{
	case MODE_CLOCK:
		break;
	case MODE_TEMP:
		break;
	case MODE_DATE:
		break;
	case MODE_WEEK:
	case MODE_YEAR:
		if (nFocusFlags == FOCUS_HIGH)
			nFocusFlags = FOCUS_LOW;
		break;
	case MODE_SECOND:
		break;
	default:
		return;
	}
}

/*	On Key Start for countdown function

	Return:
		0	OK
		1	No function, switching the mode

*/
unsigned char ClockOnStart (unsigned char bLongPress)
{
	if (bLongPress)
		return 1;

	if ((++nClockMode) > MODE_MAX)
		nClockMode = 0;

	nModeTime = 0;
	nFocusFlags = FOCUS_ALL;
	ClockOnTimer ();
	return 0;
}

void ClockOnTimer ()
{
	unsigned char nLastMin;

	nLastMin = nCurrentTime.Minute;

	if (!DS1302ReadTime (&nCurrentTime))	//Read Time
		return;
	nCurrentTime.Hour = BCDHour24 (nCurrentTime.Hour);

	if (nCurrentTime.Minute != nLastMin)	//Minute changed
		nTemp = ReadTemperature (&nTempL);

	switch (nClockMode)
	{
	case	MODE_CLOCK:
		bDotBlink = 1;
		DisplayDecNumBCD (nCurrentTime.Minute, nCurrentTime.Hour);
		DotOn ();
		return;
	case	MODE_TEMP:
		bDotBlink = 0;
		if (nTemp >= 40 && nTempL < 10)
		{
			DisplayDecNumBCD (nTempL << 4 | 0xC, MAKEBCD(nTemp-40));
			DegOn ();
		}
		else if (nTemp < 40)
		{
			DisplayDecNumBCD (((40-nTemp)%10)<<4 | 0xC, (40-nTemp)/10);
			nData [LED_4] = LED_NEGTIVE;
		}
		else
		{
			DisplayErr ();
		}
		break;
	case	MODE_DATE:
		bDotBlink = 0;
		DisplayDecNumBCD (nCurrentTime.Day, nCurrentTime.Month);
		DotOn ();
		break;
	case	MODE_WEEK:
		bDotBlink = 0;
		DisplayDecNumBCD (nCurrentTime.Week, 0);
		nData [LED_2] = nData [LED_3] = nData [LED_4] = LED_BLANK;
		break;
	case MODE_YEAR:
		bDotBlink = 0;
		DisplayDecNumBCD (nCurrentTime.Year, 0x20); 
		break;
	case MODE_SECOND:
		bDotBlink = 0;
		DisplayDecNumBCD (nCurrentTime.Second, nCurrentTime.Minute);
		SepOn ();
		break;
	}

	if (nFocusFlags != FOCUS_ALL)
	{
		nModeTime = 0;
	}
	else if ((++nModeTime) >= TIME_OTHERMODE)
	{
		//Back to Clock
		nClockMode = MODE_CLOCK;
		nModeTime = 0;
	}
}


