/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cafeteriahd.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//CafeteriaHD background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_box; //box

} Back_CafeteriaHD;


void Back_CafeteriaHD_DrawBG(StageBack *back)
{
	Back_CafeteriaHD *this = (Back_CafeteriaHD*)back;

	fixed_t fx, fy;

	//Draw cafeteriahd
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,214};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(255 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(214,1)
	};
	
	RECT back1_src = {  0,  0,251,214};
	RECT_FIXED back1_dst = {
		FIXED_DEC(254 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(251 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(214,1)
	};
	
	RECT box_src = {  0,  0,158, 77};
	RECT_FIXED box_dst = {
		FIXED_DEC(157 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(96,1) - fy,
		FIXED_DEC(158 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(77,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&box_dst, 7, fx, fy);
	Stage_DrawTex(&this->tex_box, &box_src, &box_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_CafeteriaHD_Free(StageBack *back)
{
	Back_CafeteriaHD *this = (Back_CafeteriaHD*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_CafeteriaHD_New(void)
{
	//Allocate background structure
	Back_CafeteriaHD *this = (Back_CafeteriaHD*)Mem_Alloc(sizeof(Back_CafeteriaHD));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_CafeteriaHD_DrawBG;
	this->back.free = Back_CafeteriaHD_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\CAFEHD.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_box, Archive_Find(arc_back, "box.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
