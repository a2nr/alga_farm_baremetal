#include <plclib.h>
#include "device.h"
#include "freqGenerator.h"

#if defined(DEVICE_1)
void init_device_satu();
void start_device_satu();
#elif defined(DEVICE_2)
void init_device_dua();
void start_device_dua();
#else
void init_device_satu();
void start_device_satu();
void init_device_dua();
void start_device_dua();
#endif

#define STP_DIR			PIN_OUTPUT8
#define STP_PULSE		PIN_OUTPUT9
#define STP_EN			PIN_OUTPUT10


SR		stepperDirection;
RS		stepperEnable;

void init_device()
{
	setPin(STP_DIR,			PIN_LOW);	
	setPin(STP_PULSE,		PIN_LOW);	
	setPin(STP_EN,			PIN_HIGH);	

#if defined(DEVICE_1)
	init_device_satu();
#elif defined(DEVICE_2)
	init_device_dua();
#else
	init_device_satu();
	init_device_dua();
#endif

	initPin(STP_DIR,	PIN_OUTPUT);	
	initPin(STP_PULSE,	PIN_OUTPUT);	
	initPin(STP_EN,		PIN_OUTPUT);
}
void start_device()
{
#if defined(DEVICE_1)
	start_device_satu();
#elif defined(DEVICE_2)
	start_device_dua();
#else
	start_device_satu();
	start_device_dua();
#endif
}

/*
 * 2 3 4 5 6 7 8 9    10 11 12
 * A 0 1 2 3 4 5 6 7
 */

#if defined(DEVICE_1) || defined(DEVICE_DEBUG)

#define MTR_PUMP_CONT		PIN_OUTPUT0
#define SEL_CONT			PIN_OUTPUT1
#define MTR_PUMP_ISI		PIN_OUTPUT2
#define SEL_BILAS			PIN_OUTPUT3
#define SEL_TAMPUNG			PIN_OUTPUT4
#define HEATER				PIN_OUTPUT5
#define AERATOR				PIN_OUTPUT6
#define SIKAT_ENABELER	 	PIN_OUTPUT7 

//define SW_LMT_ATAS	A0
//define SW_LMT_BAWAH	A1
//define SW_BAWAH	A2
//define SW_ATAS		A3
//define SW_SET_BILAS	0
//define SW_SET_TAMPUNG	1	// cant digital read
//define SW_SET_ISI 	13

#define SW_LMT_ATAS			PIN_INPUT7 //A7
#define SW_LMT_BAWAH		PIN_INPUT6 //A6
#define SW_BAWAH			PIN_INPUT5 //A5
#define SW_ATAS				PIN_INPUT4 //A4
#define SW_SET_BILAS		PIN_INPUT3 //A3
#define SW_SET_TAMPUNG		PIN_INPUT2 //A2	// cant digital read
#define SW_SET_ISI 			PIN_INPUT1 //A1

#define STP_SPEED_DSATU	400

void init_device_satu()
{

    freqGenerator_setPeriod(FREQGEN_CHANNEL_(0), STP_SPEED_DSATU);
	freqGenerator_setOut(FREQGEN_CHANNEL_(0), false);	

	setPin(MTR_PUMP_CONT,	PIN_HIGH);	
	setPin(MTR_PUMP_ISI,	PIN_HIGH);	
	setPin(SEL_CONT,		PIN_HIGH);	
	setPin(SEL_BILAS,		PIN_HIGH);	
	setPin(SEL_TAMPUNG,		PIN_HIGH);
	setPin(HEATER,			PIN_HIGH);	
	setPin(AERATOR,			PIN_HIGH);	
	setPin(SIKAT_ENABELER,	PIN_HIGH);	
						
	initPin(MTR_PUMP_CONT,	PIN_OUTPUT);	
	initPin(MTR_PUMP_ISI,	PIN_OUTPUT);	
	initPin(SEL_CONT,		PIN_OUTPUT);	
	initPin(SEL_BILAS,		PIN_OUTPUT);	
	initPin(SEL_TAMPUNG,	PIN_OUTPUT);	
	initPin(SIKAT_ENABELER,	PIN_OUTPUT);	
	initPin(HEATER,			PIN_OUTPUT);
	initPin(AERATOR,		PIN_OUTPUT);

	initPin(SW_LMT_ATAS,	PIN_INPUT);		
	initPin(SW_LMT_BAWAH,	PIN_INPUT);		
	initPin(SW_ATAS,		PIN_INPUT);		
	initPin(SW_BAWAH,		PIN_INPUT);		
	initPin(SW_SET_ISI,		PIN_INPUT);		
	initPin(SW_SET_BILAS,	PIN_INPUT);		
	initPin(SW_SET_TAMPUNG,	PIN_INPUT);		
}

TON 		timerOn5second(5000);
TOF 		timerOff5second(5000);

void start_device_satu()
{
	bool	_switchLimitBawah	=	getPin(SW_LMT_BAWAH);
	bool	_switchTampung		=	getPin(SW_SET_TAMPUNG);

	bool	sikatEnabeler		=	!getPin(SIKAT_ENABELER);


	timerOn5second			.process(_switchLimitBawah);	
	sikatEnabeler			= timerOn5second.Q ;	

	setPin(SIKAT_ENABELER, !sikatEnabeler);
	setPin(MTR_PUMP_CONT, !_switchTampung);
	setPin(SEL_CONT, !_switchTampung);
	setPin(SEL_TAMPUNG, !_switchTampung);

}

#endif

#if defined(DEVICE_2) || defined(DEVICE_DEBUG)

#define MTR_SIKAT 		PIN_OUTPUT0//2
#define MTR_PUMP_BILAS  PIN_OUTPUT1//3
#define REL_MODE		PIN_OUTPUT2//4
#define REL_DONE		PIN_OUTPUT3//5

#define SW_BILAS		PIN_INPUT6//A6
#define SW_LMT_KANAN	PIN_INPUT5//A5
#define SW_LMT_KIRI		PIN_INPUT4//A4
#define ENABLE_SIKAT	PIN_INPUT3//A3
#define SW_KANAN		PIN_INPUT2//A2
#define SW_KIRI			PIN_INPUT1//A1

//#define SW_LMT_KANAN	A0
//#define SW_LMT_KIRI	A1
//#define ENABLE_SIKAT	A2
//#define SW_KANAN	A3
//#define SW_KIRI		0

#define STP_SPEED_DDUA	300

void init_device_dua()
{

    freqGenerator_setPeriod(FREQGEN_CHANNEL_(1), STP_SPEED_DDUA);
	freqGenerator_setOut(FREQGEN_CHANNEL_(1),false);	

	setPin(SW_LMT_KANAN,	PIN_HIGH); 
	setPin(SW_LMT_KIRI,		PIN_HIGH); 
	setPin(SW_KANAN,		PIN_HIGH); 
	setPin(SW_KIRI,			PIN_HIGH); 
	setPin(ENABLE_SIKAT,	PIN_HIGH);

	setPin(MTR_SIKAT ,		PIN_HIGH);	
	setPin(MTR_PUMP_BILAS,	PIN_HIGH);	
	setPin(REL_MODE,	 	PIN_HIGH);	
	setPin(REL_DONE,	 	PIN_HIGH);	

	initPin(SW_LMT_KANAN,	PIN_INPUT);		
	initPin(SW_LMT_KIRI,	PIN_INPUT);		
	initPin(SW_KANAN,		PIN_INPUT);		
	initPin(SW_KIRI,		PIN_INPUT);		
	initPin(ENABLE_SIKAT, 	PIN_INPUT);
	initPin(SW_BILAS, 		PIN_INPUT);

	initPin(MTR_SIKAT ,		PIN_OUTPUT);	
	initPin(MTR_PUMP_BILAS,	PIN_OUTPUT);	
	initPin(REL_MODE, 		PIN_OUTPUT);	
	initPin(REL_DONE, 		PIN_OUTPUT);	

}

R_TRIG		risingTrigSikatEnabeler;
R_TRIG 		risingTrigLimitKiri;
CTD			counterDownMenyikat(5);
TON			timerOnJobDone(5000);
TOF			timerOffIsiBilas(60000);
F_TRIG		fallingTrigIsiBilas;


void start_device_dua()
{
	bool 	_switchLimitKiri 	= 	getPin(SW_LMT_KIRI);
	bool 	_switchLimitKanan 	= 	getPin(SW_LMT_KANAN);
	bool	_sikatEnabeler 		=	getPin(ENABLE_SIKAT);

	bool	jobDone				=	!getPin(REL_DONE);
	bool	mtrPenyikat			=	!getPin(MTR_SIKAT);
	bool	mtrPembilas			=	!getPin(MTR_PUMP_BILAS);

	risingTrigSikatEnabeler		.process(_sikatEnabeler);
	timerOffIsiBilas			.process(risingTrigSikatEnabeler.Q);
	fallingTrigIsiBilas 		.process(timerOffIsiBilas.Q);
	counterDownMenyikat			.process(risingTrigLimitKiri.process(_switchLimitKiri)
										,risingTrigSikatEnabeler.Q);
	jobDone 					= counterDownMenyikat.Q && _sikatEnabeler;

	stepperDirection			.process(_switchLimitKanan,_switchLimitKiri);
	stepperEnable				.process(fallingTrigIsiBilas.Q, jobDone);
	timerOnJobDone				.process(jobDone);
	mtrPenyikat 				= stepperEnable.Q1;
	mtrPembilas					= timerOffIsiBilas.Q;

	setPin(MTR_SIKAT,	!mtrPenyikat);
	setPin(MTR_PUMP_BILAS, !mtrPembilas);	
	setPin(REL_DONE, 	!timerOnJobDone.Q);
	setPin(STP_DIR, 	!stepperDirection.Q1);
	setPin(STP_EN, 		!stepperEnable.Q1);
	freqGenerator_setOut(FREQGEN_CHANNEL_(1), stepperEnable.Q1);
}
#endif
