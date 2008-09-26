/*!@file hello-world.c
 * @brief Simple hello-world application.
 * Initialize Framework, take a picture, modify and save it to a file.
 */

#include "inc/oscar.h"

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
	
#if defined(OSC_HOST)
	/*! @brief Handle to file name reader for camera images on the host. */
	void *hFileNameReader;
#endif
	
	uint8 frameBuffer[OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT];
	uint8 rawPic[OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT];
	uint8 colorPic[3*OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT];
	struct OSC_PICTURE pic;
	enum EnBayerOrder enBayerOrder;
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Load modules */
	OscBmpCreate(hFramework);
	OscCamCreate(hFramework);
	OscVisCreate(hFramework);
#if defined(OSC_HOST)
	OscFrdCreate(hFramework);
#endif
	
#if defined(OSC_HOST)
	/* Setup file name reader (for host compiled version); read constant image */
	OscFrdCreateConstantReader(&hFileNameReader, "imgCapture.bmp");
	OscCamSetFileNameReader(hFileNameReader);
#endif
	
	/* Configure camera */
	OscCamPresetRegs();
	OscCamSetAreaOfInterest(0,0,OSC_CAM_MAX_IMAGE_WIDTH, OSC_CAM_MAX_IMAGE_HEIGHT);
	OscCamSetFrameBuffer(0, OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT, frameBuffer, TRUE);
	
	/* Take a picture */
	OscCamSetupCapture(0);
	OscCamReadPicture(0, (void*)&rawPic, 0, 0);
	
	/* Debayer (transform to colored picture) */
	OscCamGetBayerOrder(&enBayerOrder, 0, 0);
	OscVisDebayer(&rawPic, OSC_CAM_MAX_IMAGE_WIDTH, OSC_CAM_MAX_IMAGE_HEIGHT, enBayerOrder, &colorPic);
	
	/* Write picture to file */
	pic.width = OSC_CAM_MAX_IMAGE_WIDTH;
	pic.height = OSC_CAM_MAX_IMAGE_HEIGHT;
	pic.type = OSC_PICTURE_BGR_24;
	pic.data = (void*)&colorPic;
	OscBmpWrite(&pic, "hello-world.bmp");
	
	/* Destroy modules */
	OscBmpDestroy(hFramework);
	OscCamDestroy(hFramework);
	OscVisDestroy(hFramework);
#if defined(OSC_HOST)
	OscFrdDestroy(hFramework);
#endif
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
