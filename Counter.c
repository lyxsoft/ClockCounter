/*
	(*) Counter function

		LyxSoft 06.10.2020
*/

#include "Common.h"
#include "Counter.h"
#include "LEDDisplay.h"
#include "SysTimer.h"

#define COUNTDOWN_BEEP	3	//3 times
#define COUNTDOWN_FLASH	3	//3 times

static BOOL bPause;
static unsigned char nCountDownSec, nCountDownMin, nLastMin, nLastSec;


void InitCounter ()
{
	//CountDown Data
	nCountDownSec = 0;
	nCountDownMin = 0;
	nLastSec = 0;
	nLastMin = 0;

	bPause = 1;
	RefreshSysTimer ();
}

void RestartCounter ()
{
	bPause = 1;
	nCountDownSec = nLastSec;
	nCountDownMin = nLastMin;

	//Init Display
	bSound = 0;
	bDotBlink = 0;
	DisplayDecNum (nCountDownSec, nCountDownMin);
	DotOn ();
	SetOnTimer (CountDownOnTimer);
}

void IncCountDown ()
{
	if (nFocusFlags == FOCUS_LOW)
	{
		if (nCountDownSec < 59)
		{
			nCountDownSec ++;
			DisplayDecNumL (nCountDownSec);
		}
	}
	else
	{
		if (nCountDownMin < 99)
		{
			nCountDownMin ++;
			DisplayDecNumH (nCountDownMin);
		}
	}
	DotOn ();

	nLastMin = nCountDownMin;
	nLastSec = nCountDownSec;
}

void DecCountDown ()
{	
	if (nFocusFlags == FOCUS_LOW)
	{
		if (nCountDownSec)
		{
			nCountDownSec --;
			DisplayDecNumL (nCountDownSec);
		}
	}
	else
	{
		if (nCountDownMin)
		{
			nCountDownMin --;
			DisplayDecNumH (nCountDownMin);
		}
	}
	DotOn ();

	nLastMin = nCountDownMin;
	nLastSec = nCountDownSec;
}

void SetCountDown ()
{
	if (nFocusFlags == FOCUS_ALL)
		nFocusFlags = FOCUS_LOW; //Go in Sec
	else
		nFocusFlags = FOCUS_ALL;
}


/*	On Key Start for countdown function

	Return:
		0	OK
		1	No function, switching the mode

*/
unsigned char CountDownOnStart (unsigned char bLongPress)
{
	//Change back focus
	nFocusFlags = FOCUS_ALL;

	if (bLongPress)
	{
		if (nCountDownMin == nLastMin && nCountDownSec == nLastSec)
			return 1;

		//Stop
		RestartCounter ();
	}
	else if (bPause)
	{
		//Start countdown
		RefreshSysTimer ();
		bDotBlink = 1;
		bPause = 0;
	}
	else
	{
		//Pause
		bPause = 1;
		bDotBlink = 0;
		DotOn ();
	}

	return 0;
}


void CountDownOnTimer ()
{
	static unsigned char nFlashCount = 0;

	if (bPause)
		return;

	if (nFlashCount)
	{
		nFlashCount --;
		if (!nFlashCount)
		{
			bPause = 1;
			bDotBlink = 0;
			nCountDownMin = nLastMin;
			nCountDownSec = nLastSec;
			DisplayDecNum (nCountDownSec, nCountDownMin);
			DotOn ();
		}
	}
	else if (nCountDownMin)
	{
		if (!nCountDownSec)
		{
			nCountDownMin --;
			nCountDownSec = 59;
			DisplayDecNum (nCountDownSec, nCountDownMin);
			DotOn ();
		}
		else
		{
			nCountDownSec --;
			DisplayDecNumL (nCountDownSec);
			DotOn ();
		}
	}
	else if (nCountDownSec)
	{
		nCountDownSec --;
		DisplayDecNumL (nCountDownSec);
		DotOn ();
		if (nCountDownSec <= COUNTDOWN_BEEP)
			bSound = 1;
	}
	else
	{
		nFlashCount = COUNTDOWN_FLASH - 1;
		DisplayData (LED_NEGTIVE, LED_NEGTIVE, LED_NEGTIVE, LED_NEGTIVE);	//--:--
		DotOn ();
	}
}