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
	digitalWrite(STP_DIR,		LOW);	//10
	digitalWrite(STP_PULSE,		LOW);	//11
	digitalWrite(STP_EN,		HIGH);	//12
	freqGenerator_setPeriod(STP_SPEED);	//
	freqGenerator_disable();		//
						//
	digitalWrite(MTR_PUMP_CONT,	HIGH);	//3
	digitalWrite(MTR_PUMP_ISI,	HIGH);	//4
	digitalWrite(SEL_CONT,		HIGH);	//6
	digitalWrite(SEL_BILAS,		HIGH);	//7
	digitalWrite(HEATER,		HIGH);	//8
	digitalWrite(AERATOR,		HIGH);	//8
	digitalWrite(SIKAT_ENABELER,	HIGH);	//9
						//
	pinMode(MTR_PUMP_CONT,	OUTPUT);	//3
	pinMode(MTR_PUMP_ISI,	OUTPUT);	//4
	pinMode(SEL_CONT,	OUTPUT);	//6
	pinMode(SEL_BILAS,	OUTPUT);	//7
	pinMode(SEL_TAMPUNG,	OUTPUT);	//8
	pinMode(SIKAT_ENABELER,	OUTPUT);	//9
	pinMode(HEATER,		OUTPUT);
	pinMode(AERATOR,	OUTPUT);
						//
	pinMode(STP_DIR,	OUTPUT);	//10
	pinMode(STP_PULSE,	OUTPUT);	//11
	pinMode(STP_EN,		OUTPUT);	//12
						//
	pinMode(SW_LMT_ATAS,	INPUT);		//A0
	pinMode(SW_LMT_BAWAH,	INPUT);		//A1
	pinMode(SW_ATAS,	INPUT);		//A2
	pinMode(SW_BAWAH,	INPUT);		//A3
	pinMode(SW_SET_ISI,	INPUT);		//A4
	pinMode(SW_SET_BILAS,	INPUT);		//A5
	pinMode(SW_SET_TAMPUNG,	INPUT);		//A6
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
	bool _switchTmpg 			= digitalRead(SW_SET_TAMPUNG);
	bool _switchMengisi 			= digitalRead(SW_SET_ISI); 
	bool _switchBilas 			= digitalRead(SW_SET_BILAS);
	bool _switchLimitAtas		= analogRead(SW_LMT_ATAS) > 400;
	bool _switchLimitBawah		= analogRead(SW_LMT_BAWAH) > 400;
//	bool _switchLimitAtas		= digitalRead(SW_LMT_ATAS);
//	bool _switchLimitBawah		= digitalRead(SW_LMT_BAWAH);
	bool _switchGoToAtas			= digitalRead(SW_ATAS);
	bool _switchGoToBawah		= digitalRead(SW_BAWAH);
	// 					  v----bypass switch melalui relay yang hanya aktif ketika penyikat berada dibawah
	bool _sikatDisabeler			= _switchLimitBawah ? (digitalRead(SW_SET_ISI)) : false;		
	
	bool motorPumpContainerEnable	= !digitalRead(MTR_PUMP_CONT);
	bool motorPumpMengisiEnable		= !digitalRead(MTR_PUMP_ISI);
	bool selenoidContainerEnable		= !digitalRead(SEL_CONT);
	bool selenoidMembilasEnable		= !digitalRead(SEL_BILAS);
	bool selenoidMenampungEnable		= !digitalRead(SEL_TAMPUNG);
	bool aerator 			= !digitalRead(AERATOR);
	bool heater  			= !digitalRead(HEATER);
	bool sikatEnabeler			= !digitalRead(SIKAT_ENABELER); 

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
	digitalWrite(STP_DIR, 	!DirectionStepper.Q1);
	digitalWrite(STP_EN, 	!EnableStepper.Q1);
	if (PulseEnableStepper.Q1)
		freqGenerator_enable();
	else
		freqGenerator_disable();

	digitalWrite(MTR_PUMP_CONT,	!(motorPumpContainerEnable));
	digitalWrite(MTR_PUMP_ISI,	!(motorPumpMengisiEnable));
	digitalWrite(SEL_CONT,		!(selenoidContainerEnable));
	digitalWrite(SEL_BILAS,		!(selenoidMembilasEnable));
	digitalWrite(SEL_TAMPUNG,	!(selenoidMenampungEnable));
	digitalWrite(AERATOR, 		!(aerator));
	digitalWrite(HEATER, 		!(heater));
	digitalWrite(SIKAT_ENABELER,	!(sikatEnabeler));

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

	digitalWrite(STP_DIR,		LOW);	//10
	digitalWrite(STP_PULSE,		LOW);	//11
	digitalWrite(STP_EN,		HIGH);	//12
	freqGenerator_setPeriod(STP_SPEED);	//
	freqGenerator_disable();		//
						//
	digitalWrite(SW_LMT_KANAN,	HIGH); //A7
	digitalWrite(SW_LMT_KIRI,	HIGH); //A6
	digitalWrite(SW_KANAN,		HIGH); //A5
	digitalWrite(SW_KIRI,		HIGH); //A4
	digitalWrite(ENABLE_SIKAT,	HIGH);

	digitalWrite(MTR_SIKAT ,	HIGH);	//2
	digitalWrite(MTR_PUMP_BILAS,	HIGH);	//3
	digitalWrite(REL_MODE,	 	HIGH);	//4
	digitalWrite(REL_DONE,	 	HIGH);	//5

	pinMode(SW_LMT_KANAN,	INPUT);		//A0
	pinMode(SW_LMT_KIRI,	INPUT);		//A1
	pinMode(SW_KANAN,	INPUT);		//A2
	pinMode(SW_KIRI,	INPUT);		//A3
	pinMode(ENABLE_SIKAT, 	INPUT);
	pinMode(SW_BILAS, 	INPUT);

	pinMode(MTR_SIKAT ,	OUTPUT);	//2
	pinMode(MTR_PUMP_BILAS,	OUTPUT);	//3
	pinMode(REL_MODE, 	OUTPUT);	//4
	pinMode(REL_DONE, 	OUTPUT);	//5

	pinMode(STP_DIR,	OUTPUT);	//10
	pinMode(STP_PULSE,	OUTPUT);	//11
	pinMode(STP_EN,		OUTPUT);	//12
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
	bool _switchGoToKanan 	= digitalRead(SW_KANAN);
	bool _switchGoToKiri		= digitalRead(SW_KIRI);
	bool _switchLimitKanan	= digitalRead(SW_LMT_KANAN);
	bool _switchLimitKiri	= digitalRead(SW_LMT_KIRI);
	bool _enableSikat 		= digitalRead(ENABLE_SIKAT);
	bool _switchBilas		= analogRead(SW_BILAS) > 400;

	bool motorSikat  		= !digitalRead(MTR_SIKAT);	//2
	bool motorPumpBilas 		= !digitalRead(MTR_PUMP_BILAS);	//3
	bool relayMode 		= !digitalRead(REL_MODE);	//4
	bool jobDone 		= !digitalRead(REL_DONE);	//5
									

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

	digitalWrite(STP_DIR, 	!DirectionStepper.Q1 );
	digitalWrite(STP_EN, 	!EnableStepper.Q1);
	if (PulseEnableStepper.Q1)
		freqGenerator_enable();
	else
		freqGenerator_disable();

	digitalWrite(MTR_SIKAT ,	!(motorSikat));	//2
	digitalWrite(MTR_PUMP_BILAS,	!(motorPumpBilas));	//3
	digitalWrite(REL_MODE,	 	!(relayMode));	//4
	digitalWrite(REL_DONE,	 	!(jobDone));	//5
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
