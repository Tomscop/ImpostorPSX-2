/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "drippostor.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Drippostor character structure
enum
{
  Drippostor_ArcMain_Idle0,
  Drippostor_ArcMain_Idle1,
  Drippostor_ArcMain_Idle2,
  Drippostor_ArcMain_Idle3,
  Drippostor_ArcMain_Idle4,
  Drippostor_ArcMain_Left0,
  Drippostor_ArcMain_Left1,
  Drippostor_ArcMain_Left2,
  Drippostor_ArcMain_Left3,
  Drippostor_ArcMain_Down0,
  Drippostor_ArcMain_Down1,
  Drippostor_ArcMain_Up0,
  Drippostor_ArcMain_Up1,
  Drippostor_ArcMain_Right0,
  Drippostor_ArcMain_Right1,
  Drippostor_ArcMain_Right2,
  Drippostor_ArcMain_Right3,
  Drippostor_ArcMain_Right4,
	
	Drippostor_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Drippostor_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Drippostor;

//Drippostor character definitions
static const CharFrame char_drippostor_frame[] = {
  {Drippostor_ArcMain_Idle0, {  0,  0,135,139}, {152,135}}, //0 idle 1
  {Drippostor_ArcMain_Idle1, {  0,  0,136,140}, {153,136}}, //1 idle 2
  {Drippostor_ArcMain_Idle2, {  0,  0,134,141}, {153,137}}, //2 idle 3
  {Drippostor_ArcMain_Idle3, {  0,  0,125,142}, {153,138}}, //3 idle 4
  {Drippostor_ArcMain_Idle3, {125,  0,125,142}, {153,138}}, //4 idle 5
  {Drippostor_ArcMain_Idle4, {  0,  0,124,141}, {153,137}}, //5 idle 6

  {Drippostor_ArcMain_Left0, {  0,  0,168,144}, {196,139+1}}, //6 left 1
  {Drippostor_ArcMain_Left1, {  0,  0,163,144}, {188,139+1}}, //7 left 2
  {Drippostor_ArcMain_Left2, {  0,  0,163,144}, {188,139+1}}, //8 left 3
  {Drippostor_ArcMain_Left3, {  0,  0,163,144}, {188,139+1}}, //9 left 4

  {Drippostor_ArcMain_Down0, {  0,  0,124,134}, {152,131}}, //10 down 1
  {Drippostor_ArcMain_Down0, {124,  0,122,136}, {152,133}}, //11 down 2
  {Drippostor_ArcMain_Down1, {  0,  0,121,136}, {152,133}}, //12 down 3
  {Drippostor_ArcMain_Down1, {121,  0,120,136}, {152,133}}, //13 down 4

  {Drippostor_ArcMain_Up0, {  0,  0,125,168}, {166+2,163+1}}, //14 up 1
  {Drippostor_ArcMain_Up0, {125,  0,126,162}, {165+2,157+1}}, //15 up 2
  {Drippostor_ArcMain_Up1, {  0,  0,126,163}, {165+2,157+1}}, //16 up 3
  {Drippostor_ArcMain_Up1, {126,  0,126,162}, {165+2,157+1}}, //17 up 4

  {Drippostor_ArcMain_Right0, {  0,  0,140,144}, {149,139+1}}, //18 right 1
  {Drippostor_ArcMain_Right1, {  0,  0,129,144}, {151,139+1}}, //19 right 2
  {Drippostor_ArcMain_Right2, {  0,  0,129,144}, {151,139+1}}, //20 right 3
  {Drippostor_ArcMain_Right3, {  0,  0,130,144}, {151,139+1}}, //21 right 4
  {Drippostor_ArcMain_Right4, {  0,  0,130,144}, {151,139+1}}, //22 right 5
};

static const Animation char_drippostor_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 14, 15, 16, 17, 17, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 18, 19, 20, 21, 22, 22, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Drippostor character functions
void Char_Drippostor_SetFrame(void *user, u8 frame)
{
	Char_Drippostor *this = (Char_Drippostor*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_drippostor_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Drippostor_Tick(Character *character)
{
	Char_Drippostor *this = (Char_Drippostor*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Drippypop)
	{
		if (stage.song_step == -29)
		{
			this->character.focus_x = FIXED_DEC(-43,1);
			this->character.focus_y = FIXED_DEC(-95,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.song_beat == 286)
		{
			this->character.focus_x = FIXED_DEC(-9,1);
			this->character.focus_y = FIXED_DEC(-157,1);
			this->character.focus_zoom = FIXED_DEC(1516,1024);
		}
		if (stage.song_beat == 304)
		{
			this->character.focus_x = FIXED_DEC(-43,1);
			this->character.focus_y = FIXED_DEC(-95,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.song_beat == 318)
		{
			this->character.focus_x = FIXED_DEC(-9,1);
			this->character.focus_y = FIXED_DEC(-157,1);
			this->character.focus_zoom = FIXED_DEC(1516,1024);
		}
		if (stage.song_beat == 336)
		{
			this->character.focus_x = FIXED_DEC(-43,1);
			this->character.focus_y = FIXED_DEC(-95,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.song_beat == 384)
		{
			this->character.focus_x = FIXED_DEC(-9,1);
			this->character.focus_y = FIXED_DEC(-157,1);
			this->character.focus_zoom = FIXED_DEC(1516,1024);
		}
		if (stage.song_beat == 401)
		{
			this->character.focus_x = FIXED_DEC(-43,1);
			this->character.focus_y = FIXED_DEC(-95,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Drippostor_SetFrame);
	Character_Draw(character, &this->tex, &char_drippostor_frame[this->frame]);
}

void Char_Drippostor_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Drippostor_Free(Character *character)
{
	Char_Drippostor *this = (Char_Drippostor*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Drippostor_New(fixed_t x, fixed_t y)
{
	//Allocate drippostor object
	Char_Drippostor *this = Mem_Alloc(sizeof(Char_Drippostor));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Drippostor_New] Failed to allocate drippostor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Drippostor_Tick;
	this->character.set_anim = Char_Drippostor_SetAnim;
	this->character.free = Char_Drippostor_Free;
	
	Animatable_Init(&this->character.animatable, char_drippostor_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFBD69DE;
	
	this->character.focus_x = FIXED_DEC(-43,1);
	this->character.focus_y = FIXED_DEC(-95,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\DRIPOSTR.ARC;1");
	
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
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "right4.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
