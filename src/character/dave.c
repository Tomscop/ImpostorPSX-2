/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "dave.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Dave character structure
enum
{
  Dave_ArcMain_Idle0,
  Dave_ArcMain_Idle1,
  Dave_ArcMain_Idle2,
  Dave_ArcMain_Idle3,
  Dave_ArcMain_Left0,
  Dave_ArcMain_Left1,
  Dave_ArcMain_Down0,
  Dave_ArcMain_Down1,
  Dave_ArcMain_Up0,
  Dave_ArcMain_Up1,
  Dave_ArcMain_Right0,
  Dave_ArcMain_Jump0,
  Dave_ArcMain_Jump1,
  Dave_ArcMain_Jump2,
  Dave_ArcMain_Jump3,
  Dave_ArcMain_Jump4,
  Dave_ArcMain_Jump5,
  Dave_ArcMain_Jump6,
  Dave_ArcMain_Jump7,
  Dave_ArcMain_Jump8,
  Dave_ArcMain_Jump9,
  Dave_ArcMain_Jump10,
	
	Dave_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Dave_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Dave;

//Dave character definitions
static const CharFrame char_dave_frame[] = {
  {Dave_ArcMain_Idle0, {  0,  0,149,112}, {165, 95}}, //0 idle 1
  {Dave_ArcMain_Idle0, {  0,115,146,115}, {168, 98}}, //1 idle 2
  {Dave_ArcMain_Idle1, {  0,  0,144,117}, {172,100}}, //2 idle 3
  {Dave_ArcMain_Idle1, {  0,124,161,124}, {168,106}}, //3 idle 4
  {Dave_ArcMain_Idle2, {  0,  0,157,121}, {167,103}}, //4 idle 5
  {Dave_ArcMain_Idle2, {  0,121,153,112}, {165, 95}}, //5 idle 6
  {Dave_ArcMain_Idle3, {  0,  0,148,113}, {165, 96}}, //6 idle 7
  {Dave_ArcMain_Idle3, {  0,113,149,112}, {165, 95}}, //7 idle 8

  {Dave_ArcMain_Left0, {  0,  0,166,118}, {193,101}}, //8 left 1
  {Dave_ArcMain_Left0, {  0,118,157,114}, {185, 97}}, //9 left 2
  {Dave_ArcMain_Left1, {  0,  0,158,115}, {186, 98}}, //10 left 3

  {Dave_ArcMain_Down0, {  0,  0,150,102}, {154, 85}}, //11 down 1
  {Dave_ArcMain_Down0, {  0,105,147,105}, {153, 88}}, //12 down 2
  {Dave_ArcMain_Down1, {  0,  0,146,108}, {153, 91}}, //13 down 3

  {Dave_ArcMain_Up0, {  0,  0,164,126}, {187,110}}, //14 up 1
  {Dave_ArcMain_Up0, {  0,126,163,118}, {187,101}}, //15 up 2
  {Dave_ArcMain_Up1, {  0,  0,163,121}, {186,104}}, //16 up 3

  {Dave_ArcMain_Right0, {  0,  0,137,109}, {142, 92}}, //17 right 1
  {Dave_ArcMain_Right0, {  0,110,122,110}, {144, 93}}, //18 right 2
  {Dave_ArcMain_Right0, {122,110,125,110}, {144, 93}}, //19 right 3

  {Dave_ArcMain_Jump0, {  0,  0,138,110}, {165, 94}}, //20 jump 1
  {Dave_ArcMain_Jump0, {  0,112,140,112}, {165, 96}}, //21 jump 2
  {Dave_ArcMain_Jump1, {  0,  0,139,112}, {165, 96}}, //22 jump 3
  {Dave_ArcMain_Jump1, {  0,112,140,112}, {164, 96}}, //23 jump 4
  {Dave_ArcMain_Jump2, {  0,  0,140,112}, {165, 96}}, //24 jump 5
  {Dave_ArcMain_Jump2, {  0,112,140,112}, {165, 96}}, //25 jump 6
  {Dave_ArcMain_Jump3, {  0,  0,140,112}, {165, 96}}, //26 jump 7
  {Dave_ArcMain_Jump3, {  0,112,140,112}, {165, 96}}, //27 jump 8
  {Dave_ArcMain_Jump4, {  0,  0,140,112}, {165, 96}}, //28 jump 9
  {Dave_ArcMain_Jump4, {  0,112,135,112}, {164, 96}}, //29 jump 10
  {Dave_ArcMain_Jump5, {  0,  0,145,112}, {165, 95}}, //30 jump 11
  {Dave_ArcMain_Jump5, {  0,112,144,112}, {165, 95}}, //31 jump 12
  {Dave_ArcMain_Jump6, {  0,  0,144,112}, {165, 95}}, //32 jump 13
  {Dave_ArcMain_Jump6, {  0,112,144,112}, {165, 95}}, //33 jump 14
  {Dave_ArcMain_Jump7, {  0,  0,144,112}, {165, 95}}, //34 jump 15
  {Dave_ArcMain_Jump7, {  0,112,144,112}, {164, 95}}, //35 jump 16
  {Dave_ArcMain_Jump8, {  0,  0,169,101}, {187, 84}}, //36 jump 17
  {Dave_ArcMain_Jump8, {  0,101,174, 99}, {193, 82}}, //37 jump 18
  {Dave_ArcMain_Jump9, {  0,  0,161,115}, {189,105}}, //38 jump 19
  {Dave_ArcMain_Jump10, {  0,  0,150,151}, {192,157}}, //39 jump 20
  {Dave_ArcMain_Jump10, {  0,  0,  1,  1}, {192,157}}, //40 hide
};

static const Animation char_dave_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 14, 15, 16, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{2, (const u8[]){ 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, ASCR_BACK, 1}},         //CharAnim_Special1
	{2, (const u8[]){ 36, 37, 38, 39, ASCR_CHGANI, CharAnim_Special3}},         //CharAnim_Special1
	{4, (const u8[]){ 40, ASCR_CHGANI, CharAnim_Special3}},         //CharAnim_Special3
};

//Dave character functions
void Char_Dave_SetFrame(void *user, u8 frame)
{
	Char_Dave *this = (Char_Dave*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_dave_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Dave_Tick(Character *character)
{
	Char_Dave *this = (Char_Dave*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Crewicide)
	{
		if (stage.song_step == 16)
		{
			this->character.focus_x = FIXED_DEC(-47,1);
			this->character.focus_y = FIXED_DEC(-85,1);
			this->character.focus_zoom = FIXED_DEC(549,512);
		}
		if (stage.song_step == 2032)
		{
			this->character.focus_x = FIXED_DEC(-61,1);
			this->character.focus_y = FIXED_DEC(-78,1);
			this->character.focus_zoom = FIXED_DEC(617,512);
		}
		if (stage.song_step == 2064)
		{
			this->character.focus_x = FIXED_DEC(-69,1);
			this->character.focus_y = FIXED_DEC(-85,1);
			this->character.focus_zoom = FIXED_DEC(549,512);
		}
	}
	
	//Perform idle dance
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2) && (character->animatable.anim != CharAnim_Special3))
	{
		if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
			Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Crewicide: //kys
				if (stage.song_step == 2042)
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 2060)
					character->set_anim(character, CharAnim_Special2);
				if (stage.song_step == 2065)
					character->set_anim(character, CharAnim_Special3);
				break;
			default:
				break;
		}
		
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Dave_SetFrame);
	Character_Draw(character, &this->tex, &char_dave_frame[this->frame]);
}

void Char_Dave_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Dave_Free(Character *character)
{
	Char_Dave *this = (Char_Dave*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Dave_New(fixed_t x, fixed_t y)
{
	//Allocate dave object
	Char_Dave *this = Mem_Alloc(sizeof(Char_Dave));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Dave_New] Failed to allocate dave object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Dave_Tick;
	this->character.set_anim = Char_Dave_SetAnim;
	this->character.free = Char_Dave_Free;
	
	Animatable_Init(&this->character.animatable, char_dave_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFF4C52B4;
	
	this->character.focus_x = FIXED_DEC(-51,1);
	this->character.focus_y = FIXED_DEC(-83,1);
	this->character.focus_zoom = FIXED_DEC(153,128);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\DAVE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "jump0.tim",
  "jump1.tim",
  "jump2.tim",
  "jump3.tim",
  "jump4.tim",
  "jump5.tim",
  "jump6.tim",
  "jump7.tim",
  "jump8.tim",
  "jump9.tim",
  "jump10.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
