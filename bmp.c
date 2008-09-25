/*!@file bmp.c
 * @brief Bitmap module example.
 * Demonstrates how to read and write bitmap files.
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


