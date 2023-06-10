/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henrybg.h"

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
	IO_Data arc_rhm, arc_rhm_ptr[3];
	Gfx_Tex tex_back0; //Stage part 1
	Gfx_Tex tex_back1; //Stage part 2
	Gfx_Tex tex_back2; //Stage part 3
	
	//RHM state
	Gfx_Tex tex_rhm;
	u8 rhm_frame, rhm_tex_id;
	Animatable rhm_animatable;

} Back_Henry;

//RHM animation and rects
static const CharFrame rhm_frame[] = {
  {0, {  0,  0,189,116}, {160,160}}, //0 rhm 1
  {0, {  0,  0,189,116}, { 16, 66}}, //1 rhm 1
  {1, {  0,  0,157,139}, {-128, -28}}, //2 rhm 2
  {2, {  0,  0,157,139}, {-129, -28}}, //3 rhm 3
};

static const Animation rhm_anim[] = {
	{1, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //Idle
};

//RHM functions
void Henry_RHM_SetFrame(void *user, u8 frame)
{
	Back_Henry *this = (Back_Henry*)user;
	
	//Check if this is a new frame
	if (frame != this->rhm_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &rhm_frame[this->rhm_frame = frame];
		if (cframe->tex != this->rhm_tex_id)
			Gfx_LoadTex(&this->tex_rhm, this->arc_rhm_ptr[this->rhm_tex_id = cframe->tex], 0);
	}
}

void Henry_RHM_Draw(Back_Henry *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &rhm_frame[this->rhm_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 10, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_rhm, &src, &dst, stage.camera.bzoom);
}

void Back_Henry_DrawFG(StageBack *back)
{
	Back_Henry *this = (Back_Henry*)back;

	fixed_t fx, fy;

	//Draw rhm
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.stage_id == StageId_Reinforcements)
	{
	if ((stage.stage_id == StageId_Reinforcements) && (stage.pink == 1))
		Animatable_SetAnim(&this->rhm_animatable, 0);
	Animatable_Animate(&this->rhm_animatable, (void*)this, Henry_RHM_SetFrame);
	if ((stage.stage_id == StageId_Reinforcements) && (stage.pink >= 1))
		Henry_RHM_Draw(this, FIXED_DEC(-179 - 129,1) - fx, FIXED_DEC(-17 - 28,1) - fy);
	}
}
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
	
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		Gfx_DrawRect(&screen_src, 187, 202, 209);
}

void Back_Henry_Free(StageBack *back)
{
	Back_Henry *this = (Back_Henry*)back;
	
	if (stage.stage_id == StageId_Reinforcements)
	{
		//Free rhm archive
		Mem_Free(this->arc_rhm);
	}
	
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
	this->back.draw_fg = Back_Henry_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Henry_DrawBG;
	this->back.free = Back_Henry_Free;

	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\HENRY.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	//Load rhm textures
	if (stage.stage_id == StageId_Reinforcements)
	{
		this->arc_rhm = IO_Read("\\BG\\RHM.ARC;1");
		this->arc_rhm_ptr[0] = Archive_Find(this->arc_rhm, "rhm0.tim");
		this->arc_rhm_ptr[1] = Archive_Find(this->arc_rhm, "rhm1.tim");
		this->arc_rhm_ptr[2] = Archive_Find(this->arc_rhm, "rhm2.tim");
	}
	
	//Initialize rhm state
	if (stage.stage_id == StageId_Reinforcements)
	{
		Animatable_Init(&this->rhm_animatable, rhm_anim);
		Animatable_SetAnim(&this->rhm_animatable, 0);
		this->rhm_frame = this->rhm_tex_id = 0xFF; //Force art load
	}

	Gfx_SetClear(0, 0, 0);
	
	return (StageBack*)this;
}
