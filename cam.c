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

/*!@file cam.c
 * @brief Camera module example.
 * Demonstrates how to capture pictures using multiple buffers.
 */

#include "oscar/staging/inc/oscar.h"
#include <stdio.h>

/*********************************************************************//*!
 * @brief Program entry.
 * 
 * @param argc Command line argument count.
 * @param argv Command line argument string.
 * @return 0 on success
 *//*********************************************************************/
int main(const int argc, const char * argv[])
{
	/* Handle to framework instance. */
	void *hFramework;
	
#if defined(OSC_HOST) || defined(OSC_SIM)
	/* Handle to file name reader, if compiled for host platform. */
	void *hFileNameReader;
#endif
	
	/* Camera parameters */
	uint32 shutterWidth;
	uint16 x, y, width, height;
	uint16 blackOffset;
	uint8 bufferIDs[2];
	
	/* Frame buffers. */
	uint8 frameBuffer0[OSC_CAM_MAX_IMAGE_WIDTH * OSC_CAM_MAX_IMAGE_HEIGHT];
	uint8 frameBuffer1[OSC_CAM_MAX_IMAGE_WIDTH * OSC_CAM_MAX_IMAGE_HEIGHT];
	
	/* Pointer to captured picture */
	void *pic;
	
	struct OSC_PICTURE p;
	
	OSC_ERR err = SUCCESS;
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Load camera module */
	OscCamCreate(hFramework);

	/* Load GPIO module */
	OscGpioCreate(hFramework);

	p.width = OSC_CAM_MAX_IMAGE_WIDTH;
	p.height = OSC_CAM_MAX_IMAGE_HEIGHT;
	p.type = OSC_PICTURE_GREYSCALE;
	
	/* Get camera settings */
	OscCamGetShutterWidth(&shutterWidth);
	OscCamGetAreaOfInterest(&x, &y, &width, &height);
	OscCamGetBlackLevelOffset(&blackOffset);
	printf("ShutterWidth=%lu\n",shutterWidth);
	printf("AreaOfInterest: x=%u y=%u width=%u height=%u\n", x, y, width, height);
	printf("BlackLevelOffset=%u\n",blackOffset);
	
	/* Process settings */
	/* ---------------- */
	
	/* Set new camera settings */
	shutterWidth = 50000; /* set shutter to 50ms, 0 => automatic */
	OscCamSetShutterWidth(shutterWidth);
	OscCamSetAreaOfInterest(0,0,OSC_CAM_MAX_IMAGE_WIDTH, OSC_CAM_MAX_IMAGE_HEIGHT);
	OscCamSetBlackLevelOffset(blackOffset);
	OscCamSetupPerspective(OSC_CAM_PERSPECTIVE_VERTICAL_MIRROR);
	
#if defined(OSC_HOST) || defined(OSC_SIM)
	/* Setup file name reader if compiled for host platform */
	OscFrdCreateConstantReader(&hFileNameReader, "imgCapture.bmp");
	OscCamSetFileNameReader(hFileNameReader);
#endif
	
	/* Setup framebuffers - double buffering */
	OscCamSetFrameBuffer(0, OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT, frameBuffer0, TRUE);
	OscCamSetFrameBuffer(1, OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT, frameBuffer1, TRUE);
	bufferIDs[0] = 0;
	bufferIDs[1] = 1;
	OscCamCreateMultiBuffer(2, bufferIDs);
	
	/* Setup capture to first frame buffer */
	err = OscCamSetupCapture(OSC_CAM_MULTI_BUFFER);
	if(err != SUCCESS){
		printf("%s: Unable to setup initial capture (%d)!\n", __func__, err);
		return err;
	}

	/* Trigger image capturing */
	err = OscGpioTriggerImage();
	if(err != SUCCESS){
		printf("%s: Unable to trigger capture (%d)!\n", __func__, err);
		return err;
	}
	
	/* Do something ... */
	/* ---------------- */
	
	/* Read Picture */
	err = OscCamReadPicture(OSC_CAM_MULTI_BUFFER, (void *) &pic, 0, 0);
	if(err != SUCCESS){
		printf("%s: Unable to read picture (%d)!\n", __func__, err);
		return err;
	}
	
	/* Process picture */
	/* --------------- */
	
	/* Capture picture to second frame buffer */
	err = OscCamSetupCapture(OSC_CAM_MULTI_BUFFER);
	if(err != SUCCESS){
		printf("%s: Unable to setup initial capture (%d)!\n", __func__, err);
		return err;
	}
	
	/* Do something ... */
	/* ---------------- */
	
	err = OscCamReadPicture(OSC_CAM_MULTI_BUFFER, (void *) &pic, 0, 0);
	if(err != SUCCESS){
		printf("%s: Unable to read picture (%d)!\n", __func__, err);
		return err;
	}
	
	/* Process picture */
	/* --------------- */
	
	/* Unload camera module */
	OscCamDestroy(hFramework);

	/* Unload GPIO module */
	OscGpioDestroy(hFramework);
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
