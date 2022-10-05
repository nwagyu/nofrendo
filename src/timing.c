#include <osd.h>
#include <eadk.h>

int timerfreq = 50;

//Seemingly, this will be called only once. Should call func with a freq of frequency,
int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
	timerfreq = frequency;
	return 0;
}

int osd_nofrendo_ticks(void) {
	return eadk_timing_millis() / (1000 / timerfreq);
}
