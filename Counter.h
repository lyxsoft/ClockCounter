/*
	(*) Counter function

		LyxSoft 06.10.2020
*/

#ifndef __COUNTER__

#define __COUNTER__

// Counting
extern unsigned char bCount, bPause;
extern unsigned char nCountDownSec, nCountDownMin, nLastMin, nLastSec;


void InitCounter ();
void RestartCounter ();

void IncCountDown ();
void DecCountDown ();
void SetCountDown ();

unsigned char CountDownOnStart (unsigned char bLongPress);

void CountDownOnTimer ();

#endif
