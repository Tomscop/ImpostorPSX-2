/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "warehouse.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Warehouse background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1

} Back_Warehouse;

void Back_Warehouse_DrawMG(StageBack *back)
{
	Back_Warehouse *this = (Back_Warehouse*)back;

	fixed_t fx, fy;

	//Draw warehouse
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT glasses_src = {210,  0, 27, 54};
	RECT_FIXED glasses_dst = {
		FIXED_DEC(237 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(217,1) - fy,
		FIXED_DEC(27 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(54,1)
	};
	
	Debug_StageMoveDebug(&glasses_dst, 7, fx, fy);
	Stage_DrawTexRotate(&this->tex_back1, &glasses_src, &glasses_dst, stage.camera.bzoom, -64);
}

void Back_Warehouse_DrawBG(StageBack *back)
{
	Back_Warehouse *this = (Back_Warehouse*)back;

	fixed_t fx, fy;

	//Draw warehouse
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,254,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(254 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(255,1)
	};
	
	RECT back1_src = {  0,  0,209,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(253 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(209 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(255,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_Warehouse_Free(StageBack *back)
{
	Back_Warehouse *this = (Back_Warehouse*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Warehouse_New(void)
{
	//Allocate background structure
	Back_Warehouse *this = (Back_Warehouse*)Mem_Alloc(sizeof(Back_Warehouse));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = Back_Warehouse_DrawMG;
	this->back.draw_bg = Back_Warehouse_DrawBG;
	this->back.free = Back_Warehouse_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\WAREHOUS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
