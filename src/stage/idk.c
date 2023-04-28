/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "idk.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Idk background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1

} Back_Idk;

void Back_Idk_DrawBG(StageBack *back)
{
	Back_Idk *this = (Back_Idk*)back;

	fixed_t fx, fy;

	//Draw idkbg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT vs_src = {  0,  0, 96, 47};
	RECT_FIXED vs_dst = {
		FIXED_DEC(363 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(136,1) - fy,
		FIXED_DEC(96 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(47,1)
	};
	
	RECT tobyfox_src = {  0, 48,164, 75};
	RECT_FIXED tobyfox_dst = {
		FIXED_DEC(235 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(225,1) - fy,
		FIXED_DEC(164 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(75,1)
	};
	
	RECT thekid_src = {  0,124,155, 64};
	RECT_FIXED thekid_dst = {
		FIXED_DEC(596 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(169,1) - fy,
		FIXED_DEC(155 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(64,1)
	};
	
	RECT memory_src = {  0,189,177, 34};
	RECT_FIXED memory_dst = {
		FIXED_DEC(226 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(420,1) - fy,
		FIXED_DEC(177 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(34,1)
	};
	
	RECT back1_src = {  0,  0,255, 50};
	RECT_FIXED back1_dst = {
		FIXED_DEC(-540 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-64,1) - fy,
		FIXED_DEC(583 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(114,1)
	};
	
	if (stage.stage_id == StageId_Idk)
	{
		Debug_StageMoveDebug(&vs_dst, 5, fx, fy);
		Stage_DrawTex(&this->tex_back0, &vs_src, &vs_dst, stage.camera.bzoom);
		Debug_StageMoveDebug(&tobyfox_dst, 6, fx, fy);
		Stage_DrawTex(&this->tex_back0, &tobyfox_src, &tobyfox_dst, stage.camera.bzoom);
		Debug_StageMoveDebug(&thekid_dst, 7, fx, fy);
		Stage_DrawTex(&this->tex_back0, &thekid_src, &thekid_dst, stage.camera.bzoom);
		Debug_StageMoveDebug(&memory_dst, 8, fx, fy);
		Stage_DrawTex(&this->tex_back0, &memory_src, &memory_dst, stage.camera.bzoom);
	}
	if (stage.stage_id == StageId_Ow)
	{
		Debug_StageMoveDebug(&back1_dst, 5, fx, fy);
		Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	}
	
	//Draw white
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 255, 255, 255);
}

void Back_Idk_Free(StageBack *back)
{
	Back_Idk *this = (Back_Idk*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Idk_New(void)
{
	//Allocate background structure
	Back_Idk *this = (Back_Idk*)Mem_Alloc(sizeof(Back_Idk));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Idk_DrawBG;
	this->back.free = Back_Idk_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\IDK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);

	Gfx_SetClear(0, 0, 0);
	
	return (StageBack*)this;
}
