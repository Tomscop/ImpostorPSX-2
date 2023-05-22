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

boolean healththing = false;
int bladex;

//Warehouse background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_blades, arc_blades_ptr[3];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	
	//Blades state
	Gfx_Tex tex_blades;
	u8 blades_frame, blades_tex_id;
	Animatable blades_animatable;
	
} Back_Warehouse;

//Blades animation and rects
static const CharFrame blades_frame[] = {
  {0, {  0,  0, 63,151}, {159,158}}, //0 blades 1
  {0, { 63,  0, 66,148}, {159,158}}, //1 blades 2
  {0, {129,  0, 68,152}, {159,159}}, //2 blades 3
  {1, {  0,  0, 65,151}, {159,160}}, //3 blades 4
  {1, { 65,  0, 64,151}, {158,157}}, //4 blades 5
  {1, {129,  0, 66,149}, {158,158}}, //5 blades 6
  {2, {  0,  0, 66,152}, {158,158}}, //6 blades 7
  {2, { 66,  0, 64,151}, {159,158}}, //7 blades 8
};

static const Animation blades_anim[] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_CHGANI, 0}}, //Idle
};

//Blades functions
void Warehouse_Blades_SetFrame(void *user, u8 frame)
{
	Back_Warehouse *this = (Back_Warehouse*)user;
	
	//Check if this is a new frame
	if (frame != this->blades_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &blades_frame[this->blades_frame = frame];
		if (cframe->tex != this->blades_tex_id)
			Gfx_LoadTex(&this->tex_blades, this->arc_blades_ptr[this->blades_tex_id = cframe->tex], 0);
	}
}

void Warehouse_Blades_DrawLeft(Back_Warehouse *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &blades_frame[this->blades_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_blades, &src, &dst, stage.camera.bzoom);
}

void Warehouse_Blades_DrawRight(Back_Warehouse *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &blades_frame[this->blades_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTexFlipped(&this->tex_blades, &src, &dst, stage.camera.bzoom);
}

void Back_Warehouse_BladeX(void)
{
	//+587
	if ((stage.player_state[0].health >= 0) && (stage.player_state[0].health <= 294))
		bladex = 0;
	else if ((stage.player_state[0].health >= 295) && (stage.player_state[0].health <= 882))
		bladex = 1;
	else if ((stage.player_state[0].health >= 883) && (stage.player_state[0].health <= 1470))
		bladex = 2;
	else if ((stage.player_state[0].health >= 1471) && (stage.player_state[0].health <= 2058))
		bladex = 3;
	else if ((stage.player_state[0].health >= 2059) && (stage.player_state[0].health <= 2647))
		bladex = 4;
	else if ((stage.player_state[0].health >= 2648) && (stage.player_state[0].health <= 3235))
		bladex = 5;
	else if ((stage.player_state[0].health >= 3236) && (stage.player_state[0].health <= 3823))
		bladex = 6;
	else if ((stage.player_state[0].health >= 3824) && (stage.player_state[0].health <= 4411))
		bladex = 7;
	else if ((stage.player_state[0].health >= 4412) && (stage.player_state[0].health <= 4999))
		bladex = 8;
	else if ((stage.player_state[0].health >= 5000) && (stage.player_state[0].health <= 5588))
		bladex = 9;
	else if ((stage.player_state[0].health >= 5589) && (stage.player_state[0].health <= 6176))
		bladex = 10;
	else if ((stage.player_state[0].health >= 6177) && (stage.player_state[0].health <= 6764))
		bladex = 11;
	else if ((stage.player_state[0].health >= 6765) && (stage.player_state[0].health <= 7352))
		bladex = 12;
	else if ((stage.player_state[0].health >= 7353) && (stage.player_state[0].health <= 7940))
		bladex = 13;
	else if ((stage.player_state[0].health >= 7941) && (stage.player_state[0].health <= 8528))
		bladex = 14;
	else if ((stage.player_state[0].health >= 8529) && (stage.player_state[0].health <= 9116))
		bladex = 15;
	else if ((stage.player_state[0].health >= 9117) && (stage.player_state[0].health <= 9704))
		bladex = 16;
	else if ((stage.player_state[0].health >= 9705) && (stage.player_state[0].health <= 10292))
		bladex = 17;
	else if ((stage.player_state[0].health >= 10293) && (stage.player_state[0].health <= 10880))
		bladex = 18;
	else if ((stage.player_state[0].health >= 10881) && (stage.player_state[0].health <= 11468))
		bladex = 19;
	else if ((stage.player_state[0].health >= 11469) && (stage.player_state[0].health <= 12056))
		bladex = 20;
	else if ((stage.player_state[0].health >= 12057) && (stage.player_state[0].health <= 12644))
		bladex = 21;
	else if ((stage.player_state[0].health >= 12645) && (stage.player_state[0].health <= 13232))
		bladex = 22;
	else if ((stage.player_state[0].health >= 13233) && (stage.player_state[0].health <= 13820))
		bladex = 23;
	else if ((stage.player_state[0].health >= 13821) && (stage.player_state[0].health <= 14408))
		bladex = 24;
	else if ((stage.player_state[0].health >= 14409) && (stage.player_state[0].health <= 14996))
		bladex = 25;
	else if ((stage.player_state[0].health >= 14997) && (stage.player_state[0].health <= 15584))
		bladex = 26;
	else if ((stage.player_state[0].health >= 15585) && (stage.player_state[0].health <= 16172))
		bladex = 27;
	else if ((stage.player_state[0].health >= 16173) && (stage.player_state[0].health <= 16760))
		bladex = 28;
	else if ((stage.player_state[0].health >= 16761) && (stage.player_state[0].health <= 17348))
		bladex = 29;
	else if ((stage.player_state[0].health >= 17349) && (stage.player_state[0].health <= 17936))
		bladex = 30;
	else if ((stage.player_state[0].health >= 17937) && (stage.player_state[0].health <= 18524))
		bladex = 31;
	else if ((stage.player_state[0].health >= 18525) && (stage.player_state[0].health <= 19112))
		bladex = 32;
	else if ((stage.player_state[0].health >= 19113) && (stage.player_state[0].health <= 19700))
		bladex = 33;
	else
		bladex = 34;
}
void Back_Warehouse_DrawMG(StageBack *back)
{
	Back_Warehouse *this = (Back_Warehouse*)back;

	fixed_t fx, fy;

	//Draw warehouse
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Blades
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1))
		Animatable_SetAnim(&this->blades_animatable, 0);
	Animatable_Animate(&this->blades_animatable, (void*)this, Warehouse_Blades_SetFrame);
	Warehouse_Blades_DrawLeft(this, FIXED_DEC(157 - bladex + 158,1) - fx, FIXED_DEC(-31 + 158,1) - fy); //L 157  H 123
	Warehouse_Blades_DrawRight(this, FIXED_DEC(308 + bladex + 158,1) - fx, FIXED_DEC(-31 + 158,1) - fy); //L 308  H 342
	Back_Warehouse_BladeX();
	
	RECT glasses_src = {210,  0, 27, 54};
	RECT_FIXED glasses_dst = {
		FIXED_DEC(237 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(217,1) - fy,
		FIXED_DEC(27 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(54,1)
	};
	
	Debug_StageMoveDebug(&glasses_dst, 9, fx, fy);
	Stage_DrawTexRotate(&this->tex_back1, &glasses_src, &glasses_dst, stage.camera.bzoom, -64);
	
	//health loss part
	if (stage.song_step <= 2)
		healththing = false;
	if (stage.song_beat == 272)
		healththing = true;
	if ((healththing) && (stage.player_state[0].health >= 476))
		stage.player_state[0].health -= 450;
	else if ((healththing) && (stage.player_state[0].health <= 475))
	{
		stage.player_state[0].health = 300;
		healththing = false;
	}
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
	
	//Free blades archive
	Mem_Free(this->arc_blades);
	
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
	
	//Load blades textures
	this->arc_blades = IO_Read("\\BG\\BLADES.ARC;1");
	this->arc_blades_ptr[0] = Archive_Find(this->arc_blades, "blades0.tim");
	this->arc_blades_ptr[1] = Archive_Find(this->arc_blades, "blades1.tim");
	this->arc_blades_ptr[2] = Archive_Find(this->arc_blades, "blades2.tim");
	
	//Initialize blades state
	Animatable_Init(&this->blades_animatable, blades_anim);
	Animatable_SetAnim(&this->blades_animatable, 0);
	this->blades_frame = this->blades_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
