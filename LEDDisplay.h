/*
	(*)	LED Display

		LyxSoft Oct.01.2020

*/

#ifndef __LEDDISPLAY__

#define __LEDDISPLAY__

#define __REVERSE__

#define LED_NEGTIVE	0xBF	// -
#define LED_BLANK	0xFF	// Blank

#ifdef __REVERSE__
#define LED_1	3
#define LED_2	2
#define LED_3	1
#define LED_4	0
#else
#define LED_1	0
#define LED_2	1
#define LED_3	2
#define LED_4	3
#endif

// BCD Coding
#define MAKEBCD(nValue)	((((nValue) / 10) << 4) + ((nValue) % 10))
#define GETBCD(nBCD)    ((((nBCD) >> 4) * 10) + ((nBCD) & 0xF))


// Display Data
extern unsigned char nData[4];
//Use for showing the focus
extern unsigned char nFocusFlags;
#ifdef __REVERSE__
#define FOCUS_ALL	0xF
#define FOCUS_LOW	0xC
#define FOCUS_HIGH	0x3
#else
#define FOCUS_ALL	0xF
#define FOCUS_LOW	0x3
#define FOCUS_HIGH	0xC
#endif

void DisplayDecNumLBCD (unsigned char nNumLow);

void DisplayDecNumHBCD (unsigned char nNumHigh);

void DisplayDecNumBCD (unsigned char nNumLow, unsigned char nNumHigh);

void DisplayData (unsigned char nData0, unsigned char nData1, unsigned char nData2, unsigned char nData3);

#define DotOn()		{nData [1] &= 0x7F; nData [2] &= 0x7F;}
#define DotOff()	{nData [1] |= 0x80; nData [2] |= 0x80;}

#ifdef __REVERSE__
#define DegOn()		{nData [1] &= 0x7F; nData [2] |= 0x80;}
#define SepOn()		{nData [1] |= 0x80; nData [2] &= 0x7F;}
#define DegOff()	DotOff()

#define DisplayErr()	{DisplayData(0xFF,0xBD,0xAF,0xB0);}
#else
#define DegOn()		{nData [1] |= 0x80; nData [2] &= 0x7F;}
#define SepOn()		{nData [1] &= 0x7F; nData [2] |= 0x80;}
#define DegOff()	DotOff()

#define DisplayErr()	{DisplayData(0xFF,0xAF,0xBD,0x86);}
#endif


#define DisplayDecNumL(nNumLow) (DisplayDecNumLBCD (MAKEBCD(nNumLow)))
#define DisplayDecNumH(nNumHigh) (DisplayDecNumHBCD (MAKEBCD(nNumHigh)))
#define DisplayDecNum(nNumLow, nNumHigh) (DisplayDecNumBCD (MAKEBCD(nNumLow), MAKEBCD(nNumHigh)))

#endif