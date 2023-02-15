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

/*
 * 2 3 4 5 6 7 8 9    10 11 12
 * A 0 1 2 3 4 5 6 7
 */
//defice 1
#define MTR_PUMP_CONT       PIN_OUTPUT0
#define SEL_CONT            PIN_OUTPUT1
#define MTR_PUMP_ISI        PIN_OUTPUT2
#define SEL_BILAS           PIN_OUTPUT3
#define SEL_TAMPUNG         PIN_OUTPUT4
#define HEATER              PIN_OUTPUT5
#define AERATOR             PIN_OUTPUT6
#define SIKAT_ENABELER      PIN_OUTPUT7
#define DEVICE1_STP_DIR     PIN_OUTPUT8
#define DEVICE1_STP_PERIOD      0
#define DEVICE1_STP_PULSE   PIN_OUTPUT9
#define DEVICE1_STP_EN      PIN_OUTPUT10

#define SW_LMT_ATAS         PIN_INPUT7 /*A7*/
#define SW_LMT_BAWAH        PIN_INPUT6 /*A6*/
#define SW_BAWAH            PIN_INPUT5 /*A5*/
#define SW_ATAS             PIN_INPUT4 /*A4*/
#define SW_SET_BILAS        PIN_INPUT3 /*A3*/
#define SW_SET_TAMPUNG      PIN_INPUT2 /*A2 cant digital read*/
#define SW_SET_ISI          PIN_INPUT1 /*A1*/

//defice 2
#define MTR_SIKAT           PIN_OUTPUT11
#define MTR_PUMP_BILAS      PIN_OUTPUT12
#define REL_MODE            PIN_OUTPUT13
#define REL_DONE            PIN_OUTPUT14
#define DEVICE2_STP_DIR     PIN_OUTPUT19
#define DEVICE2_STP_PERIOD      1
#define DEVICE2_STP_PULSE   PIN_OUTPUT20
#define DEVICE2_STP_EN      PIN_OUTPUT21

#define SW_BILASISI         PIN_INPUT15/*A7*/
#define SW_PENYIKAT         PIN_INPUT14/*A6*/
#define SW_LMT_KANAN        PIN_INPUT13/*A5*/
#define SW_LMT_KIRI         PIN_INPUT12/*A4*/
#define ENABLE_SIKAT        PIN_INPUT11/*A3*/
#define SW_KANAN            PIN_INPUT10/*A2*/
#define SW_KIRI             PIN_INPUT9/*A1*/

#define DEVICE2_STP_SPEED   300
#define DEVICE1_STP_SPEED	400

#if defined(DEVICE_1) || defined(DEVICE_DEBUG)

void init_device_satu()
{

  setPin(DEVICE1_STP_DIR, PIN_LOW);
  setPin(DEVICE1_STP_PULSE, PIN_LOW);
  setPin(DEVICE1_STP_EN, PIN_HIGH);

  freqGenerator_setPeriod(FREQGEN_CHANNEL_(DEVICE1_STP_PERIOD), DEVICE1_STP_SPEED);
  freqGenerator_setOut(FREQGEN_CHANNEL_(DEVICE1_STP_PULSE), false);

  setPin(MTR_PUMP_CONT, PIN_HIGH);
  setPin(MTR_PUMP_ISI, PIN_HIGH);
  setPin(SEL_CONT, PIN_HIGH);
  setPin(SEL_BILAS, PIN_HIGH);
  setPin(SEL_TAMPUNG, PIN_HIGH);
  setPin(HEATER, PIN_HIGH);
  setPin(AERATOR, PIN_HIGH);
  setPin(SIKAT_ENABELER, PIN_HIGH);

  initPin(MTR_PUMP_CONT, PIN_OUTPUT);
  initPin(MTR_PUMP_ISI,	PIN_OUTPUT);
  initPin(SEL_CONT, PIN_OUTPUT);
  initPin(SEL_BILAS, PIN_OUTPUT);
  initPin(SEL_TAMPUNG, PIN_OUTPUT);
  initPin(SIKAT_ENABELER, PIN_OUTPUT);
  initPin(HEATER, PIN_OUTPUT);
  initPin(AERATOR, PIN_OUTPUT);

  initPin(SW_LMT_ATAS, PIN_INPUT);
  initPin(SW_LMT_BAWAH,	PIN_INPUT);
  initPin(SW_ATAS, PIN_INPUT);
  initPin(SW_BAWAH, PIN_INPUT);
  initPin(SW_SET_ISI, PIN_INPUT);
  initPin(SW_SET_BILAS, PIN_INPUT);
  initPin(SW_SET_TAMPUNG, PIN_INPUT);

  initPin(DEVICE1_STP_DIR, PIN_OUTPUT);
  initPin(DEVICE1_STP_PULSE, PIN_OUTPUT);
  initPin(DEVICE1_STP_EN, PIN_OUTPUT);
}


R_TRIG risingTrigGotoAtas;
R_TRIG risingTrigGotoBawah;
F_TRIG fallingTrigGotoAtas;
F_TRIG fallingTrigGotoBawah;
SR  device1StepperDirection;
RS  device1StepperEnable;
TON timerOn5second(5000);
R_TRIG  risingTrigSwitchBilas;
R_TRIG  risingTrigLimitBawah;
R_TRIG  risingTrigLimitAtas;

RS  switchMenguras;
F_TRIG  fallingTrigSwMenguras;
TON timerOnMengurasDone(60000);

RS  switchMengisi;
F_TRIG  fallingTrigSwMenisi;
TON timerOnMengisiDone(60000);

RS  switchMenampung;
F_TRIG  fallingTrigSwMenampung;
TON timerOnMenampungDone(60000);

RS  latchSiaktDisabeler;
R_TRIG  risingSikatDisabeler;

RS  membilasEnabeler;
R_TRIG  risingTrigBilas;
TON timerOnBilasEnabeler(2000);

RS  heaterEnabeler;
TON timerOnHeatingDone(60000);

RS  aeratorEnabeler;
F_TRIG  fallingTrigAerator;
TON timerOnAeratorDone(3000);

void start_device_satu()
{
  bool  _switchGotoAtas     =   getPin(SW_ATAS);
  bool  _switchGotoBawah    =   getPin(SW_BAWAH);
  bool	_switchLimitAtas    =   getPin(SW_LMT_ATAS);
  bool	_switchLimitBawah   =   getPin(SW_LMT_BAWAH);
  bool	_switchTampung      =   getPin(SW_SET_TAMPUNG);
  bool  _switchBilas        =   getPin(SW_SET_BILAS);
  bool  _switchMengisi      =   _switchLimitBawah ? false : getPin(SW_SET_ISI); // Baypassed ketika penyikat dibawah

  bool  sikatDisabeler      =   _switchLimitBawah ? getPin(SW_SET_ISI): false; // baypassed saklar isi untuk bilas treatment
  bool	sikatEnabeler;

  timerOnBilasEnabeler      .process(_switchBilas && !(fallingTrigAerator.Q));
  membilasEnabeler          .process(risingTrigBilas.process(timerOnBilasEnabeler.Q)
                                     , timerOnMengisiDone.Q);

  sikatEnabeler             = timerOn5second.process(_switchLimitBawah && membilasEnabeler.Q1);

  risingTrigGotoAtas        .process(_switchGotoAtas && !_switchLimitAtas);
  risingTrigGotoBawah       .process(_switchGotoBawah && !_switchLimitBawah);
  fallingTrigGotoAtas       .process(_switchGotoAtas);
  fallingTrigGotoBawah       .process(_switchGotoBawah);
  device1StepperDirection   .process(_switchLimitBawah || risingTrigGotoAtas.Q
                                     , _switchLimitAtas || risingTrigGotoBawah.Q);

  device1StepperEnable      .process((timerOnMenampungDone.Q && membilasEnabeler.Q1)
                                     || risingSikatDisabeler.process(sikatDisabeler && _switchLimitBawah)
                                     || risingTrigGotoAtas.Q || risingTrigGotoBawah.Q
                                     , risingTrigLimitAtas.process(_switchLimitAtas)
                                     || risingTrigLimitBawah.process(_switchLimitBawah)
                                     || fallingTrigGotoAtas.Q || fallingTrigGotoBawah.Q);

  switchMenampung           .process(_switchTampung || risingTrigSwitchBilas.process(membilasEnabeler.Q1)
                                     , timerOnMenampungDone.Q
                                     || fallingTrigSwMenampung.process(_switchTampung));
  timerOnMenampungDone      .process(switchMenampung.Q1 && membilasEnabeler.Q1);

  latchSiaktDisabeler       .process(sikatDisabeler, timerOnMengurasDone.Q);
  switchMenguras            .process(latchSiaktDisabeler.Q1
                                     , timerOnMengurasDone.Q);
  timerOnMengurasDone       .process(switchMenguras.Q1 && membilasEnabeler.Q1);

  heaterEnabeler            .process(timerOnMengurasDone.Q
                                     , timerOnHeatingDone.Q);
  timerOnHeatingDone        .process(heaterEnabeler.Q1);

  switchMengisi             .process((_switchMengisi && !latchSiaktDisabeler.Q1)
                                     || timerOnHeatingDone.Q
                                     , timerOnMengisiDone.Q);
  timerOnMengisiDone        .process(switchMengisi.Q1 );

  aeratorEnabeler           .process(timerOnMengisiDone.Q
                                     , timerOnAeratorDone.Q);
  fallingTrigAerator        .process(aeratorEnabeler.Q1);
  timerOnAeratorDone        .process(aeratorEnabeler.Q1);


  setPin(AERATOR, !aeratorEnabeler.Q1);
  setPin(HEATER, !heaterEnabeler.Q1);
  setPin(MTR_PUMP_ISI, !switchMengisi.Q1);
  setPin(MTR_PUMP_CONT, !(switchMenampung.Q1 || switchMenguras.Q1));
  setPin(SEL_CONT, !(switchMenampung.Q1 || switchMenguras.Q1));
  setPin(SEL_BILAS, !switchMenguras.Q1);
  setPin(SEL_TAMPUNG, !switchMenampung.Q1);
  setPin(SIKAT_ENABELER, !sikatEnabeler);
  setPin(DEVICE1_STP_DIR, !device1StepperDirection.Q1);
  setPin(DEVICE1_STP_EN, !device1StepperEnable.Q1);
  freqGenerator_setOut(FREQGEN_CHANNEL_(DEVICE1_STP_PULSE), device1StepperEnable.Q1);
}

#endif

#if defined(DEVICE_2) || defined(DEVICE_DEBUG)

void init_device_dua()
{
  setPin(DEVICE2_STP_DIR, PIN_LOW);
  setPin(DEVICE2_STP_PULSE, PIN_LOW);
  setPin(DEVICE2_STP_EN, PIN_HIGH);

  freqGenerator_setPeriod(FREQGEN_CHANNEL_(DEVICE2_STP_PERIOD), DEVICE2_STP_SPEED);
  freqGenerator_setOut(FREQGEN_CHANNEL_(DEVICE2_STP_PULSE),false);

//  setPin(SW_LMT_KANAN, PIN_HIGH);
//  setPin(SW_LMT_KIRI, PIN_HIGH);
//  setPin(SW_KANAN, PIN_HIGH);
//  setPin(SW_KIRI, PIN_HIGH);
//  setPin(ENABLE_SIKAT, PIN_HIGH);

  setPin(MTR_SIKAT ,PIN_HIGH);
  setPin(MTR_PUMP_BILAS, PIN_HIGH);
  setPin(REL_MODE, PIN_HIGH);
  setPin(REL_DONE, PIN_HIGH);

  initPin(SW_LMT_KANAN, PIN_INPUT);
  initPin(SW_LMT_KIRI, PIN_INPUT);
  initPin(SW_KANAN, PIN_INPUT);
  initPin(SW_KIRI, PIN_INPUT);
  initPin(ENABLE_SIKAT, PIN_INPUT);
  initPin(SW_PENYIKAT, PIN_INPUT);
  initPin(SW_BILASISI, PIN_INPUT);

  initPin(MTR_SIKAT, PIN_OUTPUT);
  initPin(MTR_PUMP_BILAS, PIN_OUTPUT);
  initPin(REL_MODE, PIN_OUTPUT);
  initPin(REL_DONE, PIN_OUTPUT);

  initPin(DEVICE2_STP_DIR, PIN_OUTPUT);
  initPin(DEVICE2_STP_PULSE, PIN_OUTPUT);
  initPin(DEVICE2_STP_EN, PIN_OUTPUT);
}

SR  device2StepperDirection;
RS  device2StepperEnable;
R_TRIG  risingTrigLimitKiri;
R_TRIG  risingTrigLimitKanan;
R_TRIG  risingTrigGotoKiri;
R_TRIG  risingTrigGotoKanan;
F_TRIG  fallingTrigGotoKiri;
F_TRIG  fallingTrigGotoKanan;

R_TRIG  risingTrigSikatEnabeler;
F_TRIG  fallingTrigSikatEnabeler;
CTD     counterDownMenyikat(5);
TON     timerOnJobDone(5000);
TOF     timerOffJobDone(5000);
R_TRIG  risingTrigJobDone;
TOF     timerOffIsiBilas(60000);
F_TRIG  fallingTrigIsiBilas;


void start_device_dua()
{
  bool  _switchGotoKiri   = getPin(SW_KIRI);
  bool  _switchGotoKanan  = getPin(SW_KANAN);
  bool 	_switchLimitKiri  = getPin(SW_LMT_KIRI);
  bool 	_switchLimitKanan = getPin(SW_LMT_KANAN);
  bool	_sikatEnabeler    = getPin(ENABLE_SIKAT);
  bool  _switchPenyikat   = getPin(SW_PENYIKAT);
  bool  _switchBilasIsi   = getPin(SW_BILASISI);

  bool  jobDone;
  bool  mtrPenyikat;
  bool  mtrPembilas;


  risingTrigSikatEnabeler   .process(_sikatEnabeler);
  counterDownMenyikat       .process(risingTrigLimitKiri.process(_switchLimitKiri)
                                     , risingTrigSikatEnabeler.Q);
  jobDone                   = counterDownMenyikat.Q;

  risingTrigGotoKiri        .process(_switchGotoKiri  && !_switchLimitKiri);
  risingTrigGotoKanan       .process(_switchGotoKanan && !_switchLimitKanan);
  fallingTrigGotoKiri       .process(_switchGotoKiri );
  fallingTrigGotoKanan      .process(_switchGotoKanan);
  device2StepperDirection   .process((_switchLimitKanan && _sikatEnabeler)
                                     || (risingTrigGotoKiri.Q  && !_switchLimitKiri)
                                     , (_switchLimitKiri && _sikatEnabeler)
                                     || (risingTrigGotoKanan.Q && !_switchLimitKanan));

  timerOffIsiBilas          .process(risingTrigSikatEnabeler.Q);
  fallingTrigIsiBilas       .process(timerOffIsiBilas.Q);
  device2StepperEnable      .process(fallingTrigIsiBilas.Q
                                     || (risingTrigGotoKiri.Q )
                                     || (risingTrigGotoKanan.Q )
                                     , (jobDone && _sikatEnabeler) || ( !_sikatEnabeler && (
                                      risingTrigLimitKiri.process(_switchLimitKiri)
                                     || risingTrigLimitKanan.process(_switchLimitKanan)
                                     || fallingTrigGotoKiri.Q
                                     || fallingTrigGotoKanan.Q)));
  timerOnJobDone            .process(jobDone);
  timerOffJobDone           .process(risingTrigJobDone.process(timerOnJobDone.Q));
  mtrPenyikat               = (device2StepperEnable.Q1 && _sikatEnabeler) || _switchPenyikat;
  mtrPembilas               = timerOffIsiBilas.Q && _switchBilasIsi;

  setPin(MTR_SIKAT, !mtrPenyikat );
  setPin(MTR_PUMP_BILAS, !mtrPembilas);
  setPin(REL_DONE, !timerOffJobDone.Q);
  setPin(REL_MODE, !_sikatEnabeler);
  setPin(DEVICE2_STP_DIR, !device2StepperDirection.Q1);
  setPin(DEVICE2_STP_EN, !device2StepperEnable.Q1);
  freqGenerator_setOut(FREQGEN_CHANNEL_(DEVICE2_STP_PULSE), device2StepperEnable.Q1);
}
#endif
