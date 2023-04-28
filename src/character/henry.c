/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henry.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Henry character structure
enum
{
  Henry_ArcMain_Idle0,
  Henry_ArcMain_Idle1,
  Henry_ArcMain_Idle2,
  Henry_ArcMain_Left0,
  Henry_ArcMain_Left1,
  Henry_ArcMain_Left2,
  Henry_ArcMain_Down0,
  Henry_ArcMain_Down1,
  Henry_ArcMain_Up0,
  Henry_ArcMain_Right0,
	
	Henry_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Henry_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Henry;

//Henry character definitions
static const CharFrame char_henry_frame[] = {
  {Henry_ArcMain_Idle0, {  0,  0, 90,173}, {53,144}}, //0 idle 1
  {Henry_ArcMain_Idle0, { 92,  0, 90,174}, {53,145}}, //1 idle 2
  {Henry_ArcMain_Idle1, {  0,  0, 88,176}, {52,147}}, //2 idle 3
  {Henry_ArcMain_Idle1, { 90,  0, 85,181}, {52,152}}, //3 idle 4
  {Henry_ArcMain_Idle2, {  0,  0, 85,181}, {52,152}}, //4 idle 5

  {Henry_ArcMain_Left0, {  0,  0,132,176}, {97,145}}, //5 left 1
  {Henry_ArcMain_Left1, {  0,  0,129,175}, {93,144}}, //6 left 2
  {Henry_ArcMain_Left2, {  0,  0,128,174}, {92,144}}, //7 left 3

  {Henry_ArcMain_Down0, {  0,  0,139,143}, {94,111}}, //8 down 1
  {Henry_ArcMain_Down1, {  0,  0,140,145}, {94,113}}, //9 down 2

  {Henry_ArcMain_Up0, {  0,  0,104,180}, {65,150}}, //10 up 1
  {Henry_ArcMain_Up0, {106,  0,101,177}, {65,147}}, //11 up 2

  {Henry_ArcMain_Right0, {  0,  0,122,162}, {63,134}}, //12 right 1
  {Henry_ArcMain_Right0, {124,  0,118,162}, {61,134}}, //13 right 2
};

static const Animation char_henry_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6,  7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Henry character functions
void Char_Henry_SetFrame(void *user, u8 frame)
{
	Char_Henry *this = (Char_Henry*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_henry_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Henry_Tick(Character *character)
{
	Char_Henry *this = (Char_Henry*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Henry_SetFrame);
	Character_Draw(character, &this->tex, &char_henry_frame[this->frame]);
}

void Char_Henry_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Henry_Free(Character *character)
{
	Char_Henry *this = (Char_Henry*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Henry_New(fixed_t x, fixed_t y)
{
	//Allocate henry object
	Char_Henry *this = Mem_Alloc(sizeof(Char_Henry));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Henry_New] Failed to allocate henry object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Henry_Tick;
	this->character.set_anim = Char_Henry_SetAnim;
	this->character.free = Char_Henry_Free;
	
	Animatable_Init(&this->character.animatable, char_henry_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFFBDD7D8;
	
	this->character.focus_x = FIXED_DEC(5,1);
	this->character.focus_y = FIXED_DEC(-42,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\HENRY.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
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
