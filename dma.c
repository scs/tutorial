/*!@file dma.c
 * @brief DMA module example.
 * Demonstrates a memory copy operation using dma.
 */

#include "inc/oscar.h"
#include <stdio.h>

#define WIDTH 512
#define HEIGHT 128


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
	
	/* Handle to the DMA chain. */
	void *hChain;
	
	/* Dma source and destination pointer. */
	void *src, *dest;
	
	/* Source data field. */
	uint32 source[WIDTH][HEIGHT];
	
	/* Destination data field. */
	uint32 drain[WIDTH][HEIGHT];
	uint32 i,j;
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Allocate dma chain */
	OscDmaAllocChain(&hChain);
	
	/* Load dma module */
	OscDmaCreate(hFramework);
	
	/* Compute something and fill source data segment with data */
	/* -------------------------------------------------------- */
	
	/* Add a 2D dma move to the chain (max. 4 moves per chain) */
	src = (void*)&source;
	dest = (void*)&drain;
	
	OscDmaAdd2DMove(hChain, dest, DMA_WDSIZE_32, WIDTH, sizeof(uint32), HEIGHT, sizeof(uint32), src, DMA_WDSIZE_32, WIDTH, sizeof(uint32), HEIGHT, sizeof(uint32));
	OscDmaAddSyncPoint(hChain);
	
	/* Start dma transfer */
	OscDmaStart(hChain);
	
	/* DMA transfer in progress... compute something else. */
	/* --------------------------------------------------- */
	
	/* Sync with dma transfer */
	OscDmaSync(hChain);
	
	/* Compare source and drain.. print if difference detected */
	for (i=0; i<HEIGHT; i++){
		for ( j=0; j<WIDTH; j++){
			if (source[j][i] != drain[j][i]){
				printf("x=%lu y=%lu data=%lu final%lu\n",j,i,source[j][i],drain[j][i]);
			}
		}
	}
	printf("Dma transfer done!!!\n");
	
	/* Unload dma module */
	OscDmaDestroy(hFramework);
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
