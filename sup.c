/*!@file sup.c
 * @brief Support module example.
 * Demonstrates how the watchdog timer and the cycle count functions can be used.
 */

#include "inc/oscar.h"
#include <stdio.h>
#include <unistd.h>

/*********************************************************************//*!
 * @brief Program entry.
 * 
 * @param argc Command line argument count.
 * @param argv Command line argument string.
 * @return 0 on success
 *//*********************************************************************/
int main(const int argc, const char * argv[])
{
	void *hFramework;
	float time;
	uint32 cyclesStart, cyclesEnd;
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Load support module */
	OscSupCreate(hFramework);
	
	/* Initialize watchdog (default timeout 1 minute) */
	OscSupWdtInit();
	
	/* Save current cycle count */
	cyclesStart = OscSupCycGet();
	
	/* Do something... */
	/* --------------- */
	/* We just wait until one or less seconds before reboot
	 * and print the remaining time. The cycle count overflows far below one
	 * minute and is therefore not usefull to measure large time differences
	 * in one step. */
	time = 60.0;
	do {
		sleep(1);
		cyclesEnd = OscSupCycGet();
		time -= ((float)OscSupCycToMicroSecs(cyclesEnd - cyclesStart) / 1000000);
		printf("System reboot in: %f seconds\n",time);
		cyclesStart = cyclesEnd;
	} while (time > 1);
	
	/* Keep the system alive */
	OscSupWdtKeepAlive();
	
	/* Save again the current cycle count */
	cyclesStart = OscSupCycGet();
	printf("Reboot avoided!\n");
	
	/* Do something... again */
	/* --------------------- */
	/* We just wait again and print the remaining time until reboot. */
	sleep(1);
	cyclesEnd = OscSupCycGet();
	time = 60 - ((float)OscSupCycToMicroSecs(cyclesEnd - cyclesStart) / 1000000);
	printf("System reboot in: %f seconds\n",time);
	
	/* Disable Watchdog */
	OscSupWdtClose();
	printf("Watchdog disabled!\n");
	
	/* Destroy support module */
	OscSupDestroy(hFramework);
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
