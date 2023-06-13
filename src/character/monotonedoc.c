/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "monotonedoc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//MonotoneDoc character structure
enum
{
  MonotoneDoc_ArcMain_Idle0,
  MonotoneDoc_ArcMain_Left0,
  MonotoneDoc_ArcMain_Down0,
  MonotoneDoc_ArcMain_Up0,
  MonotoneDoc_ArcMain_Right0,
	
	MonotoneDoc_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[MonotoneDoc_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_MonotoneDoc;

//MonotoneDoc character definitions
static const CharFrame char_monotonedoc_frame[] = {
  {MonotoneDoc_ArcMain_Idle0, {  0,  0, 80,117}, {153,135}}, //0 idle 1
  {MonotoneDoc_ArcMain_Idle0, { 80,  0, 75,116}, {152,134}}, //1 idle 2
  {MonotoneDoc_ArcMain_Idle0, {155,  0, 75,118}, {151,136}}, //2 idle 3
  {MonotoneDoc_ArcMain_Idle0, {  0,118, 76,120}, {151,137}}, //3 idle 4
  {MonotoneDoc_ArcMain_Idle0, { 76,118, 76,120}, {151,138}}, //4 idle 5
  {MonotoneDoc_ArcMain_Idle0, {152,118, 76,120}, {151,138}}, //5 idle 6

  {MonotoneDoc_ArcMain_Left0, {  0,  0, 95,106}, {180,124}}, //6 left 1
  {MonotoneDoc_ArcMain_Left0, { 95,  0, 95,106}, {178,125}}, //7 left 2

  {MonotoneDoc_ArcMain_Down0, {  0,  0,130, 89}, {158,109}}, //8 down 1
  {MonotoneDoc_ArcMain_Down0, {  0, 89,127, 93}, {159,113}}, //9 down 2

  {MonotoneDoc_ArcMain_Up0, {  0,  0, 74,145}, {128,165}}, //10 up 1
  {MonotoneDoc_ArcMain_Up0, { 74,  0, 71,143}, {126,163}}, //11 up 2

  {MonotoneDoc_ArcMain_Right0, {  0,  0, 90,112}, {141,130}}, //12 right 1
  {MonotoneDoc_ArcMain_Right0, { 90,  0, 87,111}, {141,131}}, //13 right 2
};

static const Animation char_monotonedoc_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//MonotoneDoc character functions
void Char_MonotoneDoc_SetFrame(void *user, u8 frame)
{
	Char_MonotoneDoc *this = (Char_MonotoneDoc*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_monotonedoc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_MonotoneDoc_Tick(Character *character)
{
	Char_MonotoneDoc *this = (Char_MonotoneDoc*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_MonotoneDoc_SetFrame);
	Character_Draw(character, &this->tex, &char_monotonedoc_frame[this->frame]);
}

void Char_MonotoneDoc_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_MonotoneDoc_Free(Character *character)
{
	Char_MonotoneDoc *this = (Char_MonotoneDoc*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_MonotoneDoc_New(fixed_t x, fixed_t y)
{
	//Allocate monotonedoc object
	Char_MonotoneDoc *this = Mem_Alloc(sizeof(Char_MonotoneDoc));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_MonotoneDoc_New] Failed to allocate monotonedoc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_MonotoneDoc_Tick;
	this->character.set_anim = Char_MonotoneDoc_SetAnim;
	this->character.free = Char_MonotoneDoc_Free;
	
	Animatable_Init(&this->character.animatable, char_monotonedoc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFE0E0E0;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\MONODOC.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
