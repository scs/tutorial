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

/*!@file cfg.c
 * @brief Config module example.
 * Demonstrates how to read/write settings from/to config files.
 * See config.txt for file structure details.
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
	void *hFramework;
	struct CFG_KEY key;
	struct CFG_VAL_STR val;
	int16 number1;
	int32 number2;
	
	char strNumber[10];
	
	/* Handle to the config file */
	CFG_FILE_CONTENT_HANDLE hCfg;
	const char fileName[] = "config.txt";
	
	/* Create framework */
	OscCreate(&hFramework);
	
	/* Initialize config module */
	OscCfgCreate(hFramework);
	
	/* Register config file */
	OscCfgRegisterFile(&hCfg, fileName, 1024);
	
	/* Set config file section (global if NULL) */
	key.strSection = NULL;
	
	/* Read value of tag NAME from global section */
	key.strTag = "NAME";
	OscCfgGetStr(hCfg, &key, &val);
	printf("NAME: %s\n",(char*)&val);
	
	/* Read 16 bit integer */
	key.strTag = "16BIT";
	OscCfgGetInt(hCfg, &key, &number1);
	printf("16BIT: %d\n",number1);
	
	/* Read 32 bit integer */
	key.strTag = "32BIT";
	OscCfgGetInt32(hCfg, &key, &number2);
	printf("32Bit: %ld\n",number2);
	
	/* Write new section, tag and value */
	key.strSection = "NEWSECTION";
	key.strTag = "NEWTAG";
	OscCfgSetStr(hCfg, &key, "newvalue");
	
	/* Write new 32 bit integer */
	key.strTag = "NEW32BIT";
	sprintf(strNumber,"%ld",-number2);
	OscCfgSetStr(hCfg, &key, strNumber);
	
	/* Write changes to disk */
	OscCfgFlushContent(hCfg);
	
	/* Destroy support module */
	OscCfgDestroy(hFramework);
	
	/* Destroy framework */
	OscDestroy(hFramework);
	
	return 0;
}
