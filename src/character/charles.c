/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "charles.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Charles character structure
enum
{
  Charles_ArcMain_Idle0,
  Charles_ArcMain_Idle1,
  Charles_ArcMain_Idle2,
  Charles_ArcMain_Idle3,
  Charles_ArcMain_Idle4,
  Charles_ArcMain_Left0,
  Charles_ArcMain_Left1,
  Charles_ArcMain_Down0,
  Charles_ArcMain_Down1,
  Charles_ArcMain_Up0,
  Charles_ArcMain_Up1,
  Charles_ArcMain_Right0,
  Charles_ArcMain_Right1,
  Charles_ArcMain_Henry0,
  Charles_ArcMain_Henry1,
  Charles_ArcMain_Henry2,
  Charles_ArcMain_Henry3,
  Charles_ArcMain_Henry4,
  Charles_ArcMain_Henry5,
  Charles_ArcMain_Henry6,
  Charles_ArcMain_Henry7,
  Charles_ArcMain_Plan0,
  Charles_ArcMain_Plan1,
  Charles_ArcMain_Plan2,
  Charles_ArcMain_Plan3,
  Charles_ArcMain_Plan4,
  Charles_ArcMain_Plan5,
  Charles_ArcMain_Plan6,
  Charles_ArcMain_Plan7,
  Charles_ArcMain_Plan8,
  Charles_ArcMain_Plan9,
  Charles_ArcMain_Plan10,
  Charles_ArcMain_Plan11,
  Charles_ArcMain_Plan12,
	
	Charles_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Charles_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Charles;

//Charles character definitions
static const CharFrame char_charles_frame[] = {
  {Charles_ArcMain_Idle0, {  0,  0,198,169}, {160,160}}, //0 idle 1
  {Charles_ArcMain_Idle1, {  0,  0,197,169}, {160,160}}, //1 idle 2
  {Charles_ArcMain_Idle2, {  0,  0,197,169}, {159,160}}, //2 idle 3
  {Charles_ArcMain_Idle3, {  0,  0,197,169}, {160,160}}, //3 idle 4
  {Charles_ArcMain_Idle4, {  0,  0,197,169}, {160,160}}, //4 idle 5

  {Charles_ArcMain_Left0, {  0,  0,197,169}, {159,160}}, //5 left 1
  {Charles_ArcMain_Left1, {  0,  0,197,169}, {160,159}}, //6 left 2

  {Charles_ArcMain_Down0, {  0,  0,197,169}, {160,160}}, //7 down 1
  {Charles_ArcMain_Down1, {  0,  0,197,169}, {160,160}}, //8 down 2

  {Charles_ArcMain_Up0, {  0,  0,197,169}, {160,159}}, //9 up 1
  {Charles_ArcMain_Up1, {  0,  0,197,169}, {160,159}}, //10 up 2

  {Charles_ArcMain_Right0, {  0,  0,197,169}, {160,160}}, //11 right 1
  {Charles_ArcMain_Right1, {  0,  0,197,169}, {159,160}}, //12 right 2

  {Charles_ArcMain_Henry0, {  0,  0,197,169}, {160,159}}, //13 henry 1
  {Charles_ArcMain_Henry1, {  0,  0,198,168}, {160,159}}, //14 henry 2
  {Charles_ArcMain_Henry2, {  0,  0,197,169}, {160,159}}, //15 henry 3
  {Charles_ArcMain_Henry3, {  0,  0,197,169}, {159,159}}, //16 henry 4
  {Charles_ArcMain_Henry4, {  0,  0,197,168}, {160,159}}, //17 henry 5
  {Charles_ArcMain_Henry5, {  0,  0,197,169}, {160,159}}, //18 henry 6
  {Charles_ArcMain_Henry6, {  0,  0,197,169}, {159,159}}, //19 henry 7
  {Charles_ArcMain_Henry7, {  0,  0,197,169}, {160,160}}, //20 henry 8

  {Charles_ArcMain_Plan0, {  0,  0,197,169}, {160,160}}, //21 plan 1
  {Charles_ArcMain_Plan1, {  0,  0,197,169}, {160,160}}, //22 plan 2
  {Charles_ArcMain_Plan2, {  0,  0,197,169}, {160,160}}, //23 plan 3
  {Charles_ArcMain_Plan3, {  0,  0,197,169}, {159,160}}, //24 plan 4
  {Charles_ArcMain_Plan4, {  0,  0,197,169}, {160,160}}, //25 plan 5
  {Charles_ArcMain_Plan5, {  0,  0,197,169}, {160,160}}, //26 plan 6
  {Charles_ArcMain_Plan6, {  0,  0,197,169}, {159,160}}, //27 plan 7
  {Charles_ArcMain_Plan7, {  0,  0,197,169}, {160,160}}, //28 plan 8
  {Charles_ArcMain_Plan8, {  0,  0,197,169}, {160,160}}, //29 plan 9
  {Charles_ArcMain_Plan9, {  0,  0,198,168}, {160,159}}, //30 plan 10
  {Charles_ArcMain_Plan10, {  0,  0,197,169}, {160,160}}, //31 plan 11
  {Charles_ArcMain_Plan11, {  0,  0,197,169}, {159,160}}, //32 plan 12
  {Charles_ArcMain_Plan12, {  0,  0,198,168}, {160,159}}, //33 plan 13
};

static const Animation char_charles_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6,  ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt

	{1, (const u8[]){13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 20, 20, 20, 20, ASCR_BACK, 1}},   //CharAnim_Special1
	{1, (const u8[]){21, 21, 22, 23, 24, 24, 25, 23, 23, 26, 27, 28, 28, 28, 29, 29, 30, 30, 31, 32, 23, 23, 33, 30, 30, 30, ASCR_BACK, 1}},   //CharAnim_Special2
};

//Charles character functions
void Char_Charles_SetFrame(void *user, u8 frame)
{
	Char_Charles *this = (Char_Charles*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_charles_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Charles_Tick(Character *character)
{
	Char_Charles *this = (Char_Charles*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Charles_SetFrame);
	Character_Draw(character, &this->tex, &char_charles_frame[this->frame]);
}

void Char_Charles_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Charles_Free(Character *character)
{
	Char_Charles *this = (Char_Charles*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Charles_New(fixed_t x, fixed_t y)
{
	//Allocate charles object
	Char_Charles *this = Mem_Alloc(sizeof(Char_Charles));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Charles_New] Failed to allocate charles object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Charles_Tick;
	this->character.set_anim = Char_Charles_SetAnim;
	this->character.free = Char_Charles_Free;
	
	Animatable_Init(&this->character.animatable, char_charles_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFBDD7D8;
	
	this->character.focus_x = FIXED_DEC(5,1);
	this->character.focus_y = FIXED_DEC(-42,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\CHARLES.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "henry0.tim",
  "henry1.tim",
  "henry2.tim",
  "henry3.tim",
  "henry4.tim",
  "henry5.tim",
  "henry6.tim",
  "henry7.tim",
  "plan0.tim",
  "plan1.tim",
  "plan2.tim",
  "plan3.tim",
  "plan4.tim",
  "plan5.tim",
  "plan6.tim",
  "plan7.tim",
  "plan8.tim",
  "plan9.tim",
  "plan10.tim",
  "plan11.tim",
  "plan12.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}