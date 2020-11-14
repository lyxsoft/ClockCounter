/*
	(*)	SysTimer

		LyxSoft 06.10.2020

*/

#include "Common.h"

#ifndef __SYSTIMER__
#define __SYSTIMER__

//Use for blocking the timer function
extern BOOL bBlockClock;

//Use for beeping, set to 1 for start beeping for 1 time
extern BOOL bSound;

//Use for showing the dot
extern BOOL bDotBlink;

//Use for checking the key press timing
extern unsigned char nLongPress;
#define BTN_NONE		0
#define BTN_PRESS		1
#define BTN_PRESSED		2
#define BTN_LONGPRESS	3 

void InitSysTimer ();
void RefreshSysTimer ();
void SetOnTimer (void (*NewOnTimer)());

void Delay100ms ();
/*
void Delay1ms ();
*/

#endif
