/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mcdonalds.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//McDonalds background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_cooper, arc_cooper_ptr[4];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	
	//Cooper state
	Gfx_Tex tex_cooper;
	u8 cooper_frame, cooper_tex_id;
	Animatable cooper_animatable;
	
} Back_McDonalds;

//Cooper animation and rects
static const CharFrame cooper_frame[] = {
  {0, {  0,  0,170,161}, {160,159}}, //0 cooper 1
  {1, {  0,  0,170,161}, {160,159}}, //1 cooper 2
  {2, {  0,  0,170,162}, {160,160}}, //2 cooper 3
  {3, {  0,  0,170,162}, {160,160}}, //3 cooper 4
};

static const Animation cooper_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //Idle
};

//Cooper functions
void McDonalds_Cooper_SetFrame(void *user, u8 frame)
{
	Back_McDonalds *this = (Back_McDonalds*)user;
	
	//Check if this is a new frame
	if (frame != this->cooper_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &cooper_frame[this->cooper_frame = frame];
		if (cframe->tex != this->cooper_tex_id)
			Gfx_LoadTex(&this->tex_cooper, this->arc_cooper_ptr[this->cooper_tex_id = cframe->tex], 0);
	}
}

void McDonalds_Cooper_Draw(Back_McDonalds *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &cooper_frame[this->cooper_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 10, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_cooper, &src, &dst, stage.camera.bzoom);
}

void Back_McDonalds_DrawMG(StageBack *back)
{
	Back_McDonalds *this = (Back_McDonalds*)back;

	fixed_t fx, fy;

	//Draw mcdonalds
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back2_src = {  0,  0,127, 79};
	RECT_FIXED back2_dst = {
		stage.gf->x-FIXED_DEC(161,1) - fx,
		stage.gf->y-FIXED_DEC(90,1) - fy,
		FIXED_DEC(127 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(79,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom);
}

void Back_McDonalds_DrawBG(StageBack *back)
{
	Back_McDonalds *this = (Back_McDonalds*)back;

	fixed_t fx, fy;

	//Draw mcdonalds
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->cooper_animatable, 0);
	Animatable_Animate(&this->cooper_animatable, (void*)this, McDonalds_Cooper_SetFrame);
	McDonalds_Cooper_Draw(this, FIXED_DEC(0 + 160,1) - fx, FIXED_DEC(0 + 160,1) - fy);
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(303 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(303,1)
	};
	
	RECT back1_src = {  0,  0,199,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(302 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(237 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(303,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_McDonalds_Free(StageBack *back)
{
	Back_McDonalds *this = (Back_McDonalds*)back;
	
	//Free cooper archive
	Mem_Free(this->arc_cooper);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_McDonalds_New(void)
{
	//Allocate background structure
	Back_McDonalds *this = (Back_McDonalds*)Mem_Alloc(sizeof(Back_McDonalds));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = Back_McDonalds_DrawMG;
	this->back.draw_bg = Back_McDonalds_DrawBG;
	this->back.free = Back_McDonalds_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\MCDONLDS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	//Load cooper textures
	this->arc_cooper = IO_Read("\\BG\\COOPER.ARC;1");
	this->arc_cooper_ptr[0] = Archive_Find(this->arc_cooper, "cooper0.tim");
	this->arc_cooper_ptr[1] = Archive_Find(this->arc_cooper, "cooper1.tim");
	this->arc_cooper_ptr[2] = Archive_Find(this->arc_cooper, "cooper2.tim");
	this->arc_cooper_ptr[3] = Archive_Find(this->arc_cooper, "cooper3.tim");
	
	//Initialize cooper state
	Animatable_Init(&this->cooper_animatable, cooper_anim);
	Animatable_SetAnim(&this->cooper_animatable, 0);
	this->cooper_frame = this->cooper_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
