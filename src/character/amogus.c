/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "amogus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Amogus character structure
enum
{
  Amogus_ArcMain_Idle0,
  Amogus_ArcMain_Left0,
  Amogus_ArcMain_Down0,
  Amogus_ArcMain_Up0,
  Amogus_ArcMain_Right0,
	
	Amogus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Amogus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Amogus;

//Amogus character definitions
static const CharFrame char_amogus_frame[] = {
  {Amogus_ArcMain_Idle0, {  0,  0, 91,113}, {125, 52}}, //0 idle 1

  {Amogus_ArcMain_Left0, {  0,  0,126,114}, {160, 53}}, //1 left 1

  {Amogus_ArcMain_Down0, {  0,  0,126,113}, {125, 52}}, //2 down 1

  {Amogus_ArcMain_Up0, {  0,  0,106,129}, {145, 69}}, //3 up 1

  {Amogus_ArcMain_Right0, {  0,  0,139,114}, {125, 53}}, //4 right 1
};

static const Animation char_amogus_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 1, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 2, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 3, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 4, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Amogus character functions
void Char_Amogus_SetFrame(void *user, u8 frame)
{
	Char_Amogus *this = (Char_Amogus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_amogus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Amogus_Tick(Character *character)
{
	Char_Amogus *this = (Char_Amogus*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Amogus_SetFrame);
	Character_Draw(character, &this->tex, &char_amogus_frame[this->frame]);
}

void Char_Amogus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Amogus_Free(Character *character)
{
	Char_Amogus *this = (Char_Amogus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Amogus_New(fixed_t x, fixed_t y)
{
	//Allocate amogus object
	Char_Amogus *this = Mem_Alloc(sizeof(Char_Amogus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Amogus_New] Failed to allocate amogus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Amogus_Tick;
	this->character.set_anim = Char_Amogus_SetAnim;
	this->character.free = Char_Amogus_Free;
	
	Animatable_Init(&this->character.animatable, char_amogus_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFF87FB83;
	
	this->character.focus_x = FIXED_DEC(-25,1);
	this->character.focus_y = FIXED_DEC(-28,1);
	this->character.focus_zoom = FIXED_DEC(509,512);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\AMOGUS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
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
