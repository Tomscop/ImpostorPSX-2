/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "clowfoe.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Clowfoe player types
enum
{
  Clowfoe_ArcMain_Idle0,
  Clowfoe_ArcMain_Left0,
  Clowfoe_ArcMain_Down0,
  Clowfoe_ArcMain_Up0,
  Clowfoe_ArcMain_Right0,
	
	Clowfoe_ArcMain_Max,
};

#define Clowfoe_Arc_Max Clowfoe_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Clowfoe_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_Clowfoe;

//Clowfoe player definitions
static const CharFrame char_clowfoe_frame[] = {
  {Clowfoe_ArcMain_Idle0, {  0,  0, 71, 87}, {150,143}}, //0 idle 1
  {Clowfoe_ArcMain_Idle0, { 71,  0, 69, 89}, {155,145}}, //1 idle 2
  {Clowfoe_ArcMain_Idle0, {140,  0, 70, 87}, {151,143}}, //2 idle 3
  {Clowfoe_ArcMain_Idle0, {  0, 89, 71, 88}, {149,144}}, //3 idle 4
  {Clowfoe_ArcMain_Idle0, { 71, 89, 71, 88}, {149,144}}, //4 idle 5
  {Clowfoe_ArcMain_Idle0, {142, 89, 71, 89}, {149,144}}, //5 idle 6

  {Clowfoe_ArcMain_Left0, {  0,  0, 76, 85}, {165+5,141-1}}, //6 left 1
  {Clowfoe_ArcMain_Left0, { 76,  0, 75, 84}, {161+5,140-1}}, //7 left 2

  {Clowfoe_ArcMain_Down0, {  0,  0, 74, 74}, {165+7,133-4}}, //8 down 1
  {Clowfoe_ArcMain_Down0, { 74,  0, 72, 76}, {161+7,135-4}}, //9 down 2

  {Clowfoe_ArcMain_Up0, {  0,  0, 80, 95}, {148-3,150+1}}, //10 up 1
  {Clowfoe_ArcMain_Up0, { 80,  0, 79, 93}, {148-3,148+1}}, //11 up 2

  {Clowfoe_ArcMain_Right0, {  0,  0, 82, 85}, {151-2,142-1}}, //12 right 1
  {Clowfoe_ArcMain_Right0, { 82,  0, 82, 84}, {152-2,140-1}}, //13 right 2
};

static const Animation char_clowfoe_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 7, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 9, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 11, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 12, 13, 13, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

//Clowfoe player functions
void Char_Clowfoe_SetFrame(void *user, u8 frame)
{
	Char_Clowfoe *this = (Char_Clowfoe*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_clowfoe_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Clowfoe_Tick(Character *character)
{
	Char_Clowfoe *this = (Char_Clowfoe*)character;
	
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
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_Clowfoe_SetFrame);
	Character_Draw(character, &this->tex, &char_clowfoe_frame[this->frame]);
}

void Char_Clowfoe_SetAnim(Character *character, u8 anim)
{
	Char_Clowfoe *this = (Char_Clowfoe*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Clowfoe_Free(Character *character)
{
	Char_Clowfoe *this = (Char_Clowfoe*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Clowfoe_New(fixed_t x, fixed_t y)
{
	//Allocate clowfoe object
	Char_Clowfoe *this = Mem_Alloc(sizeof(Char_Clowfoe));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Clowfoe_New] Failed to allocate clowfoe object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Clowfoe_Tick;
	this->character.set_anim = Char_Clowfoe_SetAnim;
	this->character.free = Char_Clowfoe_Free;
	
	Animatable_Init(&this->character.animatable, char_clowfoe_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;
	
	//health bar color
	this->character.health_bar = 0xFFFFFFFF;
	
	this->character.focus_x = FIXED_DEC(-167,1);
	this->character.focus_y = FIXED_DEC(205,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\CLOWFOE.ARC;1");
		
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
