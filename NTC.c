/*

	(*) Read NTC temperature


		Connection:
		Vcc---R(4.7K)---NTC(10K)---GND
		              |
		            ADC P1.6

*/

#include "Common.h"
#include "STC\STC15F104E.H"
#include <INTRINS.H>
#include "NTC.h"

#define PIN_NTC P16			//P1.6
#define P1ASF_NTC 0x40	    //P1.6
#define NUM_NTC 0x6			//P1.6

#define ADC_POWER   0x80    //ADC电源控制位
#define ADC_SPEED3  0x60    //ADC转换速度，一次转换需要 90个时钟
#define ADC_SPEED2  0x40    //ADC转换速度，一次转换需要180个时钟
#define ADC_SPEED1  0x20    //ADC转换速度，一次转换需要360个时钟
#define ADC_SPEED0  0x00    //ADC转换速度，一次转换需要540个时钟
#define ADC_FLAG    0x10    //ADC完成标志位
#define ADC_START   0x08    //ADC启动控制位


static BOOL bGate = 0;

/*
	NTC Value & Temp
*/
#define ADC_81		263		//ADC = 253 at 81 deg
#define TABLE_COUNT 120		//-40 to 80 deg, had 120 numbers
unsigned char code nTempTable[] = {
		1,	//-40
		2,	//-39
		1,	//-38
		2,	//-37
		1,	//-36
		2,	//-35
		2,	//-34
		2,	//-33
		1,	//-32
		2,	//-31
		3,	//-30
		2,	//-29
		2,	//-28
		2,	//-27
		3,	//-26
		2,	//-25
		3,	//-24
		3,	//-23
		2,	//-22
		3,	//-21
		3,	//-20
		4,	//-19
		3,	//-18
		3,	//-17
		4,	//-16
		4,	//-15
		3,	//-14
		4,	//-13
		4,	//-12
		4,	//-11
		5,	//-10
		4,	//-9
		5,	//-8
		4,	//-7
		5,	//-6
		5,	//-5
		5,	//-4
		5,	//-3
		6,	//-2
		5,	//-1
		6,	//0
		6,	//1
		6,	//2
		6,	//3
		6,	//4
		6,	//5
		7,	//6
		6,	//7
		7,	//8
		7,	//9
		7,	//10
		7,	//11
		7,	//12
		7,	//13
		8,	//14
		7,	//15
		8,	//16
		7,	//17
		8,	//18
		8,	//19
		8,	//20
		8,	//21
		9,	//22
		8,	//23
		8,	//24
		9,	//25
		8,	//26
		8,	//27
		9,	//28
		9,	//29
		8,	//30
		9,	//31
		9,	//32
		8,	//33
		9,	//34
		9,	//35
		9,	//36
		8,	//37
		9,	//38
		9,	//39
		9,	//40
		8,	//41
		9,	//42
		9,	//43
		8,	//44
		9,	//45
		9,	//46
		8,	//47
		9,	//48
		8,	//49
		8,	//50
		8,	//51
		9,	//52
		8,	//53
		8,	//54
		8,	//55
		8,	//56
		8,	//57
		7,	//58
		8,	//59
		7,	//60
		8,	//61
		7,	//62
		7,	//63
		8,	//64
		7,	//65
		7,	//66
		7,	//67
		6,	//68
		7,	//69
		7,	//70
		6,	//71
		6,	//72
		7,	//73
		6,	//74
		6,	//75
		6,	//76
		6,	//77
		5,	//78
		6,	//79
		5	//80
};


static BOOL bInited = 0;

/*
	Init the ADC
*/
void InitADC(void)
{
    P1ASF = P1ASF_NTC;	//Open the ADC of the pin
    PIN_NTC = 0;        //Set to low to measure the pin
    ADC_RES = 0;        //Clear the result
	ADC_RESL = 0;
    ADC_CONTR = ADC_POWER | ADC_SPEED3 | NUM_NTC; //Power on, speed 3, pin #6

	bInited = 1;
}

/*
	Read the ADC value

	Return High 8bit only
*/
unsigned int GetADCResult (void)
//unsigned char Get_ADC_Result(void)
{
    ADC_RES = 0;        //Clear the result
	ADC_RESL = 0;

	if (!bInited)
		InitADC ();

    ADC_CONTR =ADC_POWER|ADC_SPEED3|NUM_NTC|ADC_START;//Power on, speed 3, pin #6, Start ADC

	//Wait for a while to start
    _nop_();    _nop_();
    _nop_();    _nop_();

    while (!(ADC_CONTR & ADC_FLAG));	//Waiting for ADC finished
    
	ADC_CONTR &= ~ADC_FLAG;				//Clear the flag
    
	return (((unsigned int)ADC_RES << 2) | ADC_RESL);
	//return ADC_RES;
}

/*
	Read the temperature

	Return temperature in deg, start from -40 deg. Max is 81 deg.
	for example: return 0 means -40deg, 40 means 0 deg, 61 means 21 deg.
	nLow return the 0.1deg
*/
unsigned char ReadTemperature (unsigned char *nLow)
{
	unsigned int nADC;
	unsigned char nStep;
	unsigned char nCount;


	nADC = GetADCResult () - ADC_81;  //Start from 81 deg

	nStep = 0;		//Start from 81 deg
	nCount = TABLE_COUNT + 1;

	while (nADC >= nStep && nCount)
	{
		nADC -= nStep;
		nCount --;
		nStep = nTempTable [nCount];
	}
	
	*nLow = 0;
	if (nADC == 0)
		return (nCount);	
	if (nCount == 0)
		return (0);

	*nLow = 10 - nADC * 10 / nStep;
	if (*nLow >= 10)
	{
		*nLow = 0;
		return (nCount);
	}
	return (nCount - 1); 
}
