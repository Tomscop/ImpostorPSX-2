/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "kid.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Kid player types
enum
{
  Kid_ArcMain_Idle0,
  Kid_ArcMain_Idle1,
  Kid_ArcMain_Idle2,
  Kid_ArcMain_Left0,
  Kid_ArcMain_Left1,
  Kid_ArcMain_Down0,
  Kid_ArcMain_Down1,
  Kid_ArcMain_Up0,
  Kid_ArcMain_Up1,
  Kid_ArcMain_Right0,
  Kid_ArcMain_Right1,
	
	Kid_ArcMain_Max,
};

#define Kid_Arc_Max Kid_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Kid_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_Kid;

//Kid player definitions
static const CharFrame char_kid_frame[] = {
  {Kid_ArcMain_Idle0, {  0,  0,192,169}, {122,  -224}}, //0 idle 1
  {Kid_ArcMain_Idle1, {  0,  0,171,186}, {111,  -207}}, //1 idle 2
  {Kid_ArcMain_Idle2, {  0,  0,177,182}, {114,  -211}}, //2 idle 3

  {Kid_ArcMain_Left0, {  0,  0,169,170}, {181,  -223}}, //3 left 1
  {Kid_ArcMain_Left1, {  0,  0,168,171}, {167,  -223}}, //4 left 2

  {Kid_ArcMain_Down0, {  0,  0,212,114}, {148,  -282}}, //5 down 1
  {Kid_ArcMain_Down0, {  0,115,234,136}, {155,  -261}}, //6 down 2
  {Kid_ArcMain_Down1, {  0,  0,173,136}, {120,  -261}}, //7 down 3

  {Kid_ArcMain_Up0, {  0,  0,181,191}, {125,  -141}}, //8 up 1
  {Kid_ArcMain_Up1, {  0,  0,217,169}, {141,  -167}}, //9 up 2

  {Kid_ArcMain_Right0, {  0,  0,190,179}, { 62,  -214}}, //10 right 1
  {Kid_ArcMain_Right1, {  0,  0,191,184}, { 73,  -209}}, //11 right 2
};

static const Animation char_kid_anim[PlayerAnim_Max] = {
	{3, (const u8[]){ 0, 1, 2, ASCR_BACK, 1}}, //CharAnim_Idle
	{3, (const u8[]){ 3, 4, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{3, (const u8[]){ 5, 6, 7, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{3, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{3, (const u8[]){ 10, 11, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

//Kid player functions
void Char_Kid_SetFrame(void *user, u8 frame)
{
	Char_Kid *this = (Char_Kid*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_kid_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Kid_Tick(Character *character)
{
	Char_Kid *this = (Char_Kid*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_Kid_SetFrame);
	Character_Draw(character, &this->tex, &char_kid_frame[this->frame]);
}

void Char_Kid_SetAnim(Character *character, u8 anim)
{
	Char_Kid *this = (Char_Kid*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Kid_Free(Character *character)
{
	Char_Kid *this = (Char_Kid*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Kid_New(fixed_t x, fixed_t y)
{
	//Allocate kid object
	Char_Kid *this = Mem_Alloc(sizeof(Char_Kid));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Kid_New] Failed to allocate kid object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Kid_Tick;
	this->character.set_anim = Char_Kid_SetAnim;
	this->character.free = Char_Kid_Free;
	
	Animatable_Init(&this->character.animatable, char_kid_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 8;
	
	//health bar color
	this->character.health_bar = 0xFF00A2E8;
	
	this->character.focus_x = FIXED_DEC(-167,1);
	this->character.focus_y = FIXED_DEC(205,1);
	this->character.focus_zoom = FIXED_DEC(121,256);
	
	this->character.size = FIXED_DEC(8,10);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\KID.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
