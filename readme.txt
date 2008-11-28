############
# Tutorial #
############

The following tutorial files should serve as a fast and
easy application programming entry point. Most of them
are dedicated to a specific oscar framework module.


hello-world.c
-------------------------------------------------------
Configure the framework, take a picture and save it to
a file (hello-world.bmp).


alarm.c
-------------------------------------------------------
This application transforms the leanXcam into a simple
alarm system. If something moves in front of the camera
an alarm is raised (a GPIO pin is aktivated).


bmp.c
-------------------------------------------------------
Read and write bitmap files.


cam.c
-------------------------------------------------------
Camera configuration and multi buffer capturing.


cfg.c
-------------------------------------------------------
Read and write configuration files.


dma.c
-------------------------------------------------------
Configure and initiate dma transfers.


sup.c
-------------------------------------------------------
Watchdog and cycle count demonstration.


-------------------------------------------------------
Compile and run!
-------------------------------------------------------
1. Configure the oscar framework path:
   -> make get

2. Compile everything:
   -> make

2. Compile a specific example for the target platform:
   -> make <sample name>_target

   <sample name> is the file name without its type identifier.

2. Compile a specific example for the host platform:
   -> make <sample name>_host

   The executables and other required files are
   automatically copied to /tftpboot.

3. Download them to the device and start playing.

