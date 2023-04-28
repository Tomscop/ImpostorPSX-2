/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tobyfox.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Toby Fox character structure
enum
{
  TobyFox_ArcMain_Idle0,
  TobyFox_ArcMain_Idle1,
  TobyFox_ArcMain_Left0,
  TobyFox_ArcMain_Down0,
  TobyFox_ArcMain_Up0,
  TobyFox_ArcMain_Up1,
  TobyFox_ArcMain_Right0,
  TobyFox_ArcMain_Right1,
	
	TobyFox_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[TobyFox_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_TobyFox;

//Toby Fox character definitions
static const CharFrame char_tobyfox_frame[] = {
  {TobyFox_ArcMain_Idle0, {  0,  0,106,129}, { 49,  -199}}, //0 idle 1
  {TobyFox_ArcMain_Idle0, {107,  0, 88,159}, { 39,  -170}}, //1 idle 2
  {TobyFox_ArcMain_Idle1, {  0,  0, 94,155}, { 42,  -174}}, //2 idle 3

  {TobyFox_ArcMain_Left0, {  0,  0,198, 90}, {145,  -239}}, //3 left 1
  {TobyFox_ArcMain_Left0, {  0, 91,196,100}, {140,  -230}}, //4 left 2

  {TobyFox_ArcMain_Down0, {  0,  0,187, 71}, { 95,  -256}}, //5 down 1
  {TobyFox_ArcMain_Down0, {  0, 72,175, 80}, { 89,  -248}}, //6 down 2

  {TobyFox_ArcMain_Up0, {  0,  0,160,186}, { 82,  -141}}, //7 up 1
  {TobyFox_ArcMain_Up1, {  0,  0,179,180}, { 91,  -148}}, //8 up 2

  {TobyFox_ArcMain_Right0, {  0,  0,155,146}, { 42,  -182}}, //9 right 1
  {TobyFox_ArcMain_Right1, {  0,  0,144,149}, { 41,  -179}}, //10 right 2
};

static const Animation char_tobyfox_anim[CharAnim_Max] = {
	{3, (const u8[]){ 0, 1, 2, ASCR_BACK, 1}}, //CharAnim_Idle
	{3, (const u8[]){ 3, 4, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Toby Fox character functions
void Char_TobyFox_SetFrame(void *user, u8 frame)
{
	Char_TobyFox *this = (Char_TobyFox*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tobyfox_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_TobyFox_Tick(Character *character)
{
	Char_TobyFox *this = (Char_TobyFox*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_TobyFox_SetFrame);
	Character_Draw(character, &this->tex, &char_tobyfox_frame[this->frame]);
}

void Char_TobyFox_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_TobyFox_Free(Character *character)
{
	Char_TobyFox *this = (Char_TobyFox*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_TobyFox_New(fixed_t x, fixed_t y)
{
	//Allocate tobyfox object
	Char_TobyFox *this = Mem_Alloc(sizeof(Char_TobyFox));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_TobyFox_New] Failed to allocate tobyfox object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_TobyFox_Tick;
	this->character.set_anim = Char_TobyFox_SetAnim;
	this->character.free = Char_TobyFox_Free;
	
	Animatable_Init(&this->character.animatable, char_tobyfox_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 9;

	//health bar color
	this->character.health_bar = 0xFFFF8CB1;
	
	this->character.focus_x = FIXED_DEC(213,1);
	this->character.focus_y = FIXED_DEC(161,1);
	this->character.focus_zoom = FIXED_DEC(121,256);
	
	this->character.size = FIXED_DEC(8,10);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\TOBYFOX.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
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
