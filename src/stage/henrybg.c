/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henryBG.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Henry stage background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Stage part 1
	Gfx_Tex tex_back1; //Stage part 2
	Gfx_Tex tex_back2; //Stage part 3

} Back_Henry;

void Back_Henry_DrawBG(StageBack *back)
{
	Back_Henry *this = (Back_Henry*)back;

	fixed_t fx, fy;

	//Draw Stage
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT stage_src = {  0,  0, 200, 255};
	RECT_FIXED stage_dst = {
		FIXED_DEC(-340 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-103,1) - fy,
		FIXED_DEC(200 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(255,1)
	};

	Debug_StageMoveDebug(&stage_dst, 5, fx, fy);
	Stage_DrawTex(&this->tex_back0, &stage_src, &stage_dst, stage.camera.bzoom);

	stage_dst.x += stage_dst.w; 
	Stage_DrawTex(&this->tex_back1, &stage_src, &stage_dst, stage.camera.bzoom);

	stage_dst.x += stage_dst.w; 
	Stage_DrawTex(&this->tex_back2, &stage_src, &stage_dst, stage.camera.bzoom);
	
}

void Back_Henry_Free(StageBack *back)
{
	Back_Henry *this = (Back_Henry*)back;

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Henry_New(void)
{
	//Allocate background structure
	Back_Henry *this = (Back_Henry*)Mem_Alloc(sizeof(Back_Henry));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Henry_DrawBG;
	this->back.free = Back_Henry_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\HENRY.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);

	Gfx_SetClear(0, 0, 0);
	
	return (StageBack*)this;
}
