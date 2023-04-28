/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "polus.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Polus background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_snow, arc_snow_ptr[12];
	IO_Data arc_people, arc_people_ptr[4];
	
	Gfx_Tex tex_back0; //back0
	Gfx_Tex tex_back1; //back1
	Gfx_Tex tex_back2; //back2
	Gfx_Tex tex_body; //body
	
	//Snow state
	Gfx_Tex tex_snow;
	u8 snow_frame, snow_tex_id;
	Animatable snow_animatable;
	
	//People state
	Gfx_Tex tex_people;
	u8 people_frame, people_tex_id;
	Animatable people_animatable;
	
	//fade stuff
	fixed_t fade, fadespd;

} Back_Polus;

//Snow animation and rects
static const CharFrame snow_frame[] = {
  {0, {  0,  0,198, 88}, {149,158}}, //0 snow 1
  {0, {  0, 88,187, 86}, {150,157}}, //1 snow 2
  {1, {  0,  0,202, 75}, {151,155}}, //2 snow 3
  {1, {  0, 75,186, 71}, {152,153}}, //3 snow 4
  {1, {  0,150,186, 73}, {152,154}}, //4 snow 5
  {2, {  0,  0,184, 74}, {152,154}}, //5 snow 6
  {2, {  0, 74,184, 72}, {152,152}}, //6 snow 7
  {2, {  0,150,183, 76}, {153,151}}, //7 snow 8
  {3, {  0,  0,183, 76}, {153,151}}, //8 snow 9
  {3, {  0, 81,185, 81}, {156,150}}, //9 snow 10
  {3, {  0,162,185, 80}, {156,149}}, //10 snow 11
  {4, {  0,  0,187, 91}, {160,155}}, //11 snow 12
  {4, {  0, 91,187, 91}, {160,155}}, //12 snow 13
  {5, {  0,  0,180, 69}, {152,160}}, //13 snow 14
  {5, {  0, 69,180, 69}, {152,160}}, //14 snow 15
  {5, {  0,138,176, 67}, {149,157}}, //15 snow 16
  {6, {  0,  0,177, 67}, {149,157}}, //16 snow 17
  {6, {  0, 67,176, 67}, {151,155}}, //17 snow 18
  {6, {  0,134,176, 67}, {151,155}}, //18 snow 19
  {7, {  0,  0,178, 72}, {152,154}}, //19 snow 20
  {7, {  0, 72,178, 72}, {152,154}}, //20 snow 21
  {7, {  0,149,181, 77}, {153,153}}, //21 snow 22
  {8, {  0,  0,182, 77}, {153,153}}, //22 snow 23
  {8, {  0, 81,186, 81}, {156,151}}, //23 snow 24
  {8, {  0,162,186, 81}, {156,151}}, //24 snow 25
  {9, {  0,  0,190, 85}, {160,150}}, //25 snow 26
  {9, {  0, 85,190, 85}, {160,150}}, //26 snow 27
  {9, {  0,170,183, 68}, {152,148}}, //27 snow 28
  {10, {  0,  0,183, 62}, {152,142}}, //28 snow 29
  {10, {  0, 70,188, 70}, {153,147}}, //29 snow 30
  {10, {  0,142,192, 72}, {154,147}}, //30 snow 31
  {11, {  0,  0,195, 74}, {154,148}}, //31 snow 32
  {11, {  0, 77,199, 77}, {153,148}}, //32 snow 33
};

static const Animation snow_anim[] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30, 30, 31, 31, 32, 32, ASCR_CHGANI, 0}}, //Idle
};

//Snow functions
void Polus_Snow_SetFrame(void *user, u8 frame)
{
	Back_Polus *this = (Back_Polus*)user;
	
	//Check if this is a new frame
	if (frame != this->snow_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &snow_frame[this->snow_frame = frame];
		if (cframe->tex != this->snow_tex_id)
			Gfx_LoadTex(&this->tex_snow, this->arc_snow_ptr[this->snow_tex_id = cframe->tex], 0);
	}
}

void Polus_Snow_Draw(Back_Polus *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &snow_frame[this->snow_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, (src.w * 3) << FIXED_SHIFT, (src.h * 3) << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 9, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_snow, &src, &dst, stage.camera.bzoom);
}

//People animation and rects
static const CharFrame people_frame[] = {
  {0, {  0,  0,252, 59}, {  0,  0}}, //0 people 1
  {0, {  0, 60,252, 59}, {  0,  0}}, //1 people 2
  {0, {  0,120,252, 59}, {  0,  0}}, //2 people 3
  {0, {  0,180,252, 59}, {  0,  0}}, //3 people 4
  {1, {  0,  0,252, 59}, {  0,  0}}, //4 people 5
  {1, {  0, 60,252, 59}, {  0,  0}}, //5 people 6
  {1, {  0,120,252, 59}, {  0,  0}}, //6 people 7
  {1, {  0,180,252, 59}, {  0,  0}}, //7 people 8
  {2, {  0,  0,252, 59}, {  0,  0}}, //8 people 9
  {2, {  0, 60,252, 59}, {  0,  0}}, //9 people 10
  {2, {  0,120,252, 59}, {  0,  0}}, //10 people 11
  {2, {  0,180,252, 59}, {  0,  0}}, //11 people 12
  {3, {  0,  0,252, 59}, {  0,  0}}, //12 people 13
  {3, {  0, 60,252, 59}, {  0,  0}}, //13 people 14
  {3, {  0,120,252, 59}, {  0,  0}}, //14 people 15
};

static const Animation people_anim[] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, ASCR_BACK, 1}}, //Idle
};

//People functions
void Polus_People_SetFrame(void *user, u8 frame)
{
	Back_Polus *this = (Back_Polus*)user;
	
	//Check if this is a new frame
	if (frame != this->people_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &people_frame[this->people_frame = frame];
		if (cframe->tex != this->people_tex_id)
			Gfx_LoadTex(&this->tex_people, this->arc_people_ptr[this->people_tex_id = cframe->tex], 0);
	}
}

void Polus_People_Draw(Back_Polus *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &people_frame[this->people_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, 526 << FIXED_SHIFT, 123 << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 10, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_people, &src, &dst, stage.camera.bzoom);
}

void Back_Polus_DrawFG(StageBack *back)
{
	Back_Polus *this = (Back_Polus*)back;

	fixed_t fx, fy;
	
	if ((stage.stage_id == StageId_Meltdown) && (stage.song_step == 1155))
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(120,1);
	}
	
	//end fade
	if ((stage.stage_id == StageId_Meltdown) && (stage.song_step == 1190))
		this->fade = 0;
	
	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
	
	if ((stage.stage_id == StageId_Meltdown) && (stage.song_step >= 1156) && (stage.song_step <= 1157))
	{
		RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	}

	//Draw body
	RECT body_src = {  0,  0,255,240};
	RECT_FIXED body_dst = {
		FIXED_DEC(-160 - screen.SCREEN_WIDEOADD2,1),
		FIXED_DEC(-120,1),
		FIXED_DEC(321 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(241,1)
	};
	
	Debug_StageMoveDebug(&body_dst, 12, 0, 0);
	if ((stage.stage_id == StageId_Meltdown) && (stage.song_step >= 1157))
		Stage_DrawTex(&this->tex_body, &body_src, &body_dst, FIXED_DEC (1,1));
	
	//Animate and draw people
	fx = (stage.camera.x / 2) * 3;
	fy = (stage.camera.y / 2) * 3;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7) == 0)
		Animatable_SetAnim(&this->people_animatable, 0);
	
	Animatable_Animate(&this->people_animatable, (void*)this, Polus_People_SetFrame);
	if (stage.stage_id == StageId_Meltdown)
		Polus_People_Draw(this, FIXED_DEC(25 + 112,1) - fx, FIXED_DEC(125 + 57,1) - fy);
	
	//Animate and draw snow
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == -29))
		Animatable_SetAnim(&this->snow_animatable, 0);
	
	Animatable_Animate(&this->snow_animatable, (void*)this, Polus_Snow_SetFrame);
	Polus_Snow_Draw(this, FIXED_DEC(-10 + 155,1) - fx, FIXED_DEC(-25 + 155,1) - fy);
}

void Back_Polus_DrawMG(StageBack *back)
{
	Back_Polus *this = (Back_Polus*)back;

	fixed_t fx, fy;
	
	//Draw bf dead
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT bfdead_src = {181,  0, 39,122};
	RECT_FIXED bfdead_dst = {
		FIXED_DEC(327 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(201,1) - fy,
		FIXED_DEC(39 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(122,1)
	};
	
	Debug_StageMoveDebug(&bfdead_dst, 11, fx, fy);
	if (stage.stage_id == StageId_Meltdown)
		Stage_DrawTexRotate(&this->tex_back2, &bfdead_src, &bfdead_dst, stage.camera.bzoom, -64);
}
void Back_Polus_DrawBG(StageBack *back)
{
	Back_Polus *this = (Back_Polus*)back;

	fixed_t fx, fy;
	
	//Draw polus
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back0_src = {  0,  0,255,223};
	RECT_FIXED back0_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(272 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(238,1)
	};
	
	RECT back1_src = {  0,  0,255,223};
	RECT_FIXED back1_dst = {
		FIXED_DEC(271 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(272 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(238,1)
	};
	
	Debug_StageMoveDebug(&back0_dst, 5, fx, fy);
	Stage_DrawTex(&this->tex_back0, &back0_src, &back0_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&back1_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back1, &back1_src, &back1_dst, stage.camera.bzoom);
	
	
	//Draw sky
	fx = stage.camera.x / 2;
	fy = stage.camera.y / 2;
	
	RECT sky0_src = {  0,  0,180,127};
	RECT_FIXED sky0_dst = {
		FIXED_DEC(-58 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-70,1) - fy,
		FIXED_DEC(238 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(168,1)
	};
	
	RECT sky1_src = {  0,128,181,127};
	RECT_FIXED sky1_dst = {
		FIXED_DEC(180 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-70,1) - fy,
		FIXED_DEC(239 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(168,1)
	};
	
	Debug_StageMoveDebug(&sky0_dst, 7, fx,fy);
	Stage_DrawTex(&this->tex_back2, &sky0_src, &sky0_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&sky1_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back2, &sky1_src, &sky1_dst, stage.camera.bzoom);
	
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 36, 25, 50);
}

void Back_Polus_Free(StageBack *back)
{
	Back_Polus *this = (Back_Polus*)back;
	
	//Free snow archive
	Mem_Free(this->arc_snow);
	
	//Free people archive
	Mem_Free(this->arc_people);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Polus_New(void)
{
	//Allocate background structure
	Back_Polus *this = (Back_Polus*)Mem_Alloc(sizeof(Back_Polus));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Polus_DrawFG;
	this->back.draw_md = Back_Polus_DrawMG;
	this->back.draw_bg = Back_Polus_DrawBG;
	this->back.free = Back_Polus_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\BG\\POLUS.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_body, Archive_Find(arc_back, "body.tim"), 0);
	Mem_Free(arc_back);
	
	//Load snow textures
	this->arc_snow = IO_Read("\\BG\\SNOW.ARC;1");
	this->arc_snow_ptr[0] = Archive_Find(this->arc_snow, "snow0.tim");
	this->arc_snow_ptr[1] = Archive_Find(this->arc_snow, "snow1.tim");
	this->arc_snow_ptr[2] = Archive_Find(this->arc_snow, "snow2.tim");
	this->arc_snow_ptr[3] = Archive_Find(this->arc_snow, "snow3.tim");
	this->arc_snow_ptr[4] = Archive_Find(this->arc_snow, "snow4.tim");
	this->arc_snow_ptr[5] = Archive_Find(this->arc_snow, "snow5.tim");
	this->arc_snow_ptr[6] = Archive_Find(this->arc_snow, "snow6.tim");
	this->arc_snow_ptr[7] = Archive_Find(this->arc_snow, "snow7.tim");
	this->arc_snow_ptr[8] = Archive_Find(this->arc_snow, "snow8.tim");
	this->arc_snow_ptr[9] = Archive_Find(this->arc_snow, "snow9.tim");
	this->arc_snow_ptr[10] = Archive_Find(this->arc_snow, "snow10.tim");
	this->arc_snow_ptr[11] = Archive_Find(this->arc_snow, "snow11.tim");
	
	//Initialize snow state
	Animatable_Init(&this->snow_animatable, snow_anim);
	Animatable_SetAnim(&this->snow_animatable, 0);
	this->snow_frame = this->snow_tex_id = 0xFF; //Force art load
	
	//Load people textures
	this->arc_people = IO_Read("\\BG\\PEOPLE.ARC;1");
	this->arc_people_ptr[0] = Archive_Find(this->arc_people, "people0.tim");
	this->arc_people_ptr[1] = Archive_Find(this->arc_people, "people1.tim");
	this->arc_people_ptr[2] = Archive_Find(this->arc_people, "people2.tim");
	this->arc_people_ptr[3] = Archive_Find(this->arc_people, "people3.tim");
	
	//Initialize people state
	Animatable_Init(&this->people_animatable, people_anim);
	Animatable_SetAnim(&this->people_animatable, 0);
	this->people_frame = this->people_tex_id = 0xFF; //Force art load
	
	//Initialize Fade
	this->fade = this->fadespd = 0;
	
	return (StageBack*)this;
}
