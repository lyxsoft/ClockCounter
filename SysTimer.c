/*
	(*)	SysTimer

		LyxSoft 06.10.2020

*/

#include "Common.h"
#include "STC\STC15F104E.H"
#include "SysTimer.h"
#include "LEDDisplay.h"

//50 us	 @ 33Mhz System Clock
#define TIME_LOW    0x8E
#define TIME_HIGH   0xF9

#define BEEP		P33

#define TIME_LONGPRESS	2	//2 seconds


unsigned char n100Ms, nMs, nUs;
unsigned char nLast100Ms;

//Use for blocking the timer function
BOOL bBlockClock;
//Use for beeping, set to 1 for start beeping for 1 time
BOOL bSound;
//Use for showing the dot
BOOL bDotBlink;
//Use for checking the key press timing
unsigned char nLongPress;

void (*OnTimeChange)();

static BOOL bBeeping = 0;


static void DefaultOnTimer () {}

void InitSysTimer ()
{
	bBlockClock = 1;

	RefreshSysTimer ();

	OnTimeChange = DefaultOnTimer;

	BEEP = 1;

	bBlockClock = 0;

	AUXR |= 0x80;		// 1T mode
	TMOD = 0;			// Timer Mode
	TL0 = TIME_LOW;
	TH0 = TIME_HIGH;
	TCON = 0x10;		// Timer
	IE = 0x82;			// INT - IE: bit 7 = EA, bit 2 = ET0

}

void RefreshSysTimer ()
{
	nUs = 0;
	nMs = 0;
	n100Ms = 0;
	nLast100Ms = 0;
	bSound = 0;
	bBeeping = 0;
	bDotBlink = 0;
	nFocusFlags = FOCUS_ALL;
}

void SetOnTimer (void (*NewOnTimer)())
{
	if (NewOnTimer)
		OnTimeChange = NewOnTimer;
}


void OnTimer () interrupt 1
{
	//static unsigned char nLastSec;
	static unsigned char nPressCount = 0;
	static unsigned char nLED = 0, bHighLight = 0;
			
	// Clock
	if ((++nUs) >= 20)	//20x50us = 1ms
	{
		nUs = 0;
		if ((++nMs) >= 100) //Per 100ms
		{
			nMs = 0;
			if ((++n100Ms) >= 10) // Per Second
				n100Ms = 0;
		}
	}

	//Blocking the clock
	if (bBlockClock)
		return;
	bBlockClock = 1;


	//Beeping

	if (bBeeping)
		BEEP = nUs <15 && (nUs & 0x1);

	if (n100Ms != nLast100Ms) // Time change per 100ms
	{
		nLast100Ms = n100Ms;

		if (n100Ms == 1)
		{
			if (bSound)
				bBeeping = 1;
		}
		else if (n100Ms == 2)
		{
			if (bBeeping)
			{
				bSound = 0;
				bBeeping = 0;
				BEEP = 1;
			}
		}
		else if (n100Ms == 5)
		{
			if (bDotBlink)
				DotOff ();
		}
		else if (n100Ms == 0)
		{
			if (bDotBlink)
				DotOn ();

			if (nLongPress == BTN_PRESSED)
			{
				nPressCount ++;
				if (nPressCount >= TIME_LONGPRESS)
					nLongPress = BTN_LONGPRESS;
			}
			else
				nPressCount = 0;

			OnTimeChange ();	//Per second
		}
	}

	if (nLongPress == BTN_LONGPRESS && n100Ms >= 5)
	{
		P3 = P3 | 0xF0; //Hide
	}
	else if (nUs == 0 || nUs == 10)
	{
		// Display per 10x50us = 500us
		P2 = nData [nLED];
		switch (nLED)
		{
		case 0:
			P34 = 1;
			if (bHighLight || (nFocusFlags & 0x1))
			{
				P37 = 0; //Lighton LED
			}
			nLED ++;
			break;
		case 1:
			P37 = 1;
			if (bHighLight || (nFocusFlags & 0x2))
			{
				P36 = 0;
			}
			nLED ++;
			break;
		case 2:
			P36 = 1;
			if (bHighLight || (nFocusFlags & 0x4))
			{
				P35 = 0;
			}
			nLED ++;
			break;
		case 3:
			P35 = 1;
			if (bHighLight || (nFocusFlags & 0x8))
			{
				P34 = 0;
			}
			nLED = 0;
			bHighLight = !bHighLight;
			break;
		}
	}

	bBlockClock = 0;
}

void Delay100ms ()
{
	unsigned char n100MsSave, nMsSave;

	n100MsSave = n100Ms;
	nMsSave = nMs;

	while (n100Ms == n100MsSave);
	while (nMsSave != nMs);
}

/*
void Delay1ms ()
{
	unsigned char nMsSave;

	nMsSave = nMs;

	while (nMsSave == nMs);
}
*/