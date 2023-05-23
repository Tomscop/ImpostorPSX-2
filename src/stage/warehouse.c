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
int bladey = -151;

//Warehouse background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_blades, arc_blades_ptr[3];
	IO_Data arc_intro, arc_intro_ptr[1];
	IO_Data arc_rozebud, arc_rozebud_ptr[6];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_monty; //monty
	
	//Blades state
	Gfx_Tex tex_blades;
	u8 blades_frame, blades_tex_id;
	Animatable blades_animatable;
	
	//Intro state
	Gfx_Tex tex_intro;
	u8 intro_frame, intro_tex_id;
	Animatable intro_animatable;
	
	//Rozebud state
	Gfx_Tex tex_rozebud;
	u8 rozebud_frame, rozebud_tex_id;
	Animatable rozebud_animatable;
	
	//fade stuff
	fixed_t fade, fadespd;
	
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

//Rozebud animation and rects
static const CharFrame rozebud_frame[] = {
  {0, {  0,  0, 84, 97}, {160,123}}, //0 rozebud 1
  {0, { 84,  0, 80,102}, {133,140}}, //1 rozebud 2
  {0, {164,  0, 78,106}, {115,152}}, //2 rozebud 3
  {0, {  0,106, 77,107}, {113,153}}, //3 rozebud 4
  {0, { 77,106, 79,106}, {113,152}}, //4 rozebud 5
  {0, {156,106, 74,108}, {112,159}}, //5 rozebud 6
  {1, {  0,  0, 74,108}, {112,159}}, //6 rozebud 7
  {1, { 74,  0, 78,107}, {113,155}}, //7 rozebud 8
  {1, {152,  0, 78,107}, {113,155}}, //8 rozebud 9
  {1, {  0,108, 78,106}, {113,153}}, //9 rozebud 10
  {1, { 78,108, 78,106}, {113,153}}, //10 rozebud 11
  {1, {156,108, 77,107}, {112,154}}, //11 rozebud 12
  {2, {  0,  0, 76,107}, {111,156}}, //12 rozebud 13
  {2, { 76,  0, 76,108}, {112,156}}, //13 rozebud 14
  {2, {152,  0, 78,106}, {113,153}}, //14 rozebud 15
  {2, {  0,108, 78,106}, {113,153}}, //15 rozebud 16
  {2, { 78,108, 77,107}, {113,154}}, //16 rozebud 17
  {2, {155,108, 78,106}, {113,153}}, //17 rozebud 18
  {3, {  0,  0, 78,107}, {113,154}}, //18 rozebud 19
  {3, { 78,  0, 77,107}, {113,154}}, //19 rozebud 20
  {3, {155,  0, 77,107}, {113,154}}, //20 rozebud 21
  {3, {  0,107, 78,106}, {113,153}}, //21 rozebud 22
  {3, { 78,107, 80,105}, {113,151}}, //22 rozebud 23
  {3, {158,107, 78,106}, {113,152}}, //23 rozebud 24
  {4, {  0,  0, 78,107}, {113,153}}, //24 rozebud 25
  {4, { 78,  0, 80,105}, {113,151}}, //25 rozebud 26
  {4, {158,  0, 80,105}, {113,151}}, //26 rozebud 27
  {4, {  0,107, 79,106}, {113,152}}, //27 rozebud 28
  {4, { 79,107, 78,106}, {112,152}}, //28 rozebud 29
  {4, {157,107, 77,107}, {112,154}}, //29 rozebud 30
  {5, {  0,  0, 77,107}, {113,154}}, //30 rozebud 31
  {5, { 77,  0, 78,107}, {113,154}}, //31 rozebud 32
  {5, {155,  0, 81,101}, {141,134}}, //32 rozebud 33
  {5, {  0,107, 83, 97}, {157,124}}, //33 rozebud 34
  {5, { 83,107, 83, 97}, {158,123}}, //34 rozebud 35
  {5, {166,107, 83, 97}, {160,123}}, //35 rozebud 36
  
  {0, {166,107,  1,  1}, {160,123}}, //36 rozebud 36
};

static const Animation rozebud_anim[] = {
	{2, (const u8[]){ 36, ASCR_BACK, 1}}, //Hide
	
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 11, 11, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, ASCR_BACK, 1}}, //1
	{1, (const u8[]){ 22, 22, 23, 23, 24, ASCR_BACK, 1}}, //2
	{1, (const u8[]){ 25, 26, 27, 28, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, ASCR_CHGANI, 0}}, //3
};

//Rozebud functions
void Warehouse_Rozebud_SetFrame(void *user, u8 frame)
{
	Back_Warehouse *this = (Back_Warehouse*)user;
	
	//Check if this is a new frame
	if (frame != this->rozebud_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &rozebud_frame[this->rozebud_frame = frame];
		if (cframe->tex != this->rozebud_tex_id)
			Gfx_LoadTex(&this->tex_rozebud, this->arc_rozebud_ptr[this->rozebud_tex_id = cframe->tex], 0);
	}
}

void Warehouse_Rozebud_Draw(Back_Warehouse *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &rozebud_frame[this->rozebud_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 12, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_rozebud, &src, &dst, stage.camera.bzoom);
}

//Intro animation and rects
static const CharFrame intro_frame[] = {
  {0, {  0,  0, 22, 27}, {160,150}}, //0 intro 1
  {0, { 22,  0, 19, 26}, {156,153}}, //1 intro 2
  {0, { 41,  0, 22, 25}, {156,153}}, //2 intro 3
  {0, { 63,  0, 23, 23}, {156,153}}, //3 intro 4
  {0, { 86,  0, 24, 33}, {155,159}}, //4 intro 5
  {0, {110,  0, 22, 32}, {153,157}}, //5 intro 6
  {0, {132,  0, 23, 30}, {155,160}}, //6 intro 7
  {0, {155,  0, 22, 29}, {155,159}}, //7 intro 8
  {0, {177,  0, 21, 30}, {155,159}}, //8 intro 9
  {0, {198,  0, 20, 28}, {155,159}}, //9 intro 10
  {0, {218,  0, 20, 32}, {148,156}}, //10 intro 11
  {0, {  0, 33, 20, 35}, {150,158}}, //11 intro 12
  {0, { 20, 33, 20, 29}, {152,159}}, //12 intro 13
  {0, { 40, 33, 21, 29}, {152,159}}, //13 intro 14
  {0, { 61, 33, 20, 33}, {158,155}}, //14 intro 15
  {0, { 81, 33, 20, 32}, {156,158}}, //15 intro 16
  {0, {101, 33, 19, 30}, {155,159}}, //16 intro 17
  {0, {120, 33, 23, 30}, {155,159}}, //17 intro 18
  {0, {143, 33, 20, 35}, {151,157}}, //18 intro 19
  {0, {163, 33, 20, 31}, {151,159}}, //19 intro 20
  {0, {183, 33, 20, 32}, {152,159}}, //20 intro 21
  {0, {203, 33, 17, 30}, {149,157}}, //21 intro 22
  {0, {220, 33, 17, 30}, {148,157}}, //22 intro 23
  {0, {237, 33, 17, 28}, {149,156}}, //23 intro 24
  {0, {  0, 68, 17, 26}, {149,157}}, //24 intro 25
  {0, { 17, 68, 21, 36}, {152,160}}, //25 intro 26
  {0, { 38, 68, 20, 33}, {151,159}}, //26 intro 27
  {0, { 58, 68, 22, 21}, {151,151}}, //27 intro 28
  {0, { 80, 68, 20, 37}, {152,159}}, //28 intro 29
  {0, {100, 68, 20, 35}, {152,159}}, //29 intro 30
  {0, {120, 68, 21, 22}, {151,151}}, //30 intro 31
  {0, {141, 68, 22, 30}, {154,157}}, //31 intro 32
  {0, {163, 68, 22, 30}, {155,158}}, //32 intro 33
  {0, {185, 68, 22, 30}, {154,158}}, //33 intro 34
  {0, {207, 68, 23, 30}, {153,159}}, //34 intro 35
  {0, {230, 68, 22, 30}, {153,159}}, //35 intro 36
  
  {0, {  0,  0,  1,  1}, {153,159}}, //36 hide
};

static const Animation intro_anim[] = {
	{1, (const u8[]){ 36, 36, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 7, 7, 7, 7, 		8, 8, 9, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 15, 15, 16, 16, 16, 16, 17, 17, 8, 8, 9, 9, 9, 18, 18, 19, 19, 20, 20, 12, 12, 13, 13, 13, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 12, 12, 27, 27, 25, 25, 26, 26, 12, 12, 26, 26, 12, 12, 28, 28, 28, 28, 26, 26, 		 29, 29, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, ASCR_CHGANI, 1}}, //Idle
	{1, (const u8[]){ 36, ASCR_BACK, 1}}, //Hide
};
//8, 9, 12, 13, 
//Intro functions
void Warehouse_Intro_SetFrame(void *user, u8 frame)
{
	Back_Warehouse *this = (Back_Warehouse*)user;
	
	//Check if this is a new frame
	if (frame != this->intro_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &intro_frame[this->intro_frame = frame];
		if (cframe->tex != this->intro_tex_id)
			Gfx_LoadTex(&this->tex_intro, this->arc_intro_ptr[this->intro_tex_id = cframe->tex], 0);
	}
}

void Warehouse_Intro_Draw(Back_Warehouse *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &intro_frame[this->intro_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 11, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_intro, &src, &dst, stage.camera.bzoom);
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

void Back_Warehouse_DrawFG(StageBack *back)
{
	Back_Warehouse *this = (Back_Warehouse*)back;

	fixed_t fx, fy;

	//Draw warehouse
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_beat == 0))
		Animatable_SetAnim(&this->intro_animatable, 1);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_beat == 2))
		Animatable_SetAnim(&this->intro_animatable, 0);
	Animatable_Animate(&this->intro_animatable, (void*)this, Warehouse_Intro_SetFrame);
	if (stage.song_beat < 24)
		Warehouse_Intro_Draw(this, FIXED_DEC(220 + 155,1) - fx, FIXED_DEC(92 + 158,1) - fy);
	
	//start fade
	if (stage.song_step == 128)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(36,1);
	}

	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
	
	RECT monty_src = {  0,  0, 72, 72};
	RECT_FIXED monty_dst = {
		FIXED_DEC(104 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(200,1) - fy,
		FIXED_DEC(72 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(72,1)
	};
	
	if (((stage.song_step & 0x1F) == 0) || ((stage.song_step & 0x1F) == 1))
		monty_src.y += 73;
	
	Debug_StageMoveDebug(&monty_dst, 10, fx, fy);
	Stage_DrawTex(&this->tex_monty, &monty_src, &monty_dst, stage.camera.bzoom);
}

void Back_Warehouse_DrawMG(StageBack *back)
{
	Back_Warehouse *this = (Back_Warehouse*)back;

	fixed_t fx, fy;

	//Draw warehouse
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Rozebud
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_beat == 0))
		Animatable_SetAnim(&this->rozebud_animatable, 0);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_beat == 255))
		Animatable_SetAnim(&this->rozebud_animatable, 1);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1045))
		Animatable_SetAnim(&this->rozebud_animatable, 2);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1081))
		Animatable_SetAnim(&this->rozebud_animatable, 3);
	Animatable_Animate(&this->rozebud_animatable, (void*)this, Warehouse_Rozebud_SetFrame);
	if ((stage.song_beat >= 255) && (stage.song_beat <= 273))
		Warehouse_Rozebud_Draw(this, FIXED_DEC(55 + 113,1) - fx, FIXED_DEC(13 + 153,1) - fy);
	
	//Blades
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1))
		Animatable_SetAnim(&this->blades_animatable, 0);
	Animatable_Animate(&this->blades_animatable, (void*)this, Warehouse_Blades_SetFrame);
	Warehouse_Blades_DrawLeft(this, FIXED_DEC(157 - bladex + 158,1) - fx, FIXED_DEC(bladey + 158,1) - fy); //L 157  H 123
	Warehouse_Blades_DrawRight(this, FIXED_DEC(308 + bladex + 158,1) - fx, FIXED_DEC(bladey + 158,1) - fy); //L 308  H 342
	Back_Warehouse_BladeX();
	if (stage.song_step == 0)
		bladey = -151;
	if ((stage.song_beat >= 62) && (bladey != -31))
		bladey += 2;
	
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
	
	//Free intro archive
	Mem_Free(this->arc_intro);
	
	//Free rozebud archive
	Mem_Free(this->arc_rozebud);
	
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
	this->back.draw_fg = Back_Warehouse_DrawFG;
	this->back.draw_md = Back_Warehouse_DrawMG;
	this->back.draw_bg = Back_Warehouse_DrawBG;
	this->back.free = Back_Warehouse_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\WAREHOUS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_monty, Archive_Find(arc_back, "monty.tim"), 0);
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
	
	//Load intro textures
	this->arc_intro = IO_Read("\\BG\\INTRO.ARC;1");
	this->arc_intro_ptr[0] = Archive_Find(this->arc_intro, "intro0.tim");
	
	//Initialize intro state
	Animatable_Init(&this->intro_animatable, intro_anim);
	Animatable_SetAnim(&this->intro_animatable, 0);
	this->intro_frame = this->intro_tex_id = 0xFF; //Force art load
	
	//Load rozebud textures
	this->arc_rozebud = IO_Read("\\BG\\ROZEBUD.ARC;1");
	this->arc_rozebud_ptr[0] = Archive_Find(this->arc_rozebud, "rozebud0.tim");
	this->arc_rozebud_ptr[1] = Archive_Find(this->arc_rozebud, "rozebud1.tim");
	this->arc_rozebud_ptr[2] = Archive_Find(this->arc_rozebud, "rozebud2.tim");
	this->arc_rozebud_ptr[3] = Archive_Find(this->arc_rozebud, "rozebud3.tim");
	this->arc_rozebud_ptr[4] = Archive_Find(this->arc_rozebud, "rozebud4.tim");
	this->arc_rozebud_ptr[5] = Archive_Find(this->arc_rozebud, "rozebud5.tim");
	
	//Initialize rozebud state
	Animatable_Init(&this->rozebud_animatable, rozebud_anim);
	Animatable_SetAnim(&this->rozebud_animatable, 0);
	this->rozebud_frame = this->rozebud_tex_id = 0xFF; //Force art load
	
	//Initialize Fade
	this->fade = FIXED_DEC(255,1);
	this->fadespd = 0;
	
	return (StageBack*)this;
}
