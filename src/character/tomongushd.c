/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tomongushd.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//TomongusHD character structure
enum
{
  TomongusHD_ArcMain_Idle0,
  TomongusHD_ArcMain_Left0,
  TomongusHD_ArcMain_Down0,
  TomongusHD_ArcMain_Up0,
  TomongusHD_ArcMain_Right0,
  TomongusHD_ArcMain_Prep0,
  TomongusHD_ArcMain_Shot0,
  TomongusHD_ArcMain_Shot1,
  TomongusHD_ArcMain_Shot2,
  TomongusHD_ArcMain_Shot3,
  TomongusHD_ArcMain_Shot4,
  TomongusHD_ArcMain_Shot5,
  TomongusHD_ArcMain_Shot6,
  TomongusHD_ArcMain_Hampter0,
	
	TomongusHD_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[TomongusHD_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_TomongusHD;

//TomongusHD character definitions
static const CharFrame char_tomongushd_frame[] = {
  {TomongusHD_ArcMain_Idle0, {  0,  0, 80, 85}, {176, 89}}, //0 idle 1
  {TomongusHD_ArcMain_Idle0, { 80,  0, 77, 85}, {175, 89}}, //1 idle 2
  {TomongusHD_ArcMain_Idle0, {157,  0, 76, 87}, {174, 91}}, //2 idle 3
  {TomongusHD_ArcMain_Idle0, {  0, 87, 75, 87}, {174, 91}}, //3 idle 4
  {TomongusHD_ArcMain_Idle0, { 75, 87, 75, 87}, {174, 91}}, //4 idle 5

  {TomongusHD_ArcMain_Left0, {  0,  0, 83, 88}, {176, 91}}, //5 left 1
  {TomongusHD_ArcMain_Left0, { 83,  0, 82, 89}, {175, 92}}, //6 left 2

  {TomongusHD_ArcMain_Down0, {  0,  0, 85, 82}, {179, 85}}, //7 down 1
  {TomongusHD_ArcMain_Down0, { 85,  0, 82, 81}, {178, 84}}, //8 down 2

  {TomongusHD_ArcMain_Up0, {  0,  0, 67, 92}, {172, 94}}, //9 up 1
  {TomongusHD_ArcMain_Up0, { 67,  0, 69, 92}, {173, 95}}, //10 up 2

  {TomongusHD_ArcMain_Right0, {  0,  0, 83, 86}, {180, 91}}, //11 right 1
  {TomongusHD_ArcMain_Right0, { 83,  0, 82, 85}, {181, 90}}, //12 right 2

  {TomongusHD_ArcMain_Prep0, {  0,  0, 75, 87}, {175, 90}}, //13 prep 1
  {TomongusHD_ArcMain_Prep0, { 75,  0, 76, 86}, {175, 89}}, //14 prep 2
  {TomongusHD_ArcMain_Prep0, {151,  0, 76, 87}, {175, 90}}, //15 prep 3
  {TomongusHD_ArcMain_Prep0, {  0, 87, 75, 88}, {175, 91}}, //16 prep 4
  {TomongusHD_ArcMain_Prep0, { 75, 87, 75, 88}, {175, 91}}, //17 prep 5
  {TomongusHD_ArcMain_Prep0, {150, 87, 75, 88}, {175, 91}}, //18 prep 6

  {TomongusHD_ArcMain_Shot0, {  0,  0,127, 90}, {220, 93}}, //19 shot 1
  {TomongusHD_ArcMain_Shot0, {  0, 90,159,101}, {253,104}}, //20 shot 2
  {TomongusHD_ArcMain_Shot1, {  0,  0,158,100}, {253,104}}, //21 shot 3
  {TomongusHD_ArcMain_Shot1, {  0,100,201, 94}, {297, 97}}, //22 shot 4
  {TomongusHD_ArcMain_Shot2, {  0,  0,201, 94}, {297, 97}}, //23 shot 5
  {TomongusHD_ArcMain_Shot2, {  0, 94,255, 90}, {350, 94}}, //24 shot 6
  {TomongusHD_ArcMain_Shot3, {  0,  0,255, 90}, {350, 94}}, //25 shot 7
  {TomongusHD_ArcMain_Shot3, {  0, 90,154, 79}, {249, 82}}, //26 shot 8
  {TomongusHD_ArcMain_Shot4, {  0,  0,154, 79}, {249, 82}}, //27 shot 9
  {TomongusHD_ArcMain_Shot4, {  0, 79,164, 65}, {259, 68}}, //28 shot 10
  {TomongusHD_ArcMain_Shot4, {  0,158,164, 65}, {259, 68}}, //29 shot 11
  {TomongusHD_ArcMain_Shot5, {  0,  0,184, 52}, {279, 55}}, //30 shot 12
  {TomongusHD_ArcMain_Shot5, {  0, 52, 84, 52}, {179, 55}}, //31 shot 13
  {TomongusHD_ArcMain_Shot5, { 84, 52, 84, 51}, {179, 54}}, //32 shot 14
  {TomongusHD_ArcMain_Shot5, {168, 52, 84, 51}, {179, 54}}, //33 shot 15
  {TomongusHD_ArcMain_Shot5, {  0,104, 84, 58}, {179, 61}}, //34 shot 16
  {TomongusHD_ArcMain_Shot5, { 84,104, 84, 58}, {179, 61}}, //35 shot 17
  {TomongusHD_ArcMain_Shot5, {168,104, 84, 62}, {179, 66}}, //36 shot 18
  {TomongusHD_ArcMain_Shot5, {  0,166, 84, 62}, {180, 66}}, //37 shot 19
  {TomongusHD_ArcMain_Shot5, { 84,166, 84, 62}, {179, 66}}, //38 shot 20
  {TomongusHD_ArcMain_Shot5, {168,166, 84, 62}, {179, 66}}, //39 shot 21
  {TomongusHD_ArcMain_Shot6, {  0,  0, 84, 62}, {178, 66}}, //40 shot 22
  {TomongusHD_ArcMain_Shot6, { 84,  0, 84, 62}, {179, 66}}, //41 shot 23
  {TomongusHD_ArcMain_Shot6, {168,  0, 84, 62}, {179, 65}}, //42 shot 24
  {TomongusHD_ArcMain_Shot6, {  0, 62, 83, 61}, {178, 64}}, //43 shot 25
  {TomongusHD_ArcMain_Shot6, { 83, 62, 83, 62}, {178, 65}}, //44 shot 26
  {TomongusHD_ArcMain_Shot6, {166, 62, 84, 62}, {178, 65}}, //45 shot 27
  {TomongusHD_ArcMain_Shot6, {  0,124, 84, 62}, {179, 65}}, //46 shot 28
  {TomongusHD_ArcMain_Shot6, { 84,124, 84, 62}, {179, 65}}, //47 shot 29

  {TomongusHD_ArcMain_Hampter0, {  0,  0, 84, 62}, {179, 66}}, //48 hampter 1
  {TomongusHD_ArcMain_Hampter0, { 84,  0, 84, 62}, {179, 66}}, //49 hampter 2
  {TomongusHD_ArcMain_Hampter0, {168,  0, 83, 62}, {179, 66}}, //50 hampter 3
  {TomongusHD_ArcMain_Hampter0, {  0, 62, 82, 63}, {178, 67}}, //51 hampter 4
  {TomongusHD_ArcMain_Hampter0, { 82, 62, 81, 63}, {177, 67}}, //52 hampter 5
  {TomongusHD_ArcMain_Hampter0, {163, 62, 81, 63}, {177, 67}}, //53 hampter 6
};

static const Animation char_tomongushd_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 13, 13, 14, 14, 15, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, ASCR_BACK, 1}},         //CharAnim_Special2
	{1, (const u8[]){ 41, 39, 40, 41, 39, 40, 41, 39, 40, 42, 43, 43, 44, 45, 44, 45, 44, 44, 46, 46, 47, 47, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Special3
};

static const Animation char_tomongushd_anim2[CharAnim_Max] = {
	{1, (const u8[]){ 48, 49, 50, 50, 51, 51, 52, 52, 53, 53, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 13, 13, 14, 14, 15, 15, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, ASCR_BACK, 1}},         //CharAnim_Special2
	{1, (const u8[]){ 41, 39, 40, 41, 39, 40, 41, 39, 40, 42, 43, 43, 44, 45, 44, 45, 44, 44, 46, 46, 47, 47, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Special3
};

//TomongusHD character functions
void Char_TomongusHD_SetFrame(void *user, u8 frame)
{
	Char_TomongusHD *this = (Char_TomongusHD*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tomongushd_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_TomongusHD_Tick(Character *character)
{
	Char_TomongusHD *this = (Char_TomongusHD*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_TomongusTuesday)
	{
		if (stage.song_step == 196)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 983)
		{
			this->character.focus_x = FIXED_DEC(-36,1);
			this->character.focus_y = FIXED_DEC(-91,1);
			this->character.focus_zoom = FIXED_DEC(1162,1024);
		}
	}
	
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2) && (character->animatable.anim != CharAnim_Special3))
	{
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_TomongusTuesday: //gets shot like a loser
				if (stage.song_step == 987)
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 993)
					character->set_anim(character, CharAnim_Special2);
				if (stage.song_step == 1009)
					character->set_anim(character, CharAnim_Special3);
				break;
			default:
				break;
		}
	if (stage.song_step == 1000)
		Animatable_Init(&this->character.animatable, char_tomongushd_anim2);
		
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_TomongusHD_SetFrame);
	Character_Draw(character, &this->tex, &char_tomongushd_frame[this->frame]);
}

void Char_TomongusHD_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_TomongusHD_Free(Character *character)
{
	Char_TomongusHD *this = (Char_TomongusHD*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_TomongusHD_New(fixed_t x, fixed_t y)
{
	//Allocate tomongushd object
	Char_TomongusHD *this = Mem_Alloc(sizeof(Char_TomongusHD));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_TomongusHD_New] Failed to allocate tomongushd object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_TomongusHD_Tick;
	this->character.set_anim = Char_TomongusHD_SetAnim;
	this->character.free = Char_TomongusHD_Free;
	
	Animatable_Init(&this->character.animatable, char_tomongushd_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFFFF4C61;
	
	this->character.focus_x = FIXED_DEC(-72,1);
	this->character.focus_y = FIXED_DEC(-88,1);
	this->character.focus_zoom = FIXED_DEC(1357,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\TOMONGHD.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "prep0.tim",
  "shot0.tim",
  "shot1.tim",
  "shot2.tim",
  "shot3.tim",
  "shot4.tim",
  "shot5.tim",
  "shot6.tim",
  "hampter0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
