/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "daveoffice.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//DaveOffice background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2

} Back_DaveOffice;


void Back_DaveOffice_DrawBG(StageBack *back)
{
	Back_DaveOffice *this = (Back_DaveOffice*)back;

	fixed_t fx, fy;

	//Draw daveoffice
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(266 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(266,1)
	};
	
	RECT back1_src = {  0,  0,178,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(265 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(185 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(266,1)
	};
	
	RECT back2_src = {  0,  0, 95, 87};
	RECT_FIXED back2_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(79,1) - fy,
		FIXED_DEC(99 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(91,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 6, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 7, fx, fy);
	Debug_StageMoveDebug(&back2_dst, 8, fx, fy);
	if (stage.song_step >= 2064)
		Stage_DrawTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_DaveOffice_Free(StageBack *back)
{
	Back_DaveOffice *this = (Back_DaveOffice*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_DaveOffice_New(void)
{
	//Allocate background structure
	Back_DaveOffice *this = (Back_DaveOffice*)Mem_Alloc(sizeof(Back_DaveOffice));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_DaveOffice_DrawBG;
	this->back.free = Back_DaveOffice_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\OFFICE.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
