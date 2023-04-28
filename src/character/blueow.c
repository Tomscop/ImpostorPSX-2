/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "blueow.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Blue Ow player types
enum
{
  BlueOw_ArcMain_Idle0,
  BlueOw_ArcMain_Left0,
  BlueOw_ArcMain_Down0,
  BlueOw_ArcMain_Up0,
  BlueOw_ArcMain_Right0,
  BlueOw_ArcMain_LeftH0,
  BlueOw_ArcMain_DownH0,
  BlueOw_ArcMain_UpH0,
  BlueOw_ArcMain_RightH0,
	
	BlueOw_ArcMain_Max,
};

#define BlueOw_Arc_Max BlueOw_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BlueOw_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BlueOw;

//Blue Ow player definitions
static const CharFrame char_blueow_frame[] = {
  {BlueOw_ArcMain_Idle0, {  0,  0, 61, 64}, {166, 94}}, //0 idle 1
  {BlueOw_ArcMain_Idle0, { 61,  0, 60, 64}, {164, 94}}, //1 idle 2
  {BlueOw_ArcMain_Idle0, {121,  0, 59, 66}, {165, 96}}, //2 idle 3
  {BlueOw_ArcMain_Idle0, {180,  0, 56, 70}, {166,100}}, //3 idle 4
  {BlueOw_ArcMain_Idle0, {  0, 70, 55, 70}, {166,100}}, //4 idle 5
  {BlueOw_ArcMain_Idle0, { 55, 70, 55, 70}, {166,100}}, //5 idle 6

  {BlueOw_ArcMain_Left0, {  0,  0, 72, 68}, {169, 99}}, //6 left 1
  {BlueOw_ArcMain_Left0, { 72,  0, 75, 69}, {168,100}}, //7 left 2

  {BlueOw_ArcMain_Down0, {  0,  0, 75, 63}, {167, 94}}, //8 down 1
  {BlueOw_ArcMain_Down0, { 75,  0, 76, 65}, {166, 96}}, //9 down 2

  {BlueOw_ArcMain_Up0, {  0,  0, 65, 76}, {163,107}}, //10 up 1
  {BlueOw_ArcMain_Up0, { 65,  0, 69, 74}, {164,105}}, //11 up 2

  {BlueOw_ArcMain_Right0, {  0,  0, 92, 69}, {162,100}}, //12 right 1
  {BlueOw_ArcMain_Right0, { 92,  0, 91, 69}, {163,100}}, //13 right 2
  
  {BlueOw_ArcMain_LeftH0, {  0,  0, 42, 82}, {155,106+7}}, //14 lefth 1
  {BlueOw_ArcMain_LeftH0, { 42,  0, 49, 88}, {162,112+7}}, //15 lefth 2
  {BlueOw_ArcMain_LeftH0, { 91,  0, 56, 94}, {167,118+7}}, //16 lefth 3
  {BlueOw_ArcMain_LeftH0, {147,  0, 57, 94}, {168,118+7}}, //17 lefth 4
  {BlueOw_ArcMain_LeftH0, {  0, 94, 53, 89}, {163,113+7}}, //18 lefth 5
  {BlueOw_ArcMain_LeftH0, { 53, 94, 52, 74}, {164, 99+7}}, //19 lefth 6

  {BlueOw_ArcMain_DownH0, {  0,  0, 69, 61}, {171-2, 91+1}}, //20 downh 1
  {BlueOw_ArcMain_DownH0, { 69,  0, 67, 64}, {171-2, 94+1}}, //21 downh 2

  {BlueOw_ArcMain_UpH0, {  0,  0, 48, 87}, {164-2,121+1}}, //22 uph 1
  {BlueOw_ArcMain_UpH0, { 48,  0, 53, 77}, {163-2,111+1}}, //23 uph 2
  {BlueOw_ArcMain_UpH0, {101,  0, 55, 75}, {164-2,109+1}}, //24 uph 3

  {BlueOw_ArcMain_RightH0, {  0,  0, 51, 71}, {161-2,100+1}}, //25 righth 1
  {BlueOw_ArcMain_RightH0, { 51,  0, 50, 71}, {163-2,100+1}}, //26 righth 2
};

static const Animation char_blueow_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 6, 6, 7, ASCR_BACK, 1}},             //CharAnim_Left
	{1, (const u8[]){ 14, 15, 16, 16, 17, 17, 18, 18, 19, ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 8, 8, 9, ASCR_BACK, 1}},             //CharAnim_Down
	{1, (const u8[]){ 20, 20, 21, ASCR_BACK, 1}},       //CharAnim_DownAlt
	{1, (const u8[]){ 10, 10, 11, ASCR_BACK, 1}},             //CharAnim_Up
	{1, (const u8[]){ 22, 22, 23, 23, 24, ASCR_BACK, 1}},       //CharAnim_UpAlt
	{1, (const u8[]){ 12, 12, 13, ASCR_BACK, 1}},             //CharAnim_Right
	{1, (const u8[]){ 25, 25, 26, ASCR_BACK, 1}},       //CharAnim_RightAlt
};

//Blue Ow player functions
void Char_BlueOw_SetFrame(void *user, u8 frame)
{
	Char_BlueOw *this = (Char_BlueOw*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_blueow_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BlueOw_Tick(Character *character)
{
	Char_BlueOw *this = (Char_BlueOw*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BlueOw_SetFrame);
	Character_Draw(character, &this->tex, &char_blueow_frame[this->frame]);
}

void Char_BlueOw_SetAnim(Character *character, u8 anim)
{
	Char_BlueOw *this = (Char_BlueOw*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BlueOw_Free(Character *character)
{
	Char_BlueOw *this = (Char_BlueOw*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BlueOw_New(fixed_t x, fixed_t y)
{
	//Allocate blueow object
	Char_BlueOw *this = Mem_Alloc(sizeof(Char_BlueOw));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BlueOw_New] Failed to allocate blueow object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BlueOw_Tick;
	this->character.set_anim = Char_BlueOw_SetAnim;
	this->character.free = Char_BlueOw_Free;
	
	Animatable_Init(&this->character.animatable, char_blueow_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;
	
	//health bar color
	this->character.health_bar = 0xFF2EC9FC;
	
	this->character.focus_x = FIXED_DEC(-212,1);
	this->character.focus_y = FIXED_DEC(-109,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	
	this->character.size = FIXED_DEC(125,100);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BLUEOW.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
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
