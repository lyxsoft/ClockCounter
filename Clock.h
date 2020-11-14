/*
	(*)	Clock Mode

		LyxSoft 06.10.2020
*/

#ifndef	__CLOCKMODE__
#define __CLOCKMODE__


void InitClock ();
void RestartClock ();

void IncClock ();
void DecClock ();
void SetClock ();

unsigned char ClockOnStart (unsigned char bLongPress);

void ClockOnTimer ();

#endif
