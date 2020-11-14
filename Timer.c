/*
	(*)	Timer

		LyxSoft 06.10.2020
*/

#include "Common.h"
#include "Timer.h"
#include "LEDDisplay.h"
#include "SysTimer.h"

static BOOL bPause;
static unsigned char nTimerMin, nTimerSec;

void InitTimer ()
{
	nTimerMin = 0;
	nTimerSec = 0;
	bPause = 1;
	RefreshSysTimer ();
}

void RestartTimer ()
{
	bPause = 1;
	nTimerMin = 0;
	nTimerSec = 0;

	//Init Display
	bSound = 0;
	bDotBlink = 0;
	DisplayDecNum (0, 0);
	DotOn ();
	SetOnTimer (TimerOnTimer);
}

/*	On Key Start for countdown function

	Return:
		0	OK
		1	No function, switching the mode

*/
unsigned char TimerOnStart (unsigned char bLongPress)
{
	//Change back focus
	nFocusFlags = 0xF;

	if (bLongPress)
	{
		if (nTimerMin == 0 && nTimerSec == 0)
			return 1;

		//Stop
		RestartTimer ();
	}
	else if (bPause)
	{
		//Start Timer
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


void TimerOnTimer ()
{
	if (bPause)
		return;

	if ((++nTimerSec) >= 60)
	{
		nTimerSec = 0;
		if ((++nTimerMin) >= 100)
			nTimerMin = 0;
	}
	DisplayDecNum (nTimerSec, nTimerMin);
	DotOn ();
}
