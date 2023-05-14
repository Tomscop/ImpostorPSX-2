/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "towers.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Towers background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_back3; //back3
	Gfx_Tex tex_back4; //back4
	Gfx_Tex tex_back5; //back5
	Gfx_Tex tex_back6; //back6
	Gfx_Tex tex_back7; //back7

} Back_Towers;


void Back_Towers_DrawBG(StageBack *back)
{
	Back_Towers *this = (Back_Towers*)back;

	fixed_t fx, fy;

	//Draw towers
	fx = (stage.camera.x * 10) / 100;
	fy = (stage.camera.y * 10) / 100;
	
	RECT back0_src = {  0,  0,255,165};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(324,1)
	};
	
	RECT back1_src = {  0,  0,255,165};
	RECT_FIXED back1_dst = {
		FIXED_DEC(0+499 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(324,1)
	};
	
	fx = (stage.camera.x * 4) / 10;
	fy = (stage.camera.y * 4) / 10;
	
	RECT back2_src = {  0,  0,255,160};
	RECT_FIXED back2_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(314,1)
	};
	
	RECT back3_src = {  0,  0,255,160};
	RECT_FIXED back3_dst = {
		FIXED_DEC(0+499 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(314,1)
	};
	
	RECT back4_src = {  0,  0,255,188};
	RECT_FIXED back4_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(369,1)
	};
	
	RECT back5_src = {  0,  0,255,188};
	RECT_FIXED back5_dst = {
		FIXED_DEC(0+499 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(369,1)
	};
	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back6_src = {  0,  0,255,122};
	RECT_FIXED back6_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(240,1)
	};
	
	RECT back7_src = {  0,  0,255,122};
	RECT_FIXED back7_dst = {
		FIXED_DEC(0+499 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(501 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(240,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Debug_StageMoveDebug(&back2_dst, 7, fx, fy);
	Debug_StageMoveDebug(&back3_dst, 8, fx, fy);
	Debug_StageMoveDebug(&back4_dst, 9, fx, fy);
	Debug_StageMoveDebug(&back5_dst, 10, fx, fy);
	Debug_StageMoveDebug(&back6_dst, 11, fx, fy);
	Debug_StageMoveDebug(&back7_dst, 12, fx, fy);
	Stage_DrawTex(&this->tex_back6, &back6_src, &back6_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back7, &back7_src, &back7_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back4, &back4_src, &back4_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back5, &back5_src, &back5_dst, stage.camera.bzoom);
	Stage_BlendTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom, 1);
	Stage_BlendTex(&this->tex_back3, &back3_src, &back3_dst, stage.camera.bzoom, 1);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Towers_Free(StageBack *back)
{
	Back_Towers *this = (Back_Towers*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Towers_New(void)
{
	//Allocate background structure
	Back_Towers *this = (Back_Towers*)Mem_Alloc(sizeof(Back_Towers));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Towers_DrawBG;
	this->back.free = Back_Towers_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\TOWERS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Gfx_LoadTex(&this->tex_back4, Archive_Find(arc_back, "back4.tim"), 0);
	Gfx_LoadTex(&this->tex_back5, Archive_Find(arc_back, "back5.tim"), 0);
	Gfx_LoadTex(&this->tex_back6, Archive_Find(arc_back, "back6.tim"), 0);
	Gfx_LoadTex(&this->tex_back7, Archive_Find(arc_back, "back7.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
