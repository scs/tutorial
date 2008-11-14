/*	A collection of example applications for the LeanXcam platform.
	Copyright (C) 2008 Supercomputing Systems AG
	
	This library is free software; you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation; either version 2.1 of the License, or (at
	your option) any later version.
	
	This library is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
	General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with this library; if not, write to the Free Software Foundation,
	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*!@file alarm.c
 * @brief Simple alarm application.
 * Calculates the mean of a captured picture and compares it with the mean
 * of the last pictures. If the difference exceeds a threshold, the alarm is
 * activated (the application ends). */

#include "inc/oscar.h"
#include <stdio.h>
#include <unistd.h>

#define HISTORY_LENGTH 20
#define IMAGE_WIDTH 752
#define IMAGE_HEIGHT 480
#define THRESHOLD 2

/*! @brief Framework module dependencies. */
struct OSC_DEPENDENCY deps[] = {
	{ "sup", OscSupCreate, OscSupDestroy },
	{ "bmp", OscBmpCreate, OscBmpDestroy },
	{ "cam", OscCamCreate, OscCamDestroy },
	{ "gpio", OscGpioCreate, OscGpioDestroy },
};

/*! @brief Global variables. */
int led = 0;

/*********************************************************************//*!
 * @brief Calculate mean of picture.
 * 
 * @param pic OSC_PICTURE
 * @return mean of pic
 *//*********************************************************************/
int mean(struct OSC_PICTURE *pic)
{
	uint16 i,j;
	uint32 sum = 0;
	uint8 *p;
	p = (uint8*)pic->data;
	for(i = 0; i < pic->height; i++){
		for(j = 0; j < pic->width; j++){
			sum += p[i*pic->width + j];
		}
	}
	sum = sum / (pic->width * pic->height);
	return sum;
}

/*********************************************************************//*!
 * @brief Toggle survailance indicator LED.
 * 
 *//*********************************************************************/
void toggle()
{
    OSC_ERR err = SUCCESS;
    if(led == 1){
	  err = OscGpioWrite(GPIO_OUT1, FALSE);
	  led = 0;
    }else{
	  err = OscGpioWrite(GPIO_OUT1, TRUE);
	  led = 1;
    }
	if (err != SUCCESS) {
	  fprintf(stderr, "%s: ERROR: GPIO write error! (%d)\n", __func__, err);
	}

	return;
}

/*********************************************************************//*!
 * @brief Program entry.
 * 
 * @param argc Command line argument count.
 * @param argv Command line argument strings.
 * @return 0 on success
 *//*********************************************************************/
int main(const int argc, const char* argv[])
{
	OSC_ERR err = SUCCESS;
	void* hFramework;
	uint8 frameBuffer[IMAGE_WIDTH * IMAGE_HEIGHT];
	struct OSC_PICTURE pic;
	uint32 m, n = 0, i, bufferIndex = 0;
	uint32 meanBuffer[HISTORY_LENGTH];
	

	/* Wait some time */
	sleep(5);

	/* Create framework */
	err = OscCreate(&hFramework);
	if (err != SUCCESS) {
		fprintf(stderr, "%s: Unable to create framework.\n", __func__);
		return err;
	}
	
	/* Load the framework module dependencies. */
	err = OscLoadDependencies(hFramework, deps, sizeof(deps)/sizeof(struct OSC_DEPENDENCY));
	if (err != SUCCESS) {
		fprintf(stderr, "%s: ERROR: Unable to load dependencies! (%d)\n", __func__, err);
		return err;
	}

	/* Configure GPIO's (LED's) outputs active high */
 	err = OscGpioSetupPolarity(GPIO_OUT1, FALSE);
 	if (err != SUCCESS) {
	  fprintf(stderr, "%s: ERROR: Unable to set GPIO! (%d)\n", __func__, err);
	  return err;
	}
	err = OscGpioSetupPolarity(GPIO_OUT2, FALSE);
	if (err != SUCCESS) {
	  fprintf(stderr, "%s: ERROR: Unable to set GPIO! (%d)\n", __func__, err);
	  return err;
	}
	OscGpioWrite(GPIO_OUT1, FALSE);
	OscGpioWrite(GPIO_OUT2, FALSE);

	/* Setup target picture */
	pic.width = IMAGE_WIDTH;
	pic.height = IMAGE_HEIGHT;
	pic.type = OSC_PICTURE_GREYSCALE;

	/* Configure camera */
	OscCamPresetRegs();
	OscCamSetAreaOfInterest(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
	OscCamSetFrameBuffer(0, IMAGE_WIDTH * IMAGE_HEIGHT, frameBuffer, TRUE);
	OscCamSetShutterWidth(50000); /* 50 ms shutter */

	/* Initialize mean buffer */
	for (i = 0; i < HISTORY_LENGTH; i++) {
		err = OscCamSetupCapture(0);
		if (err != SUCCESS) {
		  fprintf(stderr, "%s: ERROR: Unable setup capture! (%d)\n", __func__, err);
		  return err;
		}
		err = OscGpioTriggerImage();
		if (err != SUCCESS) {
		  fprintf(stderr, "%s: ERROR: Unable to trigger! (%d)\n", __func__, err);
		  return err;
		}
		err = OscCamReadPicture(0, &pic.data, 0, 0);
		if (err != SUCCESS) {
		  fprintf(stderr, "%s: ERROR: Unable read picture! (%d)\n", __func__, err);
		  return err;
		}

		meanBuffer[i] = mean(&pic);
	}

	/* Start alarm mode */
	while (1) {

	    /* Indicate active surveillance */
	    toggle();

		/* Take a new picture */
		err = OscCamSetupCapture(0);
		if (err != SUCCESS) {
		  fprintf(stderr, "%s: ERROR: Unable setup capture! (%d)\n", __func__, err);
		  return err;
		}
		err = OscGpioTriggerImage();
		if (err != SUCCESS) {
		  fprintf(stderr, "%s: ERROR: Unable to trigger! (%d)\n", __func__, err);
		  return err;
		}
		err = OscCamReadPicture(0, &pic.data, 0, 0);
		if (err != SUCCESS) {
		  fprintf(stderr, "%s: ERROR: Unable read picture! (%d)\n", __func__, err);
		  return err;
		}
		
		/* Calculate mean of new picture */
		m = mean(&pic);
		
		/* Calculate mean of history */
		for (i = 0; i < HISTORY_LENGTH; i++) {
			n += meanBuffer[i];
		}
		n = n / HISTORY_LENGTH;
		
		/* Check if in range and therefore detect intruder */
		if (m > (n + THRESHOLD) || m <= (n - THRESHOLD)) {

			/* Indicate detected intruder with LED */
			err = OscGpioWrite(GPIO_OUT2, TRUE);
			if (err != SUCCESS) {
			  fprintf(stderr, "%s: ERROR: GPIO write error! (%d)\n", __func__, err);
			  return err;
			}
			err = OscGpioWrite(GPIO_OUT1, FALSE);
			if (err != SUCCESS) {
			  fprintf(stderr, "%s: ERROR: GPIO write error! (%d)\n", __func__, err);
			  return err;
			}
			led = 0;

			/* Write a picture of the intruder to a file */
			OscBmpWrite(&pic, "../intruder.bmp");

			/* Take further actions (we just wait for some time and restart) */
			/* ------------------------------------------------------------- */
			sleep(2);

			/* Signal alarm end with LED */
			err = OscGpioWrite(GPIO_OUT2, FALSE);
			if (err != SUCCESS) {
			  fprintf(stderr, "%s: ERROR: GPIO write error! (%d)\n", __func__, err);
			  return err;
			}

			/* Re-Initialize mean buffer */
			for (i = 0; i < HISTORY_LENGTH; i++) {
			  err = OscCamSetupCapture(0);
			  if (err != SUCCESS) {
				fprintf(stderr, "%s: ERROR: Unable setup capture! (%d)\n", __func__, err);
				return err;
			  }
			  err = OscGpioTriggerImage();
			  if (err != SUCCESS) {
				fprintf(stderr, "%s: ERROR: Unable to trigger! (%d)\n", __func__, err);
				return err;
			  }
			  err = OscCamReadPicture(0, &pic.data, 0, 0);
			  if (err != SUCCESS) {
				fprintf(stderr, "%s: ERROR: Unable read picture! (%d)\n", __func__, err);
				return err;
			  }
			  
			  meanBuffer[i] = mean(&pic);
			}

			/* Reset buffer index */
			bufferIndex = 0;
		}else{
		    /* Add new mean to meanBuffer */
		    meanBuffer[bufferIndex++] = m;
		  
			/* Update buffer index */
			bufferIndex = bufferIndex % HISTORY_LENGTH;
		}

		/* Reset mean history */
		n = 0;
	}
	

	/* Destroy modules */
	OscUnloadDependencies(hFramework, deps, sizeof(deps)/sizeof(struct OSC_DEPENDENCY));
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
