/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tomongus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Tomongus character structure
enum
{
  Tomongus_ArcMain_Idle0,
  Tomongus_ArcMain_Left0,
  Tomongus_ArcMain_Down0,
  Tomongus_ArcMain_Up0,
  Tomongus_ArcMain_Right0,
  Tomongus_ArcMain_IdleH0,
  Tomongus_ArcMain_LeftH0,
  Tomongus_ArcMain_DownH0,
  Tomongus_ArcMain_UpH0,
  Tomongus_ArcMain_RightH0,
	
	Tomongus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Tomongus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Tomongus;

//Tomongus character definitions
static const CharFrame char_tomongus_frame[] = {
  {Tomongus_ArcMain_Idle0, {  0,  0, 45, 38}, {147,141}}, //0 idle 1
  {Tomongus_ArcMain_Idle0, { 46,  0, 41, 40}, {145,143}}, //1 idle 2
  {Tomongus_ArcMain_Idle0, { 88,  0, 39, 41}, {144,144}}, //2 idle 3
  {Tomongus_ArcMain_Idle0, {128,  0, 39, 42}, {144,145}}, //3 idle 4

  {Tomongus_ArcMain_Left0, {  0,  0, 48, 44}, {147,147}}, //4 left 1
  {Tomongus_ArcMain_Left0, { 49,  0, 48, 42}, {146,145}}, //5 left 2

  {Tomongus_ArcMain_Down0, {  0,  0, 45, 37}, {147,140}}, //6 down 1
  {Tomongus_ArcMain_Down0, { 46,  0, 43, 38}, {146,141}}, //7 down 2

  {Tomongus_ArcMain_Up0, {  0,  0, 37, 47}, {143,150}}, //8 up 1
  {Tomongus_ArcMain_Up0, { 38,  0, 37, 46}, {143,149}}, //9 up 2

  {Tomongus_ArcMain_Right0, {  0,  0, 48, 43}, {149,148}}, //10 right 1
  {Tomongus_ArcMain_Right0, { 49,  0, 48, 43}, {150,148}}, //11 right 2

  {Tomongus_ArcMain_IdleH0, {  0,  0, 45, 33}, {147,140-4}}, //12 idleh 1
  {Tomongus_ArcMain_IdleH0, { 46,  0, 41, 34}, {145,141-4}}, //13 idleh 2
  {Tomongus_ArcMain_IdleH0, { 88,  0, 39, 36}, {144,143-4}}, //14 idleh 3
  {Tomongus_ArcMain_IdleH0, {128,  0, 39, 38}, {144,145-4}}, //15 idleh 4

  {Tomongus_ArcMain_LeftH0, {  0,  0, 51, 41}, {146,148-4}}, //16 lefth 1
  {Tomongus_ArcMain_LeftH0, { 52,  0, 51, 41}, {145,148-4}}, //17 lefth 2
  {Tomongus_ArcMain_LeftH0, {104,  0, 48, 37}, {145,144-4}}, //18 lefth 3

  {Tomongus_ArcMain_DownH0, {  0,  0, 50, 33}, {147,140-4}}, //19 downh 1
  {Tomongus_ArcMain_DownH0, { 51,  0, 49, 34}, {146,141-4}}, //20 downh 2
  {Tomongus_ArcMain_DownH0, {101,  0, 43, 34}, {146,141-4}}, //21 downh 3

  {Tomongus_ArcMain_UpH0, {  0,  0, 45, 43}, {143,150-4}}, //22 uph 1
  {Tomongus_ArcMain_UpH0, { 46,  0, 45, 42}, {143,149-4}}, //23 uph 2
  {Tomongus_ArcMain_UpH0, { 92,  0, 37, 41}, {143,148-4}}, //24 uph 3

  {Tomongus_ArcMain_RightH0, {  0,  0, 59, 37}, {149,146-4}}, //25 righth 1
  {Tomongus_ArcMain_RightH0, { 60,  0, 59, 37}, {150,146-4}}, //26 righth 2
  {Tomongus_ArcMain_RightH0, {120,  0, 46, 37}, {150,146-4}}, //27 righth 3
};

static const Animation char_tomongus_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_tomongus_anim2[CharAnim_Max] = {
	{2, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 16, 17, 18, 18, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 19, 20, 21, 21, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 22, 23, 24, 24, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 25, 26, 27, 27, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{2, (const u8[]){ 15, ASCR_BACK, 1}}, //CharAnim_Special1
};

//Tomongus character functions
void Char_Tomongus_SetFrame(void *user, u8 frame)
{
	Char_Tomongus *this = (Char_Tomongus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tomongus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Tomongus_Tick(Character *character)
{
	Char_Tomongus *this = (Char_Tomongus*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	if (((stage.stage_id == StageId_Rivals) && (stage.song_step == 1034)) || ((stage.stage_id == StageId_Chewmate) && (stage.song_step == -29)))
	{
		Animatable_Init(&this->character.animatable, char_tomongus_anim2);
		this->character.health_bar = 0xFFF8A572;
		this->character.health_i = 2;
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Rivals: //Baka
				if ((stage.song_step == 1034) && stage.flag & STAGE_FLAG_JUST_STEP)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Tomongus_SetFrame);
	Character_Draw(character, &this->tex, &char_tomongus_frame[this->frame]);
}

void Char_Tomongus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Tomongus_Free(Character *character)
{
	Char_Tomongus *this = (Char_Tomongus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Tomongus_New(fixed_t x, fixed_t y)
{
	//Allocate tomongus object
	Char_Tomongus *this = Mem_Alloc(sizeof(Char_Tomongus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Tomongus_New] Failed to allocate tomongus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Tomongus_Tick;
	this->character.set_anim = Char_Tomongus_SetAnim;
	this->character.free = Char_Tomongus_Free;
	
	if (stage.stage_id != StageId_Chewmate)
		Animatable_Init(&this->character.animatable, char_tomongus_anim);
	else
		Animatable_Init(&this->character.animatable, char_tomongus_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	if (stage.stage_id != StageId_Chewmate)
		this->character.health_i = 1;
	else
		this->character.health_i = 2;

	//health bar color
	if (stage.stage_id != StageId_Chewmate)
		this->character.health_bar = 0xFFFF6770;
	else
		this->character.health_bar = 0xFFF8A572;
	
	this->character.focus_x = FIXED_DEC(-165,1);
	this->character.focus_y = FIXED_DEC(-302,1);
	this->character.focus_zoom = FIXED_DEC(511,512);
	
	this->character.size = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\TOMONGUS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "idleh0.tim",
  "lefth0.tim",
  "downh0.tim",
  "uph0.tim",
  "righth0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
