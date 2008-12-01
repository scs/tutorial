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

/*!@file bmp.c
 * @brief Bitmap module example.
 * Demonstrates how to read and write bitmap files.
 */

#include "inc/oscar.h"
#include <string.h>

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
	
	/* Picture data structure. */
	struct OSC_PICTURE pic;
	memset(&pic, 0, sizeof(struct OSC_PICTURE));
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Load bitmap module */
	OscBmpCreate(hFramework);
	
	/* Read picture from file */
	OscBmpRead(&pic, "imgCapture.bmp");
	
	/* Process picture */
	/* -------------- */
	
	/* Setup target picture */
	pic.width = OSC_CAM_MAX_IMAGE_WIDTH;
	pic.height = OSC_CAM_MAX_IMAGE_HEIGHT;
	pic.type = OSC_PICTURE_GREYSCALE;
	
	/* Write picture to file */
	OscBmpWrite(&pic, "modified.bmp");
	
	/* Destroy bitmap module */
	OscBmpDestroy(hFramework);
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
