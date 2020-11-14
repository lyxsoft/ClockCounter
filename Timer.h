/*
	(*)	Timer

		LyxSoft 06.10.2020
*/

#ifndef	__TIMER__
#define	__TIMER__

void InitTimer ();
void RestartTimer ();

unsigned char TimerOnStart (unsigned char bLongPress);
void TimerOnTimer ();

#endif
