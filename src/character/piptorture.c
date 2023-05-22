/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "piptorture.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//PipTorture character structure
enum
{
  PipTorture_ArcMain_Idle0,
  PipTorture_ArcMain_Idle1,
  PipTorture_ArcMain_Left0,
  PipTorture_ArcMain_Down0,
  PipTorture_ArcMain_Up0,
  PipTorture_ArcMain_Right0,
  PipTorture_ArcMain_Right1,
  PipTorture_ArcMain_Huh0,
  PipTorture_ArcMain_Huh1,
  PipTorture_ArcMain_Huh2,
  PipTorture_ArcMain_Huh3,
	
	PipTorture_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[PipTorture_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_PipTorture;

//PipTorture character definitions
static const CharFrame char_piptorture_frame[] = {
  {PipTorture_ArcMain_Idle0, {  0,  0,122, 98}, {158,168}}, //0 idle 1
  {PipTorture_ArcMain_Idle0, {123,  0,122, 99}, {158,169}}, //1 idle 2
  {PipTorture_ArcMain_Idle0, {  0,100,122,100}, {159,170}}, //2 idle 3
  {PipTorture_ArcMain_Idle0, {123,100,121,100}, {158,170}}, //3 idle 4
  {PipTorture_ArcMain_Idle1, {  0,  0,121,100}, {158,170}}, //4 idle 5

  {PipTorture_ArcMain_Left0, {  0,  0,122,102}, {164,171}}, //5 left 1
  {PipTorture_ArcMain_Left0, {123,  0,121,102}, {164,171}}, //6 left 2
  {PipTorture_ArcMain_Left0, {  0,103,121,101}, {164,170}}, //7 left 3
  {PipTorture_ArcMain_Left0, {122,103,121,101}, {164,170}}, //8 left 4

  {PipTorture_ArcMain_Down0, {  0,  0,122, 98}, {159,167}}, //9 down 1
  {PipTorture_ArcMain_Down0, {123,  0,122, 99}, {160,168}}, //10 down 2

  {PipTorture_ArcMain_Up0, {  0,  0,121,104}, {162,174}}, //11 up 1
  {PipTorture_ArcMain_Up0, {122,  0,120,103}, {161,173}}, //12 up 2
  {PipTorture_ArcMain_Up0, {  0,105,121,103}, {161,173}}, //13 up 3

  {PipTorture_ArcMain_Right0, {  0,  0,135,101}, {158,170}}, //14 right 1
  {PipTorture_ArcMain_Right0, {  0,102,133,101}, {158,170}}, //15 right 2
  {PipTorture_ArcMain_Right1, {  0,  0,133,101}, {158,170}}, //16 right 3

  {PipTorture_ArcMain_Huh0, {  0,  0,122, 98}, {157,168}}, //17 huh 1
  {PipTorture_ArcMain_Huh0, {123,  0,122, 99}, {158,169}}, //18 huh 2
  {PipTorture_ArcMain_Huh0, {  0,100,121,100}, {158,170}}, //19 huh 3
  {PipTorture_ArcMain_Huh0, {122,100,121,100}, {158,170}}, //20 huh 4
  {PipTorture_ArcMain_Huh1, {  0,  0,121,100}, {158,170}}, //21 huh 5
  {PipTorture_ArcMain_Huh1, {122,  0,121,100}, {158,170}}, //22 huh 6
  {PipTorture_ArcMain_Huh1, {  0,101,121,103}, {159,173}}, //23 huh 7
  {PipTorture_ArcMain_Huh1, {122,101,118,105}, {157,175}}, //24 huh 8
  {PipTorture_ArcMain_Huh2, {  0,  0,122,103}, {159,173}}, //25 huh 9
  {PipTorture_ArcMain_Huh2, {123,  0,122,103}, {159,173}}, //26 huh 10
  {PipTorture_ArcMain_Huh2, {  0,104,121,101}, {158,174}}, //27 huh 11
  {PipTorture_ArcMain_Huh2, {122,104,121,101}, {159,171}}, //28 huh 12
  {PipTorture_ArcMain_Huh3, {  0,  0,120,101}, {157,169}}, //29 huh 13
  {PipTorture_ArcMain_Huh3, {121,  0,122,102}, {155,169}}, //30 huh 14
  {PipTorture_ArcMain_Huh3, {  0,103,122,102}, {155,169}}, //31 huh 15
  {PipTorture_ArcMain_Huh3, {123,103,122,101}, {156,169}}, //32 huh 16
};

static const Animation char_piptorture_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 14, 15, 16, 16, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 23, 23, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 27, 27, 27, 22, 22, 22, 22, 27, 27, 27, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Special1
};

//PipTorture character functions
void Char_PipTorture_SetFrame(void *user, u8 frame)
{
	Char_PipTorture *this = (Char_PipTorture*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_piptorture_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_PipTorture_Tick(Character *character)
{
	Char_PipTorture *this = (Char_PipTorture*)character;
	
	//Perform idle dance
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2))
	{
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Torture: //Rozebud????
				if (stage.song_step == 1027)
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 1058)
					character->set_anim(character, CharAnim_Special2);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_PipTorture_SetFrame);
	Character_Draw(character, &this->tex, &char_piptorture_frame[this->frame]);
}

void Char_PipTorture_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_PipTorture_Free(Character *character)
{
	Char_PipTorture *this = (Char_PipTorture*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_PipTorture_New(fixed_t x, fixed_t y)
{
	//Allocate piptorture object
	Char_PipTorture *this = Mem_Alloc(sizeof(Char_PipTorture));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_PipTorture_New] Failed to allocate piptorture object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_PipTorture_Tick;
	this->character.set_anim = Char_PipTorture_SetAnim;
	this->character.free = Char_PipTorture_Free;
	
	Animatable_Init(&this->character.animatable, char_piptorture_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFF0064CE;
	
	this->character.focus_x = FIXED_DEC(-23,1);
	this->character.focus_y = FIXED_DEC(-102,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\PIPTORT.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "right1.tim",
  "huh0.tim",
  "huh1.tim",
  "huh2.tim",
  "huh3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
