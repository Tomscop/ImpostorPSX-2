/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "righthandman.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Right Hand Man character structure
enum
{
  RightHandMan_ArcMain_Idle0,
  RightHandMan_ArcMain_Idle1,
  RightHandMan_ArcMain_Idle2,
  RightHandMan_ArcMain_Idle3,
  RightHandMan_ArcMain_Idle4,
  RightHandMan_ArcMain_Left0,
  RightHandMan_ArcMain_Left1,
  RightHandMan_ArcMain_Left2,
  RightHandMan_ArcMain_Left3,
  RightHandMan_ArcMain_Left4,
  RightHandMan_ArcMain_Left5,
  RightHandMan_ArcMain_Down0,
  RightHandMan_ArcMain_Down1,
  RightHandMan_ArcMain_Down2,
  RightHandMan_ArcMain_Down3,
  RightHandMan_ArcMain_Up0,
  RightHandMan_ArcMain_Up1,
  RightHandMan_ArcMain_Up2,
  RightHandMan_ArcMain_Up3,
  RightHandMan_ArcMain_Up4,
  RightHandMan_ArcMain_Right0,
  RightHandMan_ArcMain_Right1,
  RightHandMan_ArcMain_Right2,
  RightHandMan_ArcMain_Right3,
	
	RightHandMan_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RightHandMan_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RightHandMan;

//Right Hand Man character definitions
static const CharFrame char_righthandman_frame[] = {
  {RightHandMan_ArcMain_Idle0, {  0,  0,128,200}, {130,202}}, //0 idle 1
  {RightHandMan_ArcMain_Idle1, {  0,  0,129,200}, {131,202}}, //1 idle 2
  {RightHandMan_ArcMain_Idle2, {  0,  0,127,202}, {129,204}}, //2 idle 3
  {RightHandMan_ArcMain_Idle2, {127,  0,123,205}, {132,207}}, //3 idle 4
  {RightHandMan_ArcMain_Idle3, {  0,  0,123,205}, {132,207}}, //4 idle 5
  {RightHandMan_ArcMain_Idle3, {123,  0,123,205}, {133,207}}, //5 idle 6
  {RightHandMan_ArcMain_Idle4, {  0,  0,123,205}, {133,207}}, //6 idle 7

  {RightHandMan_ArcMain_Left0, {  0,  0,137,204}, {163,205}}, //7 left 1
  {RightHandMan_ArcMain_Left1, {  0,  0,137,204}, {163,205}}, //8 left 2
  {RightHandMan_ArcMain_Left2, {  0,  0,140,206}, {162,207}}, //9 left 3
  {RightHandMan_ArcMain_Left3, {  0,  0,140,206}, {162,207}}, //10 left 4
  {RightHandMan_ArcMain_Left4, {  0,  0,140,206}, {162,207}}, //11 left 5
  {RightHandMan_ArcMain_Left5, {  0,  0,140,206}, {162,207}}, //12 left 6

  {RightHandMan_ArcMain_Down0, {  0,  0,133,183}, {140,185}}, //13 down 1
  {RightHandMan_ArcMain_Down1, {  0,  0,135,188}, {139,189}}, //14 down 2
  {RightHandMan_ArcMain_Down2, {  0,  0,135,188}, {139,190}}, //15 down 3
  {RightHandMan_ArcMain_Down3, {  0,  0,135,188}, {139,190}}, //16 down 4

  {RightHandMan_ArcMain_Up0, {  0,  0,119,222}, {136,251}}, //17 up 1
  {RightHandMan_ArcMain_Up0, {119,  0,120,222}, {136,251}}, //18 up 2
  {RightHandMan_ArcMain_Up1, {  0,  0,113,216}, {129,249}}, //19 up 3
  {RightHandMan_ArcMain_Up1, {113,  0,113,216}, {129,249}}, //20 up 4
  {RightHandMan_ArcMain_Up2, {  0,  0,111,218}, {128,250}}, //21 up 5
  {RightHandMan_ArcMain_Up2, {111,  0,112,217}, {128,250}}, //22 up 6
  {RightHandMan_ArcMain_Up3, {  0,  0,113,217}, {129,250}}, //23 up 7
  {RightHandMan_ArcMain_Up3, {113,  0,112,218}, {129,250}}, //24 up 8
  {RightHandMan_ArcMain_Up4, {  0,  0,112,217}, {129,250}}, //25 up 9

  {RightHandMan_ArcMain_Right0, {  0,  0,127,194}, {116,195}}, //26 right 1
  {RightHandMan_ArcMain_Right1, {  0,  0,131,195}, {122,197}}, //27 right 2
  {RightHandMan_ArcMain_Right2, {  0,  0,130,195}, {122,197}}, //28 right 3
  {RightHandMan_ArcMain_Right3, {  0,  0,130,195}, {122,197}}, //29 right 4
};

static const Animation char_righthandman_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 7, 8, 9, 10, 11, 12, 11, 12, 11, 11, 11, 11, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 13, 14, 15, 16, 16, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 17, 18, 19, 20, 21, 22, 23, 24, 25, 23, 24, 25, 23, 24, 25, 23, 24, 25, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 26, 27, 28, 29, 29, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Right Hand Man character functions
void Char_RightHandMan_SetFrame(void *user, u8 frame)
{
	Char_RightHandMan *this = (Char_RightHandMan*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_righthandman_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RightHandMan_Tick(Character *character)
{
	Char_RightHandMan *this = (Char_RightHandMan*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Armed)
	{
		if (stage.song_step == 0)
		{
			this->character.focus_x = FIXED_DEC(-49,1);
			this->character.focus_y = FIXED_DEC(-100,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
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
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0xF) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_RightHandMan_SetFrame);
	Character_Draw(character, &this->tex, &char_righthandman_frame[this->frame]);
}

void Char_RightHandMan_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RightHandMan_Free(Character *character)
{
	Char_RightHandMan *this = (Char_RightHandMan*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RightHandMan_New(fixed_t x, fixed_t y)
{
	//Allocate righthandman object
	Char_RightHandMan *this = Mem_Alloc(sizeof(Char_RightHandMan));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RightHandMan_New] Failed to allocate righthandman object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RightHandMan_Tick;
	this->character.set_anim = Char_RightHandMan_SetAnim;
	this->character.free = Char_RightHandMan_Free;
	
	Animatable_Init(&this->character.animatable, char_righthandman_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFE44832;
	
	this->character.focus_x = FIXED_DEC(-36,1);
	this->character.focus_y = FIXED_DEC(-111,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\RHM.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "left4.tim",
  "left5.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "up4.tim",
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
