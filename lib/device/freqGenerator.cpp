#ifdef DEVICE_DEBUG 
unsigned long freqGeneratorPeriod;
bool freqGeneratorState;
void freqGenerator_setPeriod(unsigned long period) 
{
	freqGeneratorPeriod = period;
}
void freqGenerator_disable()
{
	freqGeneratorState = false;
}
void freqGenerator_enable() 
{
	freqGeneratorState = true;
}
#else
#include <FrequencyTimer2.h>
void freqGenerator_setPeriod(unsigned long period) 
{
	FrequencyTimer2::setPeriod(period);
}
void freqGenerator_disable()
{
	FrequencyTimer2::disable();
}
void freqGenerator_enable() 
{
	FrequencyTimer2::disable();
}
#endif
