/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jermaroom.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Jerma Room background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_aaaaaa, arc_aaaaaa_ptr[8];
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	
	//AAAAAA state
	Gfx_Tex tex_aaaaaa;
	u8 aaaaaa_frame, aaaaaa_tex_id;
	Animatable aaaaaa_animatable;

} Back_JermaRoom;

//AAAAAA animation and rects
static const CharFrame aaaaaa_frame[] = {
  {0, {  0,  0,  4, 20}, { 43, 81}}, //0 aaaaaa 1
  {0, {  4,  0, 32, 60}, { 71,121}}, //1 aaaaaa 2
  {0, { 36,  0, 43, 63}, { 78,125}}, //2 aaaaaa 3
  {0, { 79,  0, 35, 62}, { 74,123}}, //3 aaaaaa 4
  {0, {114,  0, 29, 63}, { 68,124}}, //4 aaaaaa 5
  {0, {143,  0, 28, 65}, { 67,126}}, //5 aaaaaa 6
  {0, {171,  0, 39, 67}, { 78,129}}, //6 aaaaaa 7
  {0, {210,  0, 45, 66}, { 94,128}}, //7 aaaaaa 8
  {0, {  0, 67, 49, 66}, {101,127}}, //8 aaaaaa 9
  {0, { 49, 67, 87, 73}, {135,134}}, //9 aaaaaa 10
  {0, {136, 67, 93, 82}, {135,144}}, //10 aaaaaa 11
  {0, {  0,149,121, 95}, {160,156}}, //11 aaaaaa 12
  {0, {121,149,121, 99}, {160,160}}, //12 aaaaaa 13
  {1, {  0,  0,121, 98}, {160,159}}, //13 aaaaaa 14
  {1, {121,  0,121, 98}, {160,160}}, //14 aaaaaa 15
  {1, {  0, 98,121, 94}, {160,156}}, //15 aaaaaa 16
  {1, {121, 98,121, 94}, {160,155}}, //16 aaaaaa 17
  {2, {  0,  0,122, 95}, {160,156}}, //17 aaaaaa 18
  {2, {122,  0,122, 94}, {160,156}}, //18 aaaaaa 19
  {2, {  0, 95,122, 94}, {160,155}}, //19 aaaaaa 20
  {2, {122, 95,122, 95}, {160,156}}, //20 aaaaaa 21
  {3, {  0,  0,122, 95}, {160,156}}, //21 aaaaaa 22
  {3, {122,  0,122, 94}, {160,156}}, //22 aaaaaa 23
  {3, {  0, 95,122, 94}, {160,156}}, //23 aaaaaa 24
  {3, {122, 95,121, 95}, {160,156}}, //24 aaaaaa 25
  {4, {  0,  0,121, 95}, {160,156}}, //25 aaaaaa 26
  {4, {121,  0,121, 94}, {160,156}}, //26 aaaaaa 27
  {4, {  0, 95,121, 94}, {160,155}}, //27 aaaaaa 28
  {4, {121, 95,121, 95}, {160,156}}, //28 aaaaaa 29
  {5, {  0,  0,121, 95}, {160,156}}, //29 aaaaaa 30
  {5, {121,  0,121, 94}, {160,156}}, //30 aaaaaa 31
  {5, {  0, 95,121, 94}, {160,156}}, //31 aaaaaa 32
  {5, {121, 95,121, 95}, {160,156}}, //32 aaaaaa 33
  {6, {  0,  0,121, 95}, {160,156}}, //33 aaaaaa 34
  {6, {121,  0,121, 94}, {160,156}}, //34 aaaaaa 35
  {6, {  0, 95,121, 94}, {160,155}}, //35 aaaaaa 36
  {6, {121, 95,121, 95}, {160,156}}, //36 aaaaaa 37
  {7, {  0,  0,121, 95}, {160,156}}, //37 aaaaaa 38
  {7, {121,  0,121, 94}, {160,156}}, //38 aaaaaa 39
  {7, {  0,  0,  1,  1}, {160,156}}, //39 hide
};

static const Animation aaaaaa_anim[] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, ASCR_CHGANI, 1}}, //Idle
	{1, (const u8[]){ 39, ASCR_BACK, 1}}, //Hide
};

//AAAAAA functions
void JermaRoom_AAAAAA_SetFrame(void *user, u8 frame)
{
	Back_JermaRoom *this = (Back_JermaRoom*)user;
	
	//Check if this is a new frame
	if (frame != this->aaaaaa_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &aaaaaa_frame[this->aaaaaa_frame = frame];
		if (cframe->tex != this->aaaaaa_tex_id)
			Gfx_LoadTex(&this->tex_aaaaaa, this->arc_aaaaaa_ptr[this->aaaaaa_tex_id = cframe->tex], 0);
	}
}

void JermaRoom_AAAAAA_Draw(Back_JermaRoom *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &aaaaaa_frame[this->aaaaaa_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w * FIXED_DEC(2,1), src.h * FIXED_DEC(2,1)};
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_aaaaaa, &src, &dst, stage.camera.bzoom);
}

void Back_JermaRoom_DrawFG(StageBack *back)
{
	Back_JermaRoom *this = (Back_JermaRoom*)back;
	
	//Animate and draw aaaaaa
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == -29))
		Animatable_SetAnim(&this->aaaaaa_animatable, 1);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 997))
		Animatable_SetAnim(&this->aaaaaa_animatable, 0);
	
	Animatable_Animate(&this->aaaaaa_animatable, (void*)this, JermaRoom_AAAAAA_SetFrame);
	if ((stage.song_step >= 996) && (stage.song_step <= 1023))
		JermaRoom_AAAAAA_Draw(this, FIXED_DEC(34,1), FIXED_DEC(20+52,1));
}

void Back_JermaRoom_DrawBG(StageBack *back)
{
	Back_JermaRoom *this = (Back_JermaRoom*)back;

	fixed_t fx, fy;

	//Draw jerma room
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,255};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(265 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(265,1)
	};
	
	RECT back1_src = {  0,  0,140,255};
	RECT_FIXED back1_dst = {
		FIXED_DEC(264 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(145 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(265,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
}

void Back_JermaRoom_Free(StageBack *back)
{
	Back_JermaRoom *this = (Back_JermaRoom*)back;
	
	//Free aaaaaa archive
	Mem_Free(this->arc_aaaaaa);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_JermaRoom_New(void)
{
	//Allocate background structure
	Back_JermaRoom *this = (Back_JermaRoom*)Mem_Alloc(sizeof(Back_JermaRoom));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_JermaRoom_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_JermaRoom_DrawBG;
	this->back.free = Back_JermaRoom_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\JERMA.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	//Load aaaaaa textures
	this->arc_aaaaaa = IO_Read("\\BG\\AAAAAA.ARC;1");
	this->arc_aaaaaa_ptr[0] = Archive_Find(this->arc_aaaaaa, "aaaaaa0.tim");
	this->arc_aaaaaa_ptr[1] = Archive_Find(this->arc_aaaaaa, "aaaaaa1.tim");
	this->arc_aaaaaa_ptr[2] = Archive_Find(this->arc_aaaaaa, "aaaaaa2.tim");
	this->arc_aaaaaa_ptr[3] = Archive_Find(this->arc_aaaaaa, "aaaaaa3.tim");
	this->arc_aaaaaa_ptr[4] = Archive_Find(this->arc_aaaaaa, "aaaaaa4.tim");
	this->arc_aaaaaa_ptr[5] = Archive_Find(this->arc_aaaaaa, "aaaaaa5.tim");
	this->arc_aaaaaa_ptr[6] = Archive_Find(this->arc_aaaaaa, "aaaaaa6.tim");
	this->arc_aaaaaa_ptr[7] = Archive_Find(this->arc_aaaaaa, "aaaaaa7.tim");
	
	//Initialize aaaaaa state
	Animatable_Init(&this->aaaaaa_animatable, aaaaaa_anim);
	Animatable_SetAnim(&this->aaaaaa_animatable, 0);
	this->aaaaaa_frame = this->aaaaaa_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
