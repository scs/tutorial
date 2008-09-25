/*!@file alarm.c
 * @brief Simple alarm application.
 * Calculates the mean of a captured picture and compares it with the mean
 * of the last pictures. If the difference exceeds a threshold, the alarm is
 * activated (the application ends). */

#include "inc/oscar.h"
#include <stdio.h>

#define HISTORY_LENGTH 10
#define IMAGE_WIDTH 752
#define IMAGE_HEIGHT 480
#define THRESHOLD 3

/*! @brief Framework module dependencies. */
struct OSC_DEPENDENCY deps[] = {
        {"sup", OscSupCreate, OscSupDestroy},
        {"bmp", OscBmpCreate, OscBmpDestroy},
       	{"cam", OscCamCreate, OscCamDestroy},
};

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
  uint32 m, n = 0, i, cycles, us, bufferIndex = 0, alarm = 0;
  uint32 meanBuffer[HISTORY_LENGTH];
  uint8 *tempPic;

  /* Create framework */
  err = OscCreate(&hFramework);
  if(err != SUCCESS){
	fprintf(stderr, "%s: Unable to create framework.\n",
			__func__);
	return err;
  }

  /* Load the framework module dependencies. */
  err = OscLoadDependencies(hFramework, deps, 
							sizeof(deps)/sizeof(struct OSC_DEPENDENCY));
  
  if(err != SUCCESS){
	fprintf(stderr, "%s: ERROR: Unable to load dependencies! (%d)\n",
			__func__, err);
	return err;
  }

  /* Setup target picture */
  pic.width = IMAGE_WIDTH;
  pic.height = IMAGE_HEIGHT;
  pic.type = OSC_PICTURE_GREYSCALE;

  /* Configure camera */
  OscCamPresetRegs();
  OscCamSetAreaOfInterest(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
  OscCamSetFrameBuffer(0, IMAGE_WIDTH * IMAGE_HEIGHT, frameBuffer, TRUE);
  OscCamSetShutterWidth(50000); /* 50 ms shutter */
 
  /* Fill mean buffer */
  for(i = 0; i < HISTORY_LENGTH; i++){
	OscCamSetupCapture(0);
	OscCamReadPicture(0, &pic.data, 0, 0);
	meanBuffer[i] = mean(&pic);
  }

  /* Start alarm mode */
  while(alarm == 0){
	cycles = OscSupCycGet();

	/* Take a picture, tr until successfull */
	err = -1;
	while(err != SUCCESS){
	  OscCamSetupCapture(0);
	  err = OscCamReadPicture(0, &pic.data, 0, 200);
	}

	/* Calculate mean of new picture */
	m = mean(&pic);

	/* Calculate mean of the last pictures */
	for(i = 0; i < HISTORY_LENGTH; i++){
	  n += meanBuffer[i];
	}
	n = n / HISTORY_LENGTH;

	/* Check if in range */
	if(m > (n + THRESHOLD) || m <= (n - THRESHOLD)){
	  alarm = 1;
	  
	  /* Tag the picture (draw horizontal lines) */
	  tempPic = (uint8*)pic.data;
	  for(i = 0; i < IMAGE_WIDTH; i++){
		tempPic[40*IMAGE_WIDTH + i] = 0;
		tempPic[41*IMAGE_WIDTH + i] = 255;
		tempPic[439*IMAGE_WIDTH + i] = 255;
		tempPic[440*IMAGE_WIDTH + i] = 0;
	  }
	}


	/* Add new mean to meanBuffer */
	meanBuffer[bufferIndex++] = m;

	/* Update buffer index */
	bufferIndex = bufferIndex % HISTORY_LENGTH;

	/* Calculate processing time */
	cycles = OscSupCycGet() - cycles;
	us = OscSupCycToMicroSecs(cycles);

	printf("Time=%lu  Picture=%lu  Last Pictures=%lu\n",us,m,n);

	n = 0;

  }

  /* Write picture to file */
  OscBmpWrite(&pic, "alarm.bmp");

  /* Destroy modules */
  OscUnloadDependencies(hFramework, deps, 
						sizeof(deps)/sizeof(struct OSC_DEPENDENCY));

  /* Destroy framework */
  OscDestroy(hFramework);

  return 0;

}


