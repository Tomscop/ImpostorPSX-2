/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "picodrip.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "speaker.h"

//Pico Drip character structure
enum
{
  PicoDrip_ArcMain_Idle0,
  PicoDrip_ArcMain_Idle1,
  PicoDrip_ArcMain_Left0,
  PicoDrip_ArcMain_Left1,
  PicoDrip_ArcMain_Down0,
  PicoDrip_ArcMain_Down1,
  PicoDrip_ArcMain_Down2,
  PicoDrip_ArcMain_Up0,
  PicoDrip_ArcMain_Up1,
  PicoDrip_ArcMain_Right0,
  PicoDrip_ArcMain_Right1,
  PicoDrip_ArcMain_Kill0,
  PicoDrip_ArcMain_Kill1,
  PicoDrip_ArcMain_Kill2,
  PicoDrip_ArcMain_Kill3,
  PicoDrip_ArcMain_Mongo0,
	
	PicoDrip_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[PicoDrip_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
	//Speaker
	Speaker speaker;
	
} Char_PicoDrip;

//PicoDrip character definitions
static const CharFrame char_picodrip_frame[] = {
  {PicoDrip_ArcMain_Idle0, {  0,  0,147,104}, {120-70,193-103}}, //0 idle 1
  {PicoDrip_ArcMain_Idle0, {  0,104,149,114}, {120-70,203-103}}, //1 idle 2
  {PicoDrip_ArcMain_Idle1, {  0,  0,134,105}, {120-70,194-103}}, //2 idle 3

  {PicoDrip_ArcMain_Left0, {  0,  0,117,116}, {139-70,205-103}}, //3 left 1
  {PicoDrip_ArcMain_Left0, {117,  0,116,116}, {138-70,205-103}}, //4 left 2
  {PicoDrip_ArcMain_Left0, {  0,116,115,116}, {137-70,204-103}}, //5 left 3
  {PicoDrip_ArcMain_Left0, {115,116,116,115}, {138-70,204-103}}, //6 left 4
  {PicoDrip_ArcMain_Left1, {  0,  0,116,115}, {138-70,204-103}}, //7 left 5

  {PicoDrip_ArcMain_Down0, {  0,  0,177, 86}, {141-70,176-103}}, //8 down 1
  {PicoDrip_ArcMain_Down0, {  0, 86,178, 86}, {141-70,176-103}}, //9 down 2
  {PicoDrip_ArcMain_Down1, {  0,  0,178, 86}, {140-70,177-103}}, //10 down 3
  {PicoDrip_ArcMain_Down1, {  0, 86,179, 86}, {141-70,177-103}}, //11 down 4
  {PicoDrip_ArcMain_Down2, {  0,  0,178, 86}, {140-70,177-103}}, //12 down 5

  {PicoDrip_ArcMain_Up0, {  0,  0,118,102}, {121-70,188-103}}, //13 up 1
  {PicoDrip_ArcMain_Up0, {118,  0,117,102}, {119-70,188-103}}, //14 up 2
  {PicoDrip_ArcMain_Up0, {  0,102,115,100}, {119-70,187-103}}, //15 up 3
  {PicoDrip_ArcMain_Up0, {115,102,118,100}, {122-70,187-103}}, //16 up 4
  {PicoDrip_ArcMain_Up1, {  0,  0,116,101}, {119-70,187-103}}, //17 up 5

  {PicoDrip_ArcMain_Right0, {  0,  0,123,117}, {120-70,206-103}}, //18 right 1
  {PicoDrip_ArcMain_Right0, {123,  0,122,117}, {119-70,206-103}}, //19 right 2
  {PicoDrip_ArcMain_Right0, {  0,117,123,114}, {119-70,203-103}}, //20 right 3
  {PicoDrip_ArcMain_Right0, {123,117,124,114}, {121-70,203-103}}, //21 right 4
  {PicoDrip_ArcMain_Right1, {  0,  0,123,114}, {120-70,203-103}}, //22 right 5

  {PicoDrip_ArcMain_Kill0, {  0,  0,209,164}, {232-70,263-103}}, //23 kill 1
  {PicoDrip_ArcMain_Kill1, {  0,  0,158,110}, {181-70,197-103}}, //24 kill 2
  {PicoDrip_ArcMain_Kill1, {  0,110,163, 85}, {141-70,180-103}}, //25 kill 3
  {PicoDrip_ArcMain_Kill2, {  0,  0,127, 85}, {141-70,181-103}}, //26 kill 4
  {PicoDrip_ArcMain_Kill2, {127,  0,110,102}, {130-70,198-103}}, //27 kill 5
  {PicoDrip_ArcMain_Kill2, {  0,102,109,103}, {130-70,198-103}}, //28 kill 6
  {PicoDrip_ArcMain_Kill2, {109,102,113,103}, {131-70,197-103}}, //29 kill 7
  {PicoDrip_ArcMain_Kill3, {  0,  0,114,103}, {132-70,198-103}}, //30 kill 8

  {PicoDrip_ArcMain_Mongo0, {  0,  0, 95, 26}, {118-70,126-103}}, //31 mongo 1
  {PicoDrip_ArcMain_Mongo0, { 95,  0, 94, 33}, {117-70,133-103}}, //32 mongo 2
  {PicoDrip_ArcMain_Mongo0, {  0, 33, 95, 33}, {118-70,133-103}}, //33 mongo 3
  {PicoDrip_ArcMain_Mongo0, { 95, 33, 95, 31}, {118-70,131-103}}, //34 mongo 4
  {PicoDrip_ArcMain_Mongo0, {  0, 66, 95, 31}, {118-70,131-103}}, //35 mongo 5
  {PicoDrip_ArcMain_Mongo0, { 95, 66, 95, 31}, {118-70,131-103}}, //36 mongo 6
};

static const Animation char_picodrip_anim[CharAnim_Max] = {
	{1, (const u8[]){ 31, 31, 32, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 35, 36, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, ASCR_BACK, 1}},                        //CharAnim_Idle
	{1, (const u8[]){ 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 7, 5, 5, 5, 5, 5, ASCR_BACK, 1}},                                 //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_LeftAlt
	{1, (const u8[]){ 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 12, 10, 10, 10, 10, 10, 10, 10, 10, 10, ASCR_BACK, 1}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                      //CharAnim_DownAlt
	{1, (const u8[]){ 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 17, 15, 15, 15, 15, 15, 15, 15, 15, 15, ASCR_BACK, 1}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                 //CharAnim_UpAlt
	{1, (const u8[]){ 18, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 22, ASCR_BACK, 1}},                                 //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_RightAlt
	
	{1, (const u8[]){ 23, 23, 24, 25, 25, 26, 27, 28, 29, 29, 30, 30, 30, ASCR_BACK, 1}}, //CharAnim_Special1
};

static const Animation char_picodrip_anim2[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 0, 1, 2, 0, 1, 2, ASCR_CHGANI, CharAnim_Idle}},                        //CharAnim_Idle
	{1, (const u8[]){ 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 7, 5, 5, 5, 5, 5, ASCR_BACK, 1}},                                 //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_LeftAlt
	{1, (const u8[]){ 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 12, 10, 10, 10, 10, 10, 10, 10, 10, 10, ASCR_BACK, 1}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                      //CharAnim_DownAlt
	{1, (const u8[]){ 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 17, 15, 15, 15, 15, 15, 15, 15, 15, 15, ASCR_BACK, 1}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                                 //CharAnim_UpAlt
	{1, (const u8[]){ 18, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 22, ASCR_BACK, 1}},                                 //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_RightAlt
	
	{1, (const u8[]){ 23, 23, 24, 25, 25, 26, 27, 28, 29, 29, 30, 30, 30, ASCR_BACK, 1}}, //CharAnim_Special1
};

//PicoDrip character functions
void Char_PicoDrip_SetFrame(void *user, u8 frame)
{
	Char_PicoDrip *this = (Char_PicoDrip*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_picodrip_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_PicoDrip_Tick(Character *character)
{
	Char_PicoDrip *this = (Char_PicoDrip*)character;
	
	//Initialize Pico test
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{			
		//Perform dance
	    if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
		{	
			//Bump speakers
			Speaker_Bump(&this->speaker);
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
			 character->animatable.anim != CharAnim_Special2 &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}

	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Drippypop: //Kill
				if (stage.song_step == 1137)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	if (stage.song_step >= 1138)
		Animatable_Init(&this->character.animatable, char_picodrip_anim2);
	else
		Animatable_Init(&this->character.animatable, char_picodrip_anim);
		
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_PicoDrip_SetFrame);
	if (stage.lights != 1)
		Character_DrawParallax(character, &this->tex, &char_picodrip_frame[this->frame], parallax);
	
	//Tick speakers
	Speaker_Tick(&this->speaker, character->x, character->y, parallax);
}

void Char_PicoDrip_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_PicoDrip_Free(Character *character)
{
	Char_PicoDrip *this = (Char_PicoDrip*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_PicoDrip_New(fixed_t x, fixed_t y)
{
	//Allocate picodrip object
	Char_PicoDrip *this = Mem_Alloc(sizeof(Char_PicoDrip));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_PicoDrip_New] Failed to allocate picodrip object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_PicoDrip_Tick;
	this->character.set_anim = Char_PicoDrip_SetAnim;
	this->character.free = Char_PicoDrip_Free;
	
	Animatable_Init(&this->character.animatable, char_picodrip_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFA5004A;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\PICODRIP.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "kill0.tim",
  "kill1.tim",
  "kill2.tim",
  "kill3.tim",
  "mongo0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize speaker
	Speaker_Init(&this->speaker);
	
	return (Character*)this;
}
