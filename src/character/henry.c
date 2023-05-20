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
  Henry_ArcMain_Shock0,
  Henry_ArcMain_Shock1,
  Henry_ArcMain_Shock2,
  Henry_ArcMain_Cutscene0,
  Henry_ArcMain_Cutscene1,
  Henry_ArcMain_Cutscene2,
  Henry_ArcMain_Cutscene3,
  Henry_ArcMain_Cutscene4,
	
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
  
  {Henry_ArcMain_Shock0, {  0,  0,101,176}, {158-100,155-9}}, //14 shock 1
  {Henry_ArcMain_Shock0, {101,  0,101,176}, {158-100,156-9}}, //15 shock 2
  {Henry_ArcMain_Shock1, {  0,  0,101,176}, {158-100,156-9}}, //16 shock 3
  {Henry_ArcMain_Shock1, {101,  0,101,176}, {159-100,156-9}}, //17 shock 4
  {Henry_ArcMain_Shock2, {  0,  0, 86,182}, {151-100,162-9}}, //18 shock 5
  {Henry_ArcMain_Shock2, { 86,  0, 86,182}, {152-100,162-9}}, //19 shock 6

  {Henry_ArcMain_Cutscene0, {  0,  0, 86,182}, {152-100,162-9}}, //20 cutscene 1
  {Henry_ArcMain_Cutscene0, { 86,  0, 92,187}, {156-100,167-9}}, //21 cutscene 2
  {Henry_ArcMain_Cutscene1, {  0,  0, 92,186}, {156-100,166-9}}, //22 cutscene 3
  {Henry_ArcMain_Cutscene1, { 92,  0, 93,186}, {157-100,166-9}}, //23 cutscene 4
  {Henry_ArcMain_Cutscene2, {  0,  0, 92,184}, {157-100,164-9}}, //24 cutscene 5
  {Henry_ArcMain_Cutscene2, { 92,  0, 91,180}, {156-100,159-9}}, //25 cutscene 6
  {Henry_ArcMain_Cutscene3, {  0,  0, 88,178}, {153-100,158-9}}, //26 cutscene 7
  {Henry_ArcMain_Cutscene3, { 88,  0, 87,179}, {153-100,159-9}}, //27 cutscene 8
  {Henry_ArcMain_Cutscene4, {  0,  0, 88,180}, {153-100,159-9}}, //28 cutscene 9
  {Henry_ArcMain_Cutscene4, { 88,  0, 87,180}, {153-100,160-9}}, //29 cutscene 10
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
	
	{1, (const u8[]){ 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 19, 19, 19, 19, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special1
	{1, (const u8[]){ 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, ASCR_CHGANI, CharAnim_Special3}},   //CharAnim_Special2
	{2, (const u8[]){ 29, ASCR_CHGANI, CharAnim_Special3}},   //CharAnim_Special3
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
	
	//Camera stuff
	if (stage.stage_id == StageId_Titular)
	{
		if (stage.song_step == 0)
		{
			this->character.focus_y = FIXED_DEC(-64,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
	}
	if (stage.stage_id == StageId_Reinforcements)
	{
		if (stage.song_step == 0)
		{
			this->character.focus_y = FIXED_DEC(-64,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
		if (stage.song_step == 650)
			this->character.focus_x = FIXED_DEC(2,1);
		if (stage.song_step == 696)
			this->character.health_i = 3;
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Reinforcements:
				if (stage.song_step == 696) //woah
					character->set_anim(character, CharAnim_Special1);
//				if (stage.song_step == idk) //armed cutscene
//					character->set_anim(character, CharAnim_Special2);
				if (stage.song_step == 1216) //lmao this is just to get around a bug
					character->set_anim(character, CharAnim_Left);
				if (stage.song_step == 1222)
					character->set_anim(character, CharAnim_Up);
				break;
			default:
				break;
		}
		
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
	
	this->character.focus_x = FIXED_DEC(53,1);
	this->character.focus_y = FIXED_DEC(-75,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\HENRY.ARC;1");
	
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
  "shock0.tim",
  "shock1.tim",
  "shock2.tim",
  "cutscene0.tim",
  "cutscene1.tim",
  "cutscene2.tim",
  "cutscene3.tim",
  "cutscene4.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
