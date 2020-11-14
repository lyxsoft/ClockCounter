/*
	(*)	Clock

		LyxSoft 13.10.2015

*/

#include "Common.h"
#include "STC\STC15F104E.H"
#include "LEDDisplay.h"
#include "SysTimer.h"
#include "Clock.h"
#include "Counter.h"
#include "Timer.h"

#define KEY_START	P30
#define KEY_EC11_D	P31
#define KEY_EC11_A	P15
#define KEY_EC11_B	P14

// EC11 Direction
typedef enum __TURN {
	TURN_NONE = 0,
	TURN_CW = 1,
	TURN_CCW = 2
} TURN;

// Display
unsigned char nData [4];
unsigned char nFocusFlags;

// Mode
unsigned char nMode;
#define MODE_CLOCK	0
#define MODE_COUNT	1
#define MODE_TIMER	2
#define MODE_MAX	2
#define MODE_NEXT	(MODE_MAX+1)

static void (*OnEC11_CW) ();
static void (*OnEC11_CCW) ();
static void (*OnEC11_Down) ();
/*	On Key Start for countdown function

	Return:
		0	OK
		1	No function, switching the mode

*/
static unsigned char (*OnKeyStart) (unsigned char);

static void NoAction () {}


/*
	Change the mode
	If nTargeMode == MODE_NEXT, change to next mode
*/
void ChangeMode (unsigned char nTargetMode)
{
	BOOL bSaveBlcok;

	bSaveBlock = bBlockClock;
	bBlockClock = 1;  //Block clock

	if (nTargetMode >= MODE_NEXT)
	{
		if ((++nMode) > MODE_MAX)
			nMode = 0;
	}
	else
		nMode = nTargetMode;

	switch (nMode)
	{
	case MODE_CLOCK:
		RestartClock ();

		//Set Key Event Handle
		OnEC11_CW	= IncClock;
		OnEC11_CCW	= DecClock;
		OnEC11_Down	= SetClock;
		OnKeyStart	= ClockOnStart;
		break;
	case MODE_COUNT:
		RestartCounter ();

		//Set Key Event Handle
		OnEC11_CW	= IncCountDown;
		OnEC11_CCW	= DecCountDown;
		OnEC11_Down	= SetCountDown;
		OnKeyStart	= CountDownOnStart;
		break;
	case MODE_TIMER:
		RestartTimer ();

		//Set Key Event Handle
		OnEC11_CW	= NoAction;
		OnEC11_CCW	= NoAction;
		OnEC11_Down	= NoAction;
		OnKeyStart	= TimerOnStart;
		break;
	}
	bBlockClock = bSaveBlock;  //Release clock
}

void main()
{
	// Buttons
	BOOL bLastEC11A, bLastEC11B;
	TURN nLastTurn = TURN_NONE;


	// Init Inputs
	// P1M1 = 1; P1M0 = 0; Only Input
	// P1M1 = 0; P1M0 = 0; I/O
	// P1M1 = 0; P1M0 = 1; 20ma Output
	// P1M1 = 1; P1M0 = 1; Open Drain

	P1M1 = 0x0; //0xCF; //11001111
	P1M0 = 0x0;  //00000000
	P3M1 = 0x0;	 //00000000
	P3M0 = 0x0;	 //00000000

	nFocusFlags = FOCUS_ALL;

	InitSysTimer ();
	InitCounter ();
	InitClock ();
	InitTimer ();

	ChangeMode (MODE_CLOCK);

	bLastEC11A = KEY_EC11_A;
	bLastEC11B = KEY_EC11_B;
	nLastTurn = TURN_NONE;

	while (1)
	{
		//  CW: A1B1 -> A0B1 -> A0B0 -> A1B0 -> A1B1
		// CCW: A1B1 -> A1B0 -> A0B0 -> A0B1 -> A1B1

		if (KEY_EC11_A != bLastEC11A || KEY_EC11_B != bLastEC11B) // Change
		{
			if (KEY_EC11_A != bLastEC11A && KEY_EC11_B != bLastEC11B) // Double Change = Wrong
				nLastTurn = TURN_NONE;
			else if (bLastEC11A)
			{
				if (bLastEC11B)		//A1B1 ->
				{
					if (KEY_EC11_A)	// A1B1 -> A1B0
						nLastTurn = TURN_CCW;	//CCW
					else			// A1B1 -> A0B1
						nLastTurn = TURN_CW; //CW
				}
				else if (KEY_EC11_A)	// A1B0 -> A1B1
				{
					if (nLastTurn == TURN_CW) //CW
						OnEC11_CW ();
					nLastTurn = TURN_NONE;
				}
				// Otherwise A1B0 -> A0B0
				//Keep turning or turn back, doesn't matter							
			}
			else if  (bLastEC11B)	//A0B1 ->
			{
				if (KEY_EC11_A)		//A0B1 -> A1B1
				{
					if (nLastTurn == TURN_CCW) //CCW
						OnEC11_CCW ();
					nLastTurn = TURN_NONE; //Turn back = Wrong
				}						
				// Otherwise A0B1 -> A0B0
				// Keep turning or turn back, doesn't matter	
			}
			// Otherwise A0B0 ->, Keep turning or turn back, doesn't matter

			bLastEC11A = KEY_EC11_A;
			bLastEC11B = KEY_EC11_B;
		}

 		if (!KEY_START)
		{
			nLongPress = BTN_PRESS;
			Delay100ms ();
			if (!KEY_START)
			{
				nLongPress = BTN_PRESSED;
				while (!KEY_START);

				if (OnKeyStart (nLongPress == BTN_LONGPRESS))
					ChangeMode (MODE_NEXT);
			}
			nLongPress = BTN_NONE;
		}

		if (!KEY_EC11_D)
		{
			Delay100ms ();
			if (!KEY_EC11_D)
			{
				while (!KEY_EC11_D);

				OnEC11_Down ();
			}
		}

	}
}