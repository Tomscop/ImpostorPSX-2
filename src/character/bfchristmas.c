/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfchristmas.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Christmas player types
enum
{
	BFChristmas_ArcMain_Idle0,
	BFChristmas_ArcMain_Idle1,
	BFChristmas_ArcMain_Idle2,
	BFChristmas_ArcMain_Idle3,
	BFChristmas_ArcMain_Left0,
	BFChristmas_ArcMain_Left1,
	BFChristmas_ArcMain_Down,
	BFChristmas_ArcMain_Up,
	BFChristmas_ArcMain_Right0,
	BFChristmas_ArcMain_Right1,
	BFChristmas_ArcMain_LeftM0,
	BFChristmas_ArcMain_LeftM1,
	BFChristmas_ArcMain_DownM,
	BFChristmas_ArcMain_UpM,
	BFChristmas_ArcMain_RightM0,
	BFChristmas_ArcMain_RightM1,
	
	BFChristmas_ArcMain_Max,
};

#define BFChristmas_Arc_Max BFChristmas_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFChristmas_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFChristmas;

//Boyfriend Christmas player definitions
static const CharFrame char_bfchristmas_frame[] = {
	{BFChristmas_ArcMain_Idle0, {  0,  0,133,132}, {  0,  0}}, //0 idle 1
	{BFChristmas_ArcMain_Idle1, {  0,  0,132,133}, {  0,  1}}, //1 idle 2
	{BFChristmas_ArcMain_Idle2, {  0,  0,132,137}, {  1,  5}}, //2 idle 3
	{BFChristmas_ArcMain_Idle3, {  0,  0,130,139}, {  1,  7}}, //3 idle 4
	{BFChristmas_ArcMain_Idle3, {  0,  0,130,139}, {  1,  7}}, //4 idle 5
	
	{BFChristmas_ArcMain_Left0, {  0,  0,136,139}, {  8,  7}}, //5 left 1
	{BFChristmas_ArcMain_Left1, {  0,  0,134,139}, {  6,  7}}, //6 left 2
	
	{BFChristmas_ArcMain_Down, {  0,  0,144,125}, {  2, -7}}, //7 down 1
	{BFChristmas_ArcMain_Down, {  0,126,142,128}, {  4, -4}}, //8 down 2
	
	{BFChristmas_ArcMain_Up, {  0,  0,118,153}, { -7, 20}}, //9 up 1
	{BFChristmas_ArcMain_Up, {119,  0,123,150}, { -4, 17}}, //10 up 2
	
	{BFChristmas_ArcMain_Right0, {  0,  0,132,137}, { -4,  4}}, //11 right 1
	{BFChristmas_ArcMain_Right1, {  0,  0,129,137}, { -3,  4}}, //12 right 2
	
	{BFChristmas_ArcMain_LeftM0, {  0,  0,136,139}, {  8,  7}}, //13 left miss 1
	{BFChristmas_ArcMain_LeftM1, {  0,  0,134,139}, {  6,  7}}, //14 left miss 2
	
	{BFChristmas_ArcMain_DownM, {  0,  0,144,125}, {  2, -7}}, //15 down miss 1
	{BFChristmas_ArcMain_DownM, {  0,126,142,128}, {  4, -4}}, //16 down miss 2
	
	{BFChristmas_ArcMain_UpM, {  0,  0,118,153}, { -7, 20}}, //17 up miss 1
	{BFChristmas_ArcMain_UpM, {119,  0,123,150}, { -4, 17}}, //18 up miss 2
	
	{BFChristmas_ArcMain_RightM0, {  0,  0,132,137}, { -4,  4}}, //19 right miss 1
	{BFChristmas_ArcMain_RightM1, {  0,  0,129,137}, { -3,  4}}, //20 right miss 2
};

static const Animation char_bfchristmas_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, 6, 6, 6, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, 8, 8, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, 10, 10, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, 12, 12, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 14, 14, 14, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 15, 16, 16, 16, 16, 16, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 17, 18, 18, 18, 18, 18, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 19, 20, 20, 20, 20, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Christmas player functions
void Char_BFChristmas_SetFrame(void *user, u8 frame)
{
	Char_BFChristmas *this = (Char_BFChristmas*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfchristmas_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFChristmas_Tick(Character *character)
{
	Char_BFChristmas *this = (Char_BFChristmas*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Spookpostor)
	{
		if (stage.song_step == 70)
		{
			this->character.focus_x = FIXED_DEC(-8,1);
			this->character.focus_y = FIXED_DEC(4,1);	
			this->character.focus_zoom = FIXED_DEC(941,1024);
		}
	}
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (stage.stage_id != StageId_Spookpostor)
		{
			if (Animatable_Ended(&character->animatable) &&
				(character->animatable.anim != CharAnim_Left &&
				character->animatable.anim != CharAnim_LeftAlt &&
				character->animatable.anim != PlayerAnim_LeftMiss &&
				character->animatable.anim != CharAnim_Down &&
				character->animatable.anim != CharAnim_DownAlt &&
				character->animatable.anim != PlayerAnim_DownMiss &&
				character->animatable.anim != CharAnim_Up &&
				character->animatable.anim != CharAnim_UpAlt &&
				character->animatable.anim != PlayerAnim_UpMiss &&
				character->animatable.anim != CharAnim_Right &&
				character->animatable.anim != CharAnim_RightAlt &&
				character->animatable.anim != PlayerAnim_RightMiss) &&
				(stage.song_step & 0x7) == 0)
				character->set_anim(character, CharAnim_Idle);
		}
		else
		{
			if (Animatable_Ended(&character->animatable) &&
				(character->animatable.anim != CharAnim_Left &&
				character->animatable.anim != CharAnim_LeftAlt &&
				character->animatable.anim != PlayerAnim_LeftMiss &&
				character->animatable.anim != CharAnim_Down &&
				character->animatable.anim != CharAnim_DownAlt &&
				character->animatable.anim != PlayerAnim_DownMiss &&
				character->animatable.anim != CharAnim_Up &&
				character->animatable.anim != CharAnim_UpAlt &&
				character->animatable.anim != PlayerAnim_UpMiss &&
				character->animatable.anim != CharAnim_Right &&
				character->animatable.anim != CharAnim_RightAlt &&
				character->animatable.anim != PlayerAnim_RightMiss) &&
				(stage.song_step & 0xF) == 0)
				character->set_anim(character, CharAnim_Idle);
		}
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFChristmas_SetFrame);
	Character_Draw(character, &this->tex, &char_bfchristmas_frame[this->frame]);
}

void Char_BFChristmas_SetAnim(Character *character, u8 anim)
{
	Char_BFChristmas *this = (Char_BFChristmas*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFChristmas_Free(Character *character)
{
	Char_BFChristmas *this = (Char_BFChristmas*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFChristmas_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend christmas object
	Char_BFChristmas *this = Mem_Alloc(sizeof(Char_BFChristmas));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFChristmas_New] Failed to allocate boyfriend christmas object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFChristmas_Tick;
	this->character.set_anim = Char_BFChristmas_SetAnim;
	this->character.free = Char_BFChristmas_Free;
	
	Animatable_Init(&this->character.animatable, char_bfchristmas_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFF31B0D1;
	
	if (stage.stage_id == StageId_Christmas)
	{
	this->character.focus_x = FIXED_DEC(-1,1);
	this->character.focus_y = FIXED_DEC(0,1);
	this->character.focus_zoom = FIXED_DEC(730,1024);
	}
	if (stage.stage_id == StageId_Spookpostor)
	{
	this->character.focus_x = FIXED_DEC(-13,1);
	this->character.focus_y = FIXED_DEC(7,1);
	this->character.focus_zoom = FIXED_DEC(2794,1024);
	}
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFCHRIST.ARC;1");
		
	const char **pathp = (const char *[]){
		"idle0.tim",   //BFChristmas_ArcMain_Idle0
		"idle1.tim",   //BFChristmas_ArcMain_Idle1
		"idle2.tim",   //BFChristmas_ArcMain_Idle2
		"idle3.tim",   //BFChristmas_ArcMain_Idle3
		"left0.tim",   //BFChristmas_ArcMain_Left0
		"left1.tim",   //BFChristmas_ArcMain_Left1
		"down.tim",   //BFChristmas_ArcMain_Down
		"up.tim",   //BFChristmas_ArcMain_Up
		"right0.tim",   //BFChristmas_ArcMain_Right0
		"right1.tim",   //BFChristmas_ArcMain_Right1
		"leftm0.tim",   //BFChristmas_ArcMain_LeftM0
		"leftm1.tim",   //BFChristmas_ArcMain_LeftM1
		"downm.tim",   //BFChristmas_ArcMain_DownM
		"upm.tim",   //BFChristmas_ArcMain_UpM
		"rightm0.tim",   //BFChristmas_ArcMain_RightM0
		"rightm1.tim",   //BFChristmas_ArcMain_RightM1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
