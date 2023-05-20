/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "reginald.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Reginald character structure
enum
{
  Reginald_ArcMain_Idle0,
  Reginald_ArcMain_Idle1,
  Reginald_ArcMain_Idle2,
  Reginald_ArcMain_Left0,
  Reginald_ArcMain_Left1,
  Reginald_ArcMain_Left2,
  Reginald_ArcMain_Down0,
  Reginald_ArcMain_Down1,
  Reginald_ArcMain_Up0,
  Reginald_ArcMain_Up1,
  Reginald_ArcMain_Up2,
  Reginald_ArcMain_Right0,
  Reginald_ArcMain_Right1,
  Reginald_ArcMain_Right2,
  Reginald_ArcMain_Right3,
	
	Reginald_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Reginald_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Reginald;

//Reginald character definitions
static const CharFrame char_reginald_frame[] = {
  {Reginald_ArcMain_Idle0, {  0,  0,104,211}, {140,175}}, //0 idle 1
  {Reginald_ArcMain_Idle0, {104,  0,105,211}, {141,176}}, //1 idle 2
  {Reginald_ArcMain_Idle1, {  0,  0,109,215}, {147,179}}, //2 idle 3
  {Reginald_ArcMain_Idle1, {109,  0,110,217}, {149,181}}, //3 idle 4
  {Reginald_ArcMain_Idle2, {  0,  0,110,218}, {149,182}}, //4 idle 5
  {Reginald_ArcMain_Idle2, {110,  0,111,218}, {150,182}}, //5 idle 6

  {Reginald_ArcMain_Left0, {  0,  0,126,212}, {169,176}}, //6 left 1
  {Reginald_ArcMain_Left0, {126,  0,129,216}, {171,180}}, //7 left 2
  {Reginald_ArcMain_Left1, {  0,  0,130,216}, {171,180}}, //8 left 3
  {Reginald_ArcMain_Left2, {  0,  0,130,216}, {171,180}}, //9 left 4

  {Reginald_ArcMain_Down0, {  0,  0,105,197}, {135,162}}, //10 down 1
  {Reginald_ArcMain_Down0, {105,  0,108,200}, {138,165}}, //11 down 2
  {Reginald_ArcMain_Down1, {  0,  0,108,200}, {139,165}}, //12 down 3
  {Reginald_ArcMain_Down1, {108,  0,109,200}, {139,165}}, //13 down 4

  {Reginald_ArcMain_Up0, {  0,  0,128,229}, {155,192}}, //14 up 1
  {Reginald_ArcMain_Up0, {128,  0,126,227}, {153,190}}, //15 up 2
  {Reginald_ArcMain_Up1, {  0,  0,127,227}, {153,190}}, //16 up 3
  {Reginald_ArcMain_Up1, {127,  0,126,227}, {153,190}}, //17 up 4
  {Reginald_ArcMain_Up2, {  0,  0,126,227}, {153,190}}, //18 up 5

  {Reginald_ArcMain_Right0, {  0,  0,132,206}, {142,171}}, //19 right 1
  {Reginald_ArcMain_Right1, {  0,  0,130,210}, {142,174}}, //20 right 2
  {Reginald_ArcMain_Right2, {  0,  0,130,210}, {142,174}}, //21 right 3
  {Reginald_ArcMain_Right3, {  0,  0,130,210}, {142,174}}, //22 right 4
};

static const Animation char_reginald_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 9, 8, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 14, 15, 16, 17, 18, 18, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 19, 20, 21, 22, 22, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Reginald character functions
void Char_Reginald_SetFrame(void *user, u8 frame)
{
	Char_Reginald *this = (Char_Reginald*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_reginald_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Reginald_Tick(Character *character)
{
	Char_Reginald *this = (Char_Reginald*)character;
	
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
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0xF) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Reginald_SetFrame);
	Character_Draw(character, &this->tex, &char_reginald_frame[this->frame]);
}

void Char_Reginald_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Reginald_Free(Character *character)
{
	Char_Reginald *this = (Char_Reginald*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Reginald_New(fixed_t x, fixed_t y)
{
	//Allocate reginald object
	Char_Reginald *this = Mem_Alloc(sizeof(Char_Reginald));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Reginald_New] Failed to allocate reginald object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Reginald_Tick;
	this->character.set_anim = Char_Reginald_SetAnim;
	this->character.free = Char_Reginald_Free;
	
	Animatable_Init(&this->character.animatable, char_reginald_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFE44832;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\REGINALD.ARC;1");
	
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
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
