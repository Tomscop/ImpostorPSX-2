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
	IO_Data arc_orbyy, arc_orbyy_ptr[6];
	IO_Data arc_offbi, arc_offbi_ptr[3];
	IO_Data arc_nickt, arc_nickt_ptr[6];
	IO_Data arc_cooper, arc_cooper_ptr[4];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	
	//Orbyy state
	Gfx_Tex tex_orbyy;
	u8 orbyy_frame, orbyy_tex_id;
	Animatable orbyy_animatable;
	
	//Offbi state
	Gfx_Tex tex_offbi;
	u8 offbi_frame, offbi_tex_id;
	Animatable offbi_animatable;
	
	//Nick T state
	Gfx_Tex tex_nickt;
	u8 nickt_frame, nickt_tex_id;
	Animatable nickt_animatable;
	
	//Cooper state
	Gfx_Tex tex_cooper;
	u8 cooper_frame, cooper_tex_id;
	Animatable cooper_animatable;
	
} Back_McDonalds;

//Orbyy animation and rects
static const CharFrame orbyy_frame[] = {
  {0, {  0,  0, 78,165}, {160,156}}, //0 orbyy 1
  {0, { 78,  0, 74,165}, {158,155}}, //1 orbyy 2
  {0, {152,  0, 72,167}, {157,157}}, //2 orbyy 3
  {1, {  0,  0, 73,170}, {157,160}}, //3 orbyy 4
  {1, { 73,  0, 72,170}, {157,160}}, //4 orbyy 5

  {2, {  0,  0, 76,170}, {128+32,160}}, //5 shutup 1
  {2, { 76,  0, 77,170}, {129+32,160}}, //6 shutup 2
  {3, {  0,  0,125,170}, {159+32,160}}, //7 shutup 3
  {3, {125,  0,124,170}, {159+32,160}}, //8 shutup 4
  {4, {  0,  0,125,170}, {159+32,160}}, //9 shutup 5
  {4, {125,  0,126,168}, {160+32,159}}, //10 shutup 6
  {5, {  0,  0,124,169}, {159+32,159}}, //11 shutup 7
};

static const Animation orbyy_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //Idle
	{1, (const u8[]){ 5, 6, 7, 7, 8, 8, 8, 8, 8, 9, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, ASCR_BACK, 1}}, //Everybody shut up
};

//Orbyy functions
void McDonalds_Orbyy_SetFrame(void *user, u8 frame)
{
	Back_McDonalds *this = (Back_McDonalds*)user;
	
	//Check if this is a new frame
	if (frame != this->orbyy_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &orbyy_frame[this->orbyy_frame = frame];
		if (cframe->tex != this->orbyy_tex_id)
			Gfx_LoadTex(&this->tex_orbyy, this->arc_orbyy_ptr[this->orbyy_tex_id = cframe->tex], 0);
	}
}

void McDonalds_Orbyy_Draw(Back_McDonalds *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &orbyy_frame[this->orbyy_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	if ((stage.song_step >= 1428) && (stage.song_step <= 1439))
		Stage_BlendTex(&this->tex_orbyy, &src, &dst, stage.camera.bzoom, 1);
	else
		Stage_DrawTex(&this->tex_orbyy, &src, &dst, stage.camera.bzoom);
}

//Offbi animation and rects
static const CharFrame offbi_frame[] = {
  {0, {  0,  0,113,185}, {160,160}}, //0 offbi 1
  {0, {113,  0,110,180}, {158,155}}, //1 offbi 2
  {1, {  0,  0,110,184}, {158,158}}, //2 offbi 3
  {1, {110,  0,110,185}, {158,160}}, //3 offbi 4
  {2, {  0,  0,110,185}, {158,160}}, //4 offbi 5
};

static const Animation offbi_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //Idle
};

//Offbi functions
void McDonalds_Offbi_SetFrame(void *user, u8 frame)
{
	Back_McDonalds *this = (Back_McDonalds*)user;
	
	//Check if this is a new frame
	if (frame != this->offbi_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &offbi_frame[this->offbi_frame = frame];
		if (cframe->tex != this->offbi_tex_id)
			Gfx_LoadTex(&this->tex_offbi, this->arc_offbi_ptr[this->offbi_tex_id = cframe->tex], 0);
	}
}

void McDonalds_Offbi_Draw(Back_McDonalds *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &offbi_frame[this->offbi_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	if ((stage.song_step >= 1428) && (stage.song_step <= 1439))
		Stage_BlendTex(&this->tex_offbi, &src, &dst, stage.camera.bzoom, 1);
	else
		Stage_DrawTex(&this->tex_offbi, &src, &dst, stage.camera.bzoom);
}

//NickT animation and rects
static const CharFrame nickt_frame[] = {
  {0, {  0,  0, 56,143}, {158,158}}, //0 nickt 1
  {0, { 56,  0, 56,143}, {159,158}}, //1 nickt 2
  {0, {112,  0, 56,145}, {159,159}}, //2 nickt 3
  {0, {168,  0, 57,145}, {160,160}}, //3 nickt 4
  {1, {  0,  0, 57,145}, {160,160}}, //4 nickt 5

  {2, {  0,  0, 55,143}, {157,158}}, //5 forehead 1
  {2, { 55,  0, 54,144}, {157,159}}, //6 forehead 2
  {2, {109,  0, 54,144}, {157,159}}, //7 forehead 3
  {2, {163,  0, 53,145}, {157,160}}, //8 forehead 4
  {3, {  0,  0, 57,143}, {160,158}}, //9 forehead 5
  {3, { 57,  0, 57,143}, {160,158}}, //10 forehead 6
  {3, {114,  0, 56,144}, {159,158}}, //11 forehead 7
  {3, {170,  0, 56,144}, {159,158}}, //12 forehead 8
  {4, {  0,  0, 56,144}, {159,158}}, //13 forehead 9
  {4, { 56,  0, 56,143}, {159,158}}, //14 forehead 10
  {4, {112,  0, 56,143}, {159,158}}, //15 forehead 11
  {4, {168,  0, 56,143}, {159,158}}, //16 forehead 12
  {5, {  0,  0, 56,143}, {159,158}}, //17 forehead 13
};

static const Animation nickt_anim[] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //Idle
	{1, (const u8[]){ 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 8, 8, ASCR_BACK, 1}}, //Uhhhhh
	{1, (const u8[]){ 9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, ASCR_BACK, 1}}, //Forehead
};

//NickT functions
void McDonalds_NickT_SetFrame(void *user, u8 frame)
{
	Back_McDonalds *this = (Back_McDonalds*)user;
	
	//Check if this is a new frame
	if (frame != this->nickt_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &nickt_frame[this->nickt_frame = frame];
		if (cframe->tex != this->nickt_tex_id)
			Gfx_LoadTex(&this->tex_nickt, this->arc_nickt_ptr[this->nickt_tex_id = cframe->tex], 0);
	}
}

void McDonalds_NickT_Draw(Back_McDonalds *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &nickt_frame[this->nickt_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 9, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_nickt, &src, &dst, stage.camera.bzoom);
}

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
	if ((stage.song_step >= 1428) && (stage.song_step <= 1439))
		Stage_BlendTex(&this->tex_cooper, &src, &dst, stage.camera.bzoom, 1);
	else
		Stage_DrawTex(&this->tex_cooper, &src, &dst, stage.camera.bzoom);
}

void Back_McDonalds_DrawMG(StageBack *back)
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
	
	RECT back2_src = {  0,  0,127, 79};
	RECT_FIXED back2_dst = {
		stage.gf->x-FIXED_DEC(161,1) - fx,
		stage.gf->y-FIXED_DEC(90,1) - fy,
		FIXED_DEC(127 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(79,1)
	};
	
	if ((stage.song_step >= 1428) && (stage.song_step <= 1439))
		Stage_BlendTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom, 1);
	else
		Stage_DrawTex(&this->tex_back2, &back2_src, &back2_dst, stage.camera.bzoom);
}

void Back_McDonalds_DrawBG(StageBack *back)
{
	Back_McDonalds *this = (Back_McDonalds*)back;

	fixed_t fx, fy;

	//Draw mcdonalds
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 4) && ((stage.song_step <= 383) || (stage.song_step >= 396)))
		Animatable_SetAnim(&this->orbyy_animatable, 0);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 384))
		Animatable_SetAnim(&this->orbyy_animatable, 1);
	Animatable_Animate(&this->orbyy_animatable, (void*)this, McDonalds_Orbyy_SetFrame);
	McDonalds_Orbyy_Draw(this, FIXED_DEC(0 + 157,1) - fx, FIXED_DEC(0 + 160,1) - fy);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 4)
		Animatable_SetAnim(&this->offbi_animatable, 0);
	Animatable_Animate(&this->offbi_animatable, (void*)this, McDonalds_Offbi_SetFrame);
	McDonalds_Offbi_Draw(this, FIXED_DEC(0 + 158,1) - fx, FIXED_DEC(0 + 160,1) - fy);
	
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0) && ((stage.song_step <= 1426) || (stage.song_step >= 1440)))
		Animatable_SetAnim(&this->nickt_animatable, 0);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1427))
		Animatable_SetAnim(&this->nickt_animatable, 1);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1435))
		Animatable_SetAnim(&this->nickt_animatable, 2);
	Animatable_Animate(&this->nickt_animatable, (void*)this, McDonalds_NickT_SetFrame);
	McDonalds_NickT_Draw(this, FIXED_DEC(0 + 160,1) - fx, FIXED_DEC(0 + 160,1) - fy);
	
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
	
	//Free orbyy archive
	Mem_Free(this->arc_orbyy);
	
	//Free offbi archive
	Mem_Free(this->arc_offbi);
	
	//Free nick t archive
	Mem_Free(this->arc_nickt);
	
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
	
	//Load orbyy textures
	this->arc_orbyy = IO_Read("\\BG\\ORBYY.ARC;1");
	this->arc_orbyy_ptr[0] = Archive_Find(this->arc_orbyy, "orbyy0.tim");
	this->arc_orbyy_ptr[1] = Archive_Find(this->arc_orbyy, "orbyy1.tim");
	this->arc_orbyy_ptr[2] = Archive_Find(this->arc_orbyy, "shutup0.tim");
	this->arc_orbyy_ptr[3] = Archive_Find(this->arc_orbyy, "shutup1.tim");
	this->arc_orbyy_ptr[4] = Archive_Find(this->arc_orbyy, "shutup2.tim");
	this->arc_orbyy_ptr[5] = Archive_Find(this->arc_orbyy, "shutup3.tim");
	
	//Initialize orbyy state
	Animatable_Init(&this->orbyy_animatable, orbyy_anim);
	Animatable_SetAnim(&this->orbyy_animatable, 0);
	this->orbyy_frame = this->orbyy_tex_id = 0xFF; //Force art load
	
	//Load offbi textures
	this->arc_offbi = IO_Read("\\BG\\OFFBI.ARC;1");
	this->arc_offbi_ptr[0] = Archive_Find(this->arc_offbi, "offbi0.tim");
	this->arc_offbi_ptr[1] = Archive_Find(this->arc_offbi, "offbi1.tim");
	this->arc_offbi_ptr[2] = Archive_Find(this->arc_offbi, "offbi2.tim");
	
	//Initialize offbi state
	Animatable_Init(&this->offbi_animatable, offbi_anim);
	Animatable_SetAnim(&this->offbi_animatable, 0);
	this->offbi_frame = this->offbi_tex_id = 0xFF; //Force art load
	
	//Load nick t textures
	this->arc_nickt = IO_Read("\\BG\\NICKT.ARC;1");
	this->arc_nickt_ptr[0] = Archive_Find(this->arc_nickt, "nickt0.tim");
	this->arc_nickt_ptr[1] = Archive_Find(this->arc_nickt, "nickt1.tim");
	this->arc_nickt_ptr[2] = Archive_Find(this->arc_nickt, "forehead0.tim");
	this->arc_nickt_ptr[3] = Archive_Find(this->arc_nickt, "forehead1.tim");
	this->arc_nickt_ptr[4] = Archive_Find(this->arc_nickt, "forehead2.tim");
	this->arc_nickt_ptr[5] = Archive_Find(this->arc_nickt, "forehead3.tim");
	
	//Initialize nick t state
	Animatable_Init(&this->nickt_animatable, nickt_anim);
	Animatable_SetAnim(&this->nickt_animatable, 0);
	this->nickt_frame = this->nickt_tex_id = 0xFF; //Force art load
	
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
