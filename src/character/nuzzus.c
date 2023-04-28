/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "nuzzus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Nuzzus character structure
enum
{
  Nuzzus_ArcMain_Idle0,
  Nuzzus_ArcMain_Idle1,
  Nuzzus_ArcMain_Left0,
  Nuzzus_ArcMain_Down0,
  Nuzzus_ArcMain_Up0,
  Nuzzus_ArcMain_Right0,
	
	Nuzzus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Nuzzus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Nuzzus;

//Nuzzus character definitions
static const CharFrame char_nuzzus_frame[] = {
  {Nuzzus_ArcMain_Idle0, {  0,  0,126, 79}, {140-137,142-147}}, //0 idle 1
  {Nuzzus_ArcMain_Idle0, {127,  0,128, 79}, {141-137,142-147}}, //1 idle 2
  {Nuzzus_ArcMain_Idle0, {  0, 80,128, 80}, {140-137,143-147}}, //2 idle 3
  {Nuzzus_ArcMain_Idle0, {129,161,124, 84}, {138-137,147-147}}, //3 idle 4
  {Nuzzus_ArcMain_Idle1, {  0,  0,122, 84}, {137-137,147-147}}, //4 idle 5

  {Nuzzus_ArcMain_Left0, {  0,  0,132, 79}, {150-137,142-147}}, //5 left 1
  {Nuzzus_ArcMain_Left0, {  0, 80,129, 79}, {148-137,142-147}}, //6 left 2

  {Nuzzus_ArcMain_Down0, {  0,  0,124, 81}, {139-137,144-147}}, //7 down 1
  {Nuzzus_ArcMain_Down0, {125,  0,124, 83}, {139-137,146-147}}, //8 down 2

  {Nuzzus_ArcMain_Up0, {  0,  0,119, 87}, {139-137,150-147}}, //9 up 1
  {Nuzzus_ArcMain_Up0, {120,  0,119, 85}, {139-137,148-147}}, //10 up 2

  {Nuzzus_ArcMain_Right0, {  0,  0,126, 79}, {144-137,142-147}}, //11 right 1
  {Nuzzus_ArcMain_Right0, {127,  0,127, 79}, {146-137,142-147}}, //12 right 2
};

static const Animation char_nuzzus_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Nuzzus character functions
void Char_Nuzzus_SetFrame(void *user, u8 frame)
{
	Char_Nuzzus *this = (Char_Nuzzus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_nuzzus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Nuzzus_Tick(Character *character)
{
	Char_Nuzzus *this = (Char_Nuzzus*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Nuzzus_SetFrame);
	Character_Draw(character, &this->tex, &char_nuzzus_frame[this->frame]);
}

void Char_Nuzzus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Nuzzus_Free(Character *character)
{
	Char_Nuzzus *this = (Char_Nuzzus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Nuzzus_New(fixed_t x, fixed_t y)
{
	//Allocate nuzzus object
	Char_Nuzzus *this = Mem_Alloc(sizeof(Char_Nuzzus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Nuzzus_New] Failed to allocate nuzzus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Nuzzus_Tick;
	this->character.set_anim = Char_Nuzzus_SetAnim;
	this->character.free = Char_Nuzzus_Free;
	
	Animatable_Init(&this->character.animatable, char_nuzzus_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFFFFFFF;
	
	this->character.focus_x = FIXED_DEC(271,1);
	this->character.focus_y = FIXED_DEC(169,1);
	this->character.focus_zoom = FIXED_DEC(324,1024);
	
	this->character.size = FIXED_DEC(5,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\NUZZUS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
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
