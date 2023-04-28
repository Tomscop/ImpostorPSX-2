/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

//thanks to spicyjpeg for helping me understand this code

#include "save.h"

#include <libmcrd.h>
#include "stage.h"
				  
	        //HAS to be BASCUS-scusid,somename
#define savetitle "bu00:BASCUS-61518impostor"
#define savename  "ImpostorPSX"

static const u8 saveIconPalette[32] = 
{
  	0x00, 0x00, 0x00, 0x80, 0x9F, 0xB1, 0x1F, 0x80, 0x1D, 0xFB, 0x08, 0x84,
	0xBF, 0xB0, 0x5B, 0xF2, 0x16, 0x90, 0x66, 0xA4, 0x23, 0x8C, 0xDD, 0xFF,
	0x0E, 0xFF, 0xE7, 0xED, 0xED, 0xFF, 0xCC, 0xCC
};

static const u8 saveIconImage[128] = 
{
 	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x12, 0x00, 0x00, 0x00, 0x30, 0x33, 0x33, 0x41, 0x12, 0x00, 0x00,
	0x00, 0x33, 0x33, 0x53, 0x62, 0x17, 0x00, 0x00, 0x00, 0x33, 0x33, 0x11,
	0x88, 0x16, 0x00, 0x00, 0x00, 0x33, 0x13, 0xA9, 0xAA, 0x18, 0x00, 0x00,
	0x00, 0x33, 0x91, 0x8A, 0xAA, 0x11, 0x00, 0x00, 0x00, 0xB3, 0xCB, 0x3C,
	0xAB, 0xAA, 0x0A, 0x00, 0x00, 0xC3, 0xDD, 0xB3, 0xA2, 0x9A, 0x3A, 0x00,
	0x00, 0xB0, 0xCC, 0x3C, 0xAE, 0x1A, 0x3A, 0x00, 0x00, 0x00, 0xA9, 0x5A,
	0xAA, 0x1A, 0x1A, 0x00, 0x00, 0x00, 0xAF, 0xAA, 0xAA, 0x1A, 0x1A, 0x00,
	0x00, 0x00, 0x9F, 0xAA, 0xAA, 0xAA, 0x1A, 0x00, 0x00, 0x00, 0x9A, 0xAA,
	0xAA, 0x1A, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x03, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void toShiftJIS(u8 *buffer, const char *text)
{
    int pos = 0;
    for (u32 i = 0; i < strlen(text); i++) 
    {
        u8 c = text[i];
        if (c >= '0' && c <= '9') { buffer[pos++] = 0x82; buffer[pos++] = 0x4F + c - '0'; }
        else if (c >= 'A' && c <= 'Z') { buffer[pos++] = 0x82; buffer[pos++] = 0x60 + c - 'A'; }
        else if (c >= 'a' && c <= 'z') { buffer[pos++] = 0x82; buffer[pos++] = 0x81 + c - 'a'; }
        else if (c == '(') { buffer[pos++] = 0x81; buffer[pos++] = 0x69; }
        else if (c == ')') { buffer[pos++] = 0x81; buffer[pos++] = 0x6A; }
        else /* space */ { buffer[pos++] = 0x81; buffer[pos++] = 0x40; }
    }
}

static void initSaveFile(SaveFile *file, const char *name) 
{
	file->id = 0x4353;
 	file->iconDisplayFlag = 0x11;
 	file->iconBlockNum = 1;
  	toShiftJIS(file->title, name);
 	memcpy(file->iconPalette, saveIconPalette, 32);
 	memcpy(file->iconImage, saveIconImage, 128);
}

void defaultSettings()
{
	stage.prefs.ghost = 1;	
	stage.prefs.sfxmiss = 1;	
	stage.prefs.songtimer = 1;
	stage.prefs.flash = 1;
	stage.prefs.defeat = 1;

	for (int i = 0; i < StageId_Max; i++)
	{
		stage.prefs.savescore[i][0] = 0;
		stage.prefs.savescore[i][1] = 0;
		stage.prefs.savescore[i][2] = 0;
	}
}

boolean readSaveFile()
{
	int fd = open(savetitle, 0x0001);
	if (fd < 0) // file doesnt exist 
		return false;

	SaveFile file;
	if (read(fd, (void *) &file, sizeof(SaveFile)) == sizeof(SaveFile)) 
		printf("ok\n");
	else {
		printf("read error\n");
		return false;
	}
	memcpy((void *) &stage.prefs, (const void *) file.saveData, sizeof(stage.prefs));
	close(fd);
	return true;
}

void writeSaveFile()
{	
	int fd = open(savetitle, 0x0002);

	if (fd < 0) // if save doesnt exist make one
		fd =  open(savetitle, 0x0202 | (1 << 16));

	SaveFile file;
	initSaveFile(&file, savename);
  	memcpy((void *) file.saveData, (const void *) &stage.prefs, sizeof(stage.prefs));
	
	if (fd >= 0) {
	  	if (write(fd, (void *) &file, sizeof(SaveFile)) == sizeof(SaveFile)) 
	  		printf("ok\n");
	 	else 
	 		printf("write error\n");  // if save doesnt exist do a error
		close(fd);
	} 
	else 
		printf("open error %d\n", fd);  // failed to save
}
