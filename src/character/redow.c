/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "redow.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Red Ow character structure
enum
{
  RedOw_ArcMain_Idle0,
  RedOw_ArcMain_Left0,
  RedOw_ArcMain_Down0,
  RedOw_ArcMain_Up0,
  RedOw_ArcMain_Right0,
  RedOw_ArcMain_Right1,
	
	RedOw_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RedOw_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RedOw;

//Red Ow character definitions
static const CharFrame char_redow_frame[] = {
  {RedOw_ArcMain_Idle0, {  0,  0, 65, 60}, {166, 90}}, //0 idle 1
  {RedOw_ArcMain_Idle0, { 65,  0, 60, 62}, {166, 92}}, //1 idle 2
  {RedOw_ArcMain_Idle0, {125,  0, 51, 72}, {164,102}}, //2 idle 3
  {RedOw_ArcMain_Idle0, {176,  0, 56, 68}, {166, 98}}, //3 idle 4
  {RedOw_ArcMain_Idle0, {  0, 72, 54, 70}, {164,100}}, //4 idle 5
  {RedOw_ArcMain_Idle0, { 54, 72, 55, 69}, {165, 99}}, //5 idle 6

  {RedOw_ArcMain_Left0, {  0,  0,101, 76}, {171,104}}, //6 left 1
  {RedOw_ArcMain_Left0, {101,  0, 97, 76}, {165,103}}, //7 left 2
  {RedOw_ArcMain_Left0, {  0, 76, 75, 80}, {165,108}}, //8 left 3
  {RedOw_ArcMain_Left0, { 75, 76, 74, 80}, {165,108}}, //9 left 4

  {RedOw_ArcMain_Down0, {  0,  0, 91, 81}, {159,109}}, //10 down 1
  {RedOw_ArcMain_Down0, { 91,  0, 85, 82}, {157,110}}, //11 down 2
  {RedOw_ArcMain_Down0, {  0, 82, 80, 83}, {157,111}}, //12 down 3
  {RedOw_ArcMain_Down0, { 80, 82, 80, 83}, {157,111}}, //13 down 4

  {RedOw_ArcMain_Up0, {  0,  0, 47, 78}, {162,106}}, //14 up 1
  {RedOw_ArcMain_Up0, { 47,  0, 50, 74}, {163,102}}, //15 up 2
  {RedOw_ArcMain_Up0, { 97,  0, 51, 73}, {164,101}}, //16 up 3
  {RedOw_ArcMain_Up0, {148,  0, 50, 73}, {163,101}}, //17 up 4

  {RedOw_ArcMain_Right0, {  0,  0,127, 65}, {156, 94}}, //18 right 1
  {RedOw_ArcMain_Right0, {  0, 65,133, 66}, {156, 95}}, //19 right 2
  {RedOw_ArcMain_Right0, {  0,131,145, 72}, {158,101}}, //20 right 3
  {RedOw_ArcMain_Right1, {  0,  0,147, 76}, {158,105}}, //21 right 4
  {RedOw_ArcMain_Right1, {147,  0, 60, 65}, {158, 94}}, //22 right 5
};

static const Animation char_redow_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 6, 6, 7, 7, 8, 8, 9, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 10, 10, 11, 11, 12, 12, 13, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 14, 14, 15, 15, 16, 16, 17, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 18, 19, 20, 20, 21, 21, 22, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Red Ow character functions
void Char_RedOw_SetFrame(void *user, u8 frame)
{
	Char_RedOw *this = (Char_RedOw*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_redow_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RedOw_Tick(Character *character)
{
	Char_RedOw *this = (Char_RedOw*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_RedOw_SetFrame);
	Character_Draw(character, &this->tex, &char_redow_frame[this->frame]);
}

void Char_RedOw_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RedOw_Free(Character *character)
{
	Char_RedOw *this = (Char_RedOw*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RedOw_New(fixed_t x, fixed_t y)
{
	//Allocate redow object
	Char_RedOw *this = Mem_Alloc(sizeof(Char_RedOw));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RedOw_New] Failed to allocate redow object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RedOw_Tick;
	this->character.set_anim = Char_RedOw_SetAnim;
	this->character.free = Char_RedOw_Free;
	
	Animatable_Init(&this->character.animatable, char_redow_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFCD0000;
	
	this->character.focus_x = FIXED_DEC(-131,1);
	this->character.focus_y = FIXED_DEC(-108,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(125,100);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\REDOW.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
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
