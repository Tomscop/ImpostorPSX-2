/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "shields.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

int eject = -207;
u8 sprite_angle = 0;

//Shields background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_ending; //ending
	Gfx_Tex tex_alert; //alert

} Back_Shields;

void Back_Shields_DrawFG(StageBack *back)
{
	Back_Shields *this = (Back_Shields*)back;
	
	RECT emergency_src = {  0,135,103,120};
	RECT_FIXED emergency_dst = {
		FIXED_DEC(-90 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-105,1),
		FIXED_DEC(180 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(210,1)
	};
	
	RECT eject_src = {104,135, 63, 84};
	RECT_FIXED eject_dst = {
		FIXED_DEC(eject - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(0,1),
		FIXED_DEC(47 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(63,1)
	};
	
	RECT stars_src = {  0,  0,241,134};
	RECT_FIXED stars_dst = {
		FIXED_DEC(-175 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-120,1),
		FIXED_DEC(432 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(240,1)
	};
	
	RECT alert1_src = {  0,  0,169, 81};
	RECT_FIXED alert1_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-77,1),
		FIXED_DEC(320 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(154,1)
	};
	
	RECT alert2_src = {  0, 82,169, 81};
	RECT_FIXED alert2_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-77,1),
		FIXED_DEC(320 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(154,1)
	};
	
	RECT alert3_src = {  0,164,169, 81};
	RECT_FIXED alert3_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-77,1),
		FIXED_DEC(320 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(154,1)
	};
	
	Debug_StageMoveDebug(&emergency_dst, 7, emergency_dst.x, emergency_dst.y);
	Debug_StageMoveDebug(&eject_dst, 8, eject_dst.x, eject_dst.y);
	Debug_StageMoveDebug(&stars_dst, 9, stars_dst.x, stars_dst.y);
	Debug_StageMoveDebug(&alert1_dst, 10, alert1_dst.x, alert1_dst.y);
	Debug_StageMoveDebug(&alert2_dst, 10, alert2_dst.x, alert2_dst.y);
	Debug_StageMoveDebug(&alert3_dst, 10, alert3_dst.x, alert3_dst.y);
	if ((stage.song_step >= 1156) && (stage.song_step <= 1167))
		Stage_DrawTex(&this->tex_ending, &emergency_src, &emergency_dst, FIXED_DEC(1,1));
	if (stage.song_step >= 1168)
	{
		Stage_DrawTexRotate(&this->tex_ending, &eject_src, &eject_dst, FIXED_DEC(1,1), sprite_angle);
		Stage_DrawTex(&this->tex_ending, &stars_src, &stars_dst, FIXED_DEC(1,1));
	}
	if (stage.song_step == 1152)
		Stage_DrawTex(&this->tex_alert, &alert1_src, &alert1_dst, FIXED_DEC(1,1));
	if (stage.song_step == 1153)
		Stage_DrawTex(&this->tex_alert, &alert2_src, &alert2_dst, FIXED_DEC(1,1));
	if (stage.song_step == 1154)
		Stage_DrawTex(&this->tex_alert, &alert3_src, &alert3_dst, FIXED_DEC(1,1));
	if ((stage.song_step >= 1155) && (stage.song_step <= 1167))
		Stage_DrawTex(&this->tex_alert, &alert1_src, &alert1_dst, FIXED_DEC(1,1));
	
	if (stage.song_step >= 1168)
	{
		eject += 1;
		sprite_angle += 1;
	}
}

void Back_Shields_DrawBG(StageBack *back)
{
	Back_Shields *this = (Back_Shields*)back;

	fixed_t fx, fy;

	//Draw shields
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(273 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(273,1)
	};
	
	RECT back1_src = {  0,  0,188,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(272 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(201 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(273,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Shields_Free(StageBack *back)
{
	Back_Shields *this = (Back_Shields*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Shields_New(void)
{
	//Allocate background structure
	Back_Shields *this = (Back_Shields*)Mem_Alloc(sizeof(Back_Shields));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Shields_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Shields_DrawBG;
	this->back.free = Back_Shields_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG2\\SHIELDS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_ending, Archive_Find(arc_back, "ending.tim"), 0);
	Gfx_LoadTex(&this->tex_alert, Archive_Find(arc_back, "alert.tim"), 0);
	Mem_Free(arc_back);
	
	eject = -207;
	
	return (StageBack*)this;
}
