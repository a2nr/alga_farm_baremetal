#include <Arduino.h>
#include <freqGenerator.h>
#include <plclib.h>
#include "device.h"

/*
 * 2 3 4 5 6 7 8 9    10 11 12
 * A 0 1 2 3 4 5 6 7
 */

#define STP_DIR		10
#define STP_PULSE	11
#define STP_EN		12

RS DirectionStepper;
RS PulseEnableStepper;
RS EnableStepper;
R_TRIG RSwitchA; 
R_TRIG RSwitchB; 
F_TRIG FSwitchA; 
F_TRIG FSwitchB; 
R_TRIG RLimitSwitchA; 
R_TRIG RLimitSwitchB; 
bool softStopStepper;

#if defined(DEVICE_1) || defined(DEVICE_DEBUG)

#define MTR_PUMP_CONT	2
#define SEL_CONT	3
#define MTR_PUMP_ISI	4
#define SEL_BILAS	5
#define SEL_TAMPUNG	6
#define HEATER		7
#define AERATOR		8
#define SIKAT_ENABELER 	9 

//define SW_LMT_ATAS	A0
//define SW_LMT_BAWAH	A1
//define SW_BAWAH	A2
//define SW_ATAS		A3
//define SW_SET_BILAS	0
//define SW_SET_TAMPUNG	1	// cant digital read
//define SW_SET_ISI 	13

#define SW_LMT_ATAS	A7
#define SW_LMT_BAWAH	A6
#define SW_BAWAH	A5
#define SW_ATAS		A4
#define SW_SET_BILAS	A3
#define SW_SET_TAMPUNG	A2	// cant digital read
#define SW_SET_ISI 	A1

#define STP_SPEED	400

void init_device_satu()
{
	setPin(STP_DIR,		LOW);	//10
	setPin(STP_PULSE,		LOW);	//11
	setPin(STP_EN,		HIGH);	//12
	freqGenerator_setPeriod(STP_SPEED);	//
	freqGenerator_disable();		//
						//
	setPin(MTR_PUMP_CONT,	HIGH);	//3
	setPin(MTR_PUMP_ISI,	HIGH);	//4
	setPin(SEL_CONT,		HIGH);	//6
	setPin(SEL_BILAS,		HIGH);	//7
	setPin(HEATER,		HIGH);	//8
	setPin(AERATOR,		HIGH);	//8
	setPin(SIKAT_ENABELER,	HIGH);	//9
						//
	initPin(MTR_PUMP_CONT,	OUTPUT);	//3
	initPin(MTR_PUMP_ISI,	OUTPUT);	//4
	initPin(SEL_CONT,	OUTPUT);	//6
	initPin(SEL_BILAS,	OUTPUT);	//7
	initPin(SEL_TAMPUNG,	OUTPUT);	//8
	initPin(SIKAT_ENABELER,	OUTPUT);	//9
	initPin(HEATER,		OUTPUT);
	initPin(AERATOR,	OUTPUT);
						//
	initPin(STP_DIR,	OUTPUT);	//10
	initPin(STP_PULSE,	OUTPUT);	//11
	initPin(STP_EN,		OUTPUT);	//12
						//
	initPin(SW_LMT_ATAS,	INPUT);		//A0
	initPin(SW_LMT_BAWAH,	INPUT);		//A1
	initPin(SW_ATAS,	INPUT);		//A2
	initPin(SW_BAWAH,	INPUT);		//A3
	initPin(SW_SET_ISI,	INPUT);		//A4
	initPin(SW_SET_BILAS,	INPUT);		//A5
	initPin(SW_SET_TAMPUNG,	INPUT);		//A6
}

RS sikatDisabelerLatch;
RS switchBilasLatch;
RS switchMengisiLatch;
RS switchTampungLatch;

R_TRIG RSwitchBilas;
F_TRIG FSwitchBilas;
R_TRIG RSwitchMengisi;
F_TRIG FSwitchMengisi;
R_TRIG RTampungEnable;
F_TRIG FTampungEnable;
R_TRIG RMembilasEnable;
R_TRIG RSikatDisabeler;
R_TRIG RSwitchLimitAtas;
R_TRIG RSwitchLimitBawah;
R_TRIG RSwitchGoToAtas;
R_TRIG RSwitchGoToBawah;
F_TRIG FSwitchGoToAtas;
F_TRIG FSwitchGoToBawah;

TON delayOnMotorSikat(3000);
TON delayOffMengisi(200);
TON delayOffTampung(2000);
TON delayOnSikatDisabeler(500);

void start_device_satu()
{
	bool _switchTmpg 			= getPin(SW_SET_TAMPUNG);
	bool _switchMengisi 			= getPin(SW_SET_ISI); 
	bool _switchBilas 			= getPin(SW_SET_BILAS);
	bool _switchLimitAtas		= analogRead(SW_LMT_ATAS) > 400;
	bool _switchLimitBawah		= analogRead(SW_LMT_BAWAH) > 400;
//	bool _switchLimitAtas		= getPin(SW_LMT_ATAS);
//	bool _switchLimitBawah		= getPin(SW_LMT_BAWAH);
	bool _switchGoToAtas			= getPin(SW_ATAS);
	bool _switchGoToBawah		= getPin(SW_BAWAH);
	// 					  v----bypass switch melalui relay yang hanya aktif ketika penyikat berada dibawah
	bool _sikatDisabeler			= _switchLimitBawah ? (getPin(SW_SET_ISI)) : false;		
	
	bool motorPumpContainerEnable	= !getPin(MTR_PUMP_CONT);
	bool motorPumpMengisiEnable		= !getPin(MTR_PUMP_ISI);
	bool selenoidContainerEnable		= !getPin(SEL_CONT);
	bool selenoidMembilasEnable		= !getPin(SEL_BILAS);
	bool selenoidMenampungEnable		= !getPin(SEL_TAMPUNG);
	bool aerator 			= !getPin(AERATOR);
	bool heater  			= !getPin(HEATER);
	bool sikatEnabeler			= !getPin(SIKAT_ENABELER); 

	RSwitchBilas 		.process(_switchBilas);
	FSwitchBilas 		.process(_switchBilas);
	switchBilasLatch 	.process(RSwitchBilas.Q, RSwitchLimitAtas.Q);// FSwitchBilas.Q || );
	switchMengisiLatch 	.process(RSwitchMengisi.Q, (delayOffMengisi.Q || FSwitchMengisi.Q));
	delayOffMengisi		.process(RSwitchMengisi.Q);
	RSwitchMengisi 		.process(_switchMengisi);
	FSwitchMengisi 		.process(_switchMengisi);

	RTampungEnable 		.process(_switchTmpg);
	FTampungEnable 		.process(_switchTmpg || delayOffTampung.Q);
	switchTampungLatch 	.process(RTampungEnable.Q,FTampungEnable.Q);
	delayOffTampung 	.process(RTampungEnable.Q);

	RSikatDisabeler 	.process(_sikatDisabeler);
	sikatDisabelerLatch 	.process(RSikatDisabeler.Q || FSwitchBilas.Q, _switchLimitAtas);

	RSwitchGoToAtas 	.process(_switchGoToAtas);
	RSwitchGoToBawah 	.process(_switchGoToBawah);
	FSwitchGoToAtas 	.process(_switchGoToAtas);
	FSwitchGoToBawah 	.process(_switchGoToBawah);
	RSwitchLimitAtas 	.process(_switchLimitAtas);
	RSwitchLimitBawah 	.process(_switchLimitBawah);

	sikatEnabeler 			= _switchLimitBawah; // || sikatDisabelerLatch.Q1;
	motorPumpMengisiEnable 		= (!selenoidContainerEnable) && (switchMengisiLatch.Q1) && _switchLimitAtas ;
	selenoidContainerEnable 	= (selenoidMembilasEnable || selenoidMenampungEnable) ;
	motorPumpContainerEnable 	= selenoidContainerEnable;
	selenoidMembilasEnable 		= _switchLimitBawah && (switchBilasLatch.Q1 && (!_sikatDisabeler));
	selenoidMenampungEnable 	= _switchLimitAtas && (switchTampungLatch.Q1);// && delayOffTampung.Q);

	softStopStepper		= RSwitchLimitAtas.Q || RSwitchLimitBawah.Q || FSwitchGoToAtas.Q || FSwitchGoToBawah.Q || FSwitchBilas.Q;
	DirectionStepper 	.process(RSwitchGoToBawah.Q || (RSwitchBilas.Q && (!_sikatDisabeler)),
					RSwitchGoToAtas.Q || RSikatDisabeler.Q);
	EnableStepper		.process((RSwitchGoToAtas.Q && !_switchLimitAtas) || (RSwitchGoToBawah.Q && !_switchLimitBawah) 
					|| RSwitchBilas.Q || RSikatDisabeler.Q ,softStopStepper);
	PulseEnableStepper	.process(EnableStepper.Q1, softStopStepper);
	setPin(STP_DIR, 	!DirectionStepper.Q1);
	setPin(STP_EN, 	!EnableStepper.Q1);
	if (PulseEnableStepper.Q1)
		freqGenerator_enable();
	else
		freqGenerator_disable();

	setPin(MTR_PUMP_CONT,	!(motorPumpContainerEnable));
	setPin(MTR_PUMP_ISI,	!(motorPumpMengisiEnable));
	setPin(SEL_CONT,		!(selenoidContainerEnable));
	setPin(SEL_BILAS,		!(selenoidMembilasEnable));
	setPin(SEL_TAMPUNG,	!(selenoidMenampungEnable));
	setPin(AERATOR, 		!(aerator));
	setPin(HEATER, 		!(heater));
	setPin(SIKAT_ENABELER,	!(sikatEnabeler));

}

#endif

#if defined(DEVICE_2) || defined(DEVICE_DEBUG)

#define MTR_SIKAT 	2
#define MTR_PUMP_BILAS  3
#define REL_MODE	4
#define REL_DONE	5

#define SW_LMT_KANAN	A5
#define SW_LMT_KIRI	A4
#define ENABLE_SIKAT	A3
#define SW_KANAN	A2
#define SW_KIRI		A1
#define SW_BILAS	A6

//#define SW_LMT_KANAN	A0
//#define SW_LMT_KIRI	A1
//#define ENABLE_SIKAT	A2
//#define SW_KANAN	A3
//#define SW_KIRI		0

#define STP_SPEED	300

void init_device_dua()
{

	setPin(STP_DIR,		LOW);	//10
	setPin(STP_PULSE,		LOW);	//11
	setPin(STP_EN,		HIGH);	//12
	freqGenerator_setPeriod(STP_SPEED);	//
	freqGenerator_disable();		//
						//
	setPin(SW_LMT_KANAN,	HIGH); //A7
	setPin(SW_LMT_KIRI,	HIGH); //A6
	setPin(SW_KANAN,		HIGH); //A5
	setPin(SW_KIRI,		HIGH); //A4
	setPin(ENABLE_SIKAT,	HIGH);

	setPin(MTR_SIKAT ,	HIGH);	//2
	setPin(MTR_PUMP_BILAS,	HIGH);	//3
	setPin(REL_MODE,	 	HIGH);	//4
	setPin(REL_DONE,	 	HIGH);	//5

	initPin(SW_LMT_KANAN,	INPUT);		//A0
	initPin(SW_LMT_KIRI,	INPUT);		//A1
	initPin(SW_KANAN,	INPUT);		//A2
	initPin(SW_KIRI,	INPUT);		//A3
	initPin(ENABLE_SIKAT, 	INPUT);
	initPin(SW_BILAS, 	INPUT);

	initPin(MTR_SIKAT ,	OUTPUT);	//2
	initPin(MTR_PUMP_BILAS,	OUTPUT);	//3
	initPin(REL_MODE, 	OUTPUT);	//4
	initPin(REL_DONE, 	OUTPUT);	//5

	initPin(STP_DIR,	OUTPUT);	//10
	initPin(STP_PULSE,	OUTPUT);	//11
	initPin(STP_EN,		OUTPUT);	//12
}

RS SwitchBilasDua;
R_TRIG RSwitchKanan; 
R_TRIG RSwitchKiri; 
F_TRIG FSwitchKanan; 
F_TRIG FSwitchKiri; 
R_TRIG RLimitSwitchKanan; 
R_TRIG RLimitSwitchKiri; 
R_TRIG RSwitchBilasDua; 
F_TRIG FSwitchBilasDua; 

R_TRIG 	REnableSikat;
F_TRIG 	FEnableSikat;
RS 	enableSikat;
RS 	disabelerSikat;
CTU 	sikatJobCounter(5);

void start_device_dua()
{
	bool _switchGoToKanan 	= getPin(SW_KANAN);
	bool _switchGoToKiri		= getPin(SW_KIRI);
	bool _switchLimitKanan	= getPin(SW_LMT_KANAN);
	bool _switchLimitKiri	= getPin(SW_LMT_KIRI);
	bool _enableSikat 		= getPin(ENABLE_SIKAT);
	bool _switchBilas		= analogRead(SW_BILAS) > 400;

	bool motorSikat  		= !getPin(MTR_SIKAT);	//2
	bool motorPumpBilas 		= !getPin(MTR_PUMP_BILAS);	//3
	bool relayMode 		= !getPin(REL_MODE);	//4
	bool jobDone 		= !getPin(REL_DONE);	//5
									

	REnableSikat 		.process(_enableSikat);
	FEnableSikat 		.process(_enableSikat);
	disabelerSikat 		.process(sikatJobCounter.Q, REnableSikat.Q);
	enableSikat 		.process(REnableSikat.Q /*_enableSikat*/,disabelerSikat.Q1 && _switchLimitKiri); 

	RSwitchKiri 		.process(_switchGoToKiri);
	RSwitchKanan 		.process(_switchGoToKanan);
	FSwitchKiri 		.process(_switchGoToKiri);
	FSwitchKanan 		.process(_switchGoToKanan);
	RLimitSwitchKiri 	.process(_switchLimitKiri);
	RLimitSwitchKanan 	.process(_switchLimitKanan);
	RSwitchBilasDua 		.process(_switchBilas);
	FSwitchBilasDua 		.process(_switchBilas);
	SwitchBilasDua 		.process(RSwitchBilasDua.Q, FSwitchBilasDua.Q);

	DirectionStepper 	.process(_switchGoToKiri , //|| (_switchLimitKanan && enableSikat.Q1) ,
					 _switchGoToKanan); // || (_switchLimitKiri && enableSikat.Q1));
//	softStopStepper 	.process(FSwitchKiri.Q || FSwitchKanan.Q || RLimitSwitchKiri.Q || RLimitSwitchKanan.Q , // ((_switchLimitKanan || _switchLimitKiri) && !enableSikat.Q1),
	softStopStepper		= RLimitSwitchKiri.Q || RLimitSwitchKanan.Q || FSwitchKanan.Q || FSwitchKiri.Q;
	PulseEnableStepper	.process(EnableStepper.Q1, softStopStepper);
	EnableStepper		.process((RSwitchKiri.Q && !_switchLimitKiri) || (RSwitchKanan.Q && !_switchLimitKanan), //REnableSikat.Q ||
		       			softStopStepper);
	sikatJobCounter 	.process(RLimitSwitchKiri.Q, REnableSikat.Q || FEnableSikat.Q);

	motorSikat 		= SwitchBilasDua.Q1;
	motorPumpBilas 		= SwitchBilasDua.Q1;
	relayMode 		= SwitchBilasDua.Q1;
	jobDone 		= FSwitchBilasDua.Q;

	setPin(STP_DIR, 	!DirectionStepper.Q1 );
	setPin(STP_EN, 	!EnableStepper.Q1);
	if (PulseEnableStepper.Q1)
		freqGenerator_enable();
	else
		freqGenerator_disable();

	setPin(MTR_SIKAT ,	!(motorSikat));	//2
	setPin(MTR_PUMP_BILAS,	!(motorPumpBilas));	//3
	setPin(REL_MODE,	 	!(relayMode));	//4
	setPin(REL_DONE,	 	!(jobDone));	//5
}
#endif

void init_device()
{
#if defined(DEVICE_1)
	init_device_satu();
#elif defined(DEVICE_2)
	init_device_dua();
#else
	init_device_satu();
	init_device_dua();
#endif
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
