/*
	(*)	LED Display

		LyxSoft Oct.01.2020

*/

#include "LEDDisplay.h"

/*                                      	   0    1	  2	   3	4	 5	  6	   7	8	 9    A    b    C    d    E    F    G    H    I    J         L         N    O    P    q    r    S    t    U    V    W         y    Z    [    \    ]
																																																						  DEG	 -	 8.
                                        	   0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34   35   36   37   38   39
											   0    1    2    3    4    5    6    7    8    9    A	   B	C	 D	  E	   F   10	11	 12	  13   14	15	 16	  17   18	19	 1A	  1B   1C	1D	 1E	  1F   20	21	 22	  23   24	25	 26	  27
*/

static unsigned char code tblNumbers[]    ={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xC2,0x89,0xF9,0xF1,0xFF,0xC7,0xFF,0xC8,0xC0,0x8C,0x98,0xAF,0x92,0x87,0xC1,0x8D,0x81,0xFF,0x91,0xA4,0x9C,0xBF, 0x0,0xFF};
static unsigned char code tblNumbersRev[] ={0xC0,0xCF,0xA4,0x86,0x8B,0x92,0x90,0xC7,0x80,0x82,0x81,0x98,0xF0,0x8C,0xB0,0xB1,0xD0,0x89,0xCF,0xCE,0xFF,0xF8,0xFF,0xC1,0xC0,0xA1,0x83,0xBD,0x92,0xB8,0xC8,0xA9,0x88,0xFF,0x8A,0xA4,0xA3,0xBF, 0x0,0xFF};

#ifdef __REVERSE__
void DisplayDecNumLBCD (unsigned char nNumLow)
{
	nData [3] = tblNumbersRev [nNumLow & 0xF];
	nData [2] = tblNumbersRev [nNumLow >> 4];
}

void DisplayDecNumHBCD (unsigned char nNumHigh)
{
	nData [1] = tblNumbers [nNumHigh & 0xF];
	nData [0] = tblNumbersRev [nNumHigh >> 4];
}

void DisplayDecNumBCD (unsigned char nNumLow, unsigned char nNumHigh)
{
	nData [3] = tblNumbersRev [nNumLow & 0xF];
	nData [2] = tblNumbersRev [nNumLow >> 4];
	nData [1] = tblNumbers [nNumHigh & 0xF];
	nData [0] = tblNumbersRev [nNumHigh >> 4];
}

void DisplayData (unsigned char nData0, unsigned char nData1, unsigned char nData2, unsigned char nData3)
{
	nData [3] = nData0;
	nData [2] = nData1;
	nData [1] = nData2;
	nData [0] = nData3;
}

#else
void DisplayDecNumLBCD (unsigned char nNumLow)
{
	nData [0] = tblNumbers [nNumLow & 0xF];
	nData [1] = tblNumbersRev [nNumLow >> 4];

}

void DisplayDecNumHBCD (unsigned char nNumHigh)
{
	nData [2] = tblNumbers [nNumHigh & 0xF];
	nData [3] = tblNumbers [nNumHigh >> 4];
}

void DisplayDecNumBCD (unsigned char nNumLow, unsigned char nNumHigh)
{
	nData [0] = tblNumbers [nNumLow & 0xF];
	nData [1] = tblNumbersRev [nNumLow >> 4];
	nData [2] = tblNumbers [nNumHigh & 0xF];
	nData [3] = tblNumbers [nNumHigh >> 4];
}

void DisplayData (unsigned char nData0, unsigned char nData1, unsigned char nData2, unsigned char nData3)
{
	nData [0] = nData0;
	nData [1] = nData1;
	nData [2] = nData2;
	nData [3] = nData3;
}

#endif

/*
	End of file
*/

