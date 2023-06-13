/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "loggoma.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//LoggoMA character structure
enum
{
  LoggoMA_ArcMain_Idle0,
  LoggoMA_ArcMain_Idle1,
	
	LoggoMA_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[LoggoMA_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_LoggoMA;

//LoggoMA character definitions
static const CharFrame char_loggoma_frame[] = {
  {LoggoMA_ArcMain_Idle0, {  0,  0, 93, 69}, {130,155}}, //0 idle 1
  {LoggoMA_ArcMain_Idle0, { 93,  0, 93, 69}, {130,155}}, //1 idle 2
  {LoggoMA_ArcMain_Idle0, {  0, 69, 96, 67}, {132,153}}, //2 idle 3
  {LoggoMA_ArcMain_Idle0, { 96, 69, 96, 67}, {131,153}}, //3 idle 4
  {LoggoMA_ArcMain_Idle0, {  0,138, 94, 72}, {130,158}}, //4 idle 5
  {LoggoMA_ArcMain_Idle0, { 94,138, 94, 72}, {130,158}}, //5 idle 6
  {LoggoMA_ArcMain_Idle1, {  0,  0, 92, 73}, {128,159}}, //6 idle 7
  {LoggoMA_ArcMain_Idle1, { 92,  0, 93, 73}, {129,159}}, //7 idle 8
  {LoggoMA_ArcMain_Idle1, {  0, 73, 92, 74}, {129,160}}, //8 idle 9
  {LoggoMA_ArcMain_Idle1, { 92, 73, 92, 74}, {129,160}}, //9 idle 10
};

static const Animation char_loggoma_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ASCR_BACK, 1}}, //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//LoggoMA character functions
void Char_LoggoMA_SetFrame(void *user, u8 frame)
{
	Char_LoggoMA *this = (Char_LoggoMA*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_loggoma_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_LoggoMA_Tick(Character *character)
{
	Char_LoggoMA *this = (Char_LoggoMA*)character;
	
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
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt) &&
			(stage.song_step & 0x3) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_LoggoMA_SetFrame);
	Character_Draw(character, &this->tex, &char_loggoma_frame[this->frame]);
}

void Char_LoggoMA_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_LoggoMA_Free(Character *character)
{
	Char_LoggoMA *this = (Char_LoggoMA*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_LoggoMA_New(fixed_t x, fixed_t y)
{
	//Allocate loggoma object
	Char_LoggoMA *this = Mem_Alloc(sizeof(Char_LoggoMA));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_LoggoMA_New] Failed to allocate loggoma object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_LoggoMA_Tick;
	this->character.set_anim = Char_LoggoMA_SetAnim;
	this->character.free = Char_LoggoMA_Free;
	
	Animatable_Init(&this->character.animatable, char_loggoma_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\GF\\LOGGOMA.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
