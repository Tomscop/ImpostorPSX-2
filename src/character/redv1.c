/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "redv1.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Red Ow character structure
enum
{
  RedV1_ArcMain_Idle0,
  RedV1_ArcMain_Idle1,
  RedV1_ArcMain_Idle2,
  RedV1_ArcMain_Idle3,
  RedV1_ArcMain_Left0,
  RedV1_ArcMain_Down0,
  RedV1_ArcMain_Up0,
  RedV1_ArcMain_Right0,
	
	RedV1_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RedV1_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RedV1;

//Red Ow character definitions
static const CharFrame char_redv1_frame[] = {
  {RedV1_ArcMain_Idle0, {  0,  0,190,112}, {214, 92}}, //0 idle 1
  {RedV1_ArcMain_Idle0, {  0,113,201,108}, {217, 91}}, //1 idle 2
  {RedV1_ArcMain_Idle1, {  0,  0,220, 94}, {231, 85}}, //2 idle 3
  {RedV1_ArcMain_Idle1, {  0, 95,227, 92}, {235, 82}}, //3 idle 4
  {RedV1_ArcMain_Idle2, {  0,  0,187,104}, {208, 88}}, //4 idle 5
  {RedV1_ArcMain_Idle2, {  0,105,185,112}, {211, 92}}, //5 idle 6
  {RedV1_ArcMain_Idle3, {  0,  0,191,111}, {215, 92}}, //6 idle 7

  {RedV1_ArcMain_Left0, {  0,  0,215,100}, {234, 92}}, //7 left 1
  {RedV1_ArcMain_Left0, {  0,101,214,100}, {231, 92}}, //8 left 2

  {RedV1_ArcMain_Down0, {  0,  0,212, 90}, {231, 84}}, //9 down 1
  {RedV1_ArcMain_Down0, {  0, 91,208, 85}, {225, 79}}, //10 down 2

  {RedV1_ArcMain_Up0, {  0,  0,160,116}, {186,109}}, //11 up 1
  {RedV1_ArcMain_Up0, {  0,117,159,110}, {184,103}}, //12 up 2

  {RedV1_ArcMain_Right0, {  0,  0,166,100}, {207, 96}}, //13 right 1
  {RedV1_ArcMain_Right0, {  0,101,161,100}, {208, 96}}, //14 right 2
};

static const Animation char_redv1_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 13, 14, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Red Ow character functions
void Char_RedV1_SetFrame(void *user, u8 frame)
{
	Char_RedV1 *this = (Char_RedV1*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_redv1_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RedV1_Tick(Character *character)
{
	Char_RedV1 *this = (Char_RedV1*)character;
	
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
		if ((character->animatable.anim != CharAnim_Left &&
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
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_RedV1_SetFrame);
	Character_Draw(character, &this->tex, &char_redv1_frame[this->frame]);
}

void Char_RedV1_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RedV1_Free(Character *character)
{
	Char_RedV1 *this = (Char_RedV1*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RedV1_New(fixed_t x, fixed_t y)
{
	//Allocate redv1 object
	Char_RedV1 *this = Mem_Alloc(sizeof(Char_RedV1));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RedV1_New] Failed to allocate redv1 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RedV1_Tick;
	this->character.set_anim = Char_RedV1_SetAnim;
	this->character.free = Char_RedV1_Free;
	
	Animatable_Init(&this->character.animatable, char_redv1_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFFF0000;
	
	this->character.focus_x = FIXED_DEC(-85,1);
	this->character.focus_y = FIXED_DEC(-60,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\REDV1.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
