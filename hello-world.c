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

/*!@file hello-world.c
 * @brief Simple hello-world application.
 * Initialize Framework, take a picture, modify and save it to a file.
 */

#include "oscar/staging/inc/oscar.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if defined(OSC_HOST)
#define HTTP_ROOT "/var/www"
#else
#define HTTP_ROOT "/home/httpd/"
#endif

/*********************************************************************//*!
 * @brief Program entry.
 * 
 * @param argc Command line argument count.
 * @param argv Command line argument string.
 * @return 0 on success
 *//*********************************************************************/
int main(const int argc, const char * argv[])
{
	/*! @brief Handle to framework instance. */
	void *hFramework;
	
#if defined(OSC_HOST) || defined(OSC_SIM)
	/*! @brief Handle to file name reader for camera images on the host. */
	void *hFileNameReader;
#endif

	static uint8 frameBuffer[OSC_CAM_MAX_IMAGE_WIDTH * OSC_CAM_MAX_IMAGE_HEIGHT];
	static uint8 colorPic[3 * OSC_CAM_MAX_IMAGE_WIDTH * OSC_CAM_MAX_IMAGE_HEIGHT];
	
	uint16 i;
	uint8 * rawPic = NULL;
	struct OSC_PICTURE pic;
	enum EnBayerOrder enBayerOrder;
	
	int32 opt_shutterWidth = 50000;
	bool opt_debayer = false;
	
	for (i = 1; i < argc; i += 1)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			opt_debayer = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			i += 1;
			if (i >= argc)
			{
				printf("Error: -s needs an argument.\n");
				return 1;
			}
			opt_shutterWidth = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			printf("Usage: hello-world [ -h ] [ -d ] [ -s <shutter-width> ]\n");
			printf("    -h: Prints this help.");
			printf("    -d: Debayers the image.");
			printf("    -s <shutter-width>: Sets the shutter with in us.");
		}
		else
		{
			printf("Error: Unknown option: %s", argv[i]);
			return 1;
		}
	}
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Load modules */
	OscBmpCreate(hFramework);
	OscCamCreate(hFramework);
	OscVisCreate(hFramework);
	OscGpioCreate(hFramework);
	
#if defined(OSC_HOST) || defined(OSC_SIM)
	/* Setup file name reader (for host compiled version); read constant image */
	OscFrdCreateConstantReader(&hFileNameReader, "imgCapture.bmp");
	OscCamSetFileNameReader(hFileNameReader);
#endif
	
	/* Configure camera */
	OscCamSetShutterWidth(opt_shutterWidth);
	OscCamSetAreaOfInterest(0, 0, OSC_CAM_MAX_IMAGE_WIDTH, OSC_CAM_MAX_IMAGE_HEIGHT);
	OscCamSetFrameBuffer(0, OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT, frameBuffer, TRUE);
	
	/* Take a picture */
	OscCamSetupCapture(0);
#if defined(OSC_TARGET)
	/* This hacks around the camera chip activating settings only after the next image taken. */
	OscGpioTriggerImage();
	OscCamReadPicture(0, (void *) &rawPic, 0, 0);
	OscCamSetShutterWidth(48);
	
	OscCamSetupCapture(0);
	OscGpioTriggerImage();
#endif
	OscCamReadPicture(0, (void *) &rawPic, 0, 0);
	
	/* Debayer (transform to colored picture) */
	/* OscCamGetBayerOrder(&enBayerOrder, 0, 0); */
	/* OscVisDebayer((uint8*)&rawPic, OSC_CAM_MAX_IMAGE_WIDTH, OSC_CAM_MAX_IMAGE_HEIGHT, enBayerOrder, (uint8*)&colorPic);*/
	
	/* Write picture to file */
	pic.width = OSC_CAM_MAX_IMAGE_WIDTH;
	pic.height = OSC_CAM_MAX_IMAGE_HEIGHT;
	
	if (opt_debayer)
	{
		OscCamGetBayerOrder(&enBayerOrder, 0, 0);
		OscVisDebayer(frameBuffer, OSC_CAM_MAX_IMAGE_WIDTH, OSC_CAM_MAX_IMAGE_HEIGHT, enBayerOrder, colorPic);
		pic.type = OSC_PICTURE_BGR_24;
		pic.data = colorPic;
	}
	else
	{
		pic.type = OSC_PICTURE_GREYSCALE;
		pic.data = rawPic;
	}

	OscBmpWrite(&pic, HTTP_ROOT "hello-world.bmp~");
	rename(HTTP_ROOT "hello-world.bmp~", HTTP_ROOT "hello-world.bmp");
	
	/* Destroy modules */
	OscBmpDestroy(hFramework);
	OscCamDestroy(hFramework);
	OscVisDestroy(hFramework);
	OscGpioDestroy(hFramework);
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
