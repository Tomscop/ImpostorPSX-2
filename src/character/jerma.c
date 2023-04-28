/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jerma.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Jerma character structure
enum
{
  Jerma_ArcMain_Idle0,
  Jerma_ArcMain_Idle1,
  Jerma_ArcMain_Idle2,
  Jerma_ArcMain_Left0,
  Jerma_ArcMain_Left1,
  Jerma_ArcMain_Down0,
  Jerma_ArcMain_Down1,
  Jerma_ArcMain_Up0,
  Jerma_ArcMain_Up1,
  Jerma_ArcMain_Right0,
  Jerma_ArcMain_Right1,
	
	Jerma_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Jerma_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Jerma;

//Jerma character definitions
static const CharFrame char_jerma_frame[] = {
  {Jerma_ArcMain_Idle0, {  0,  0,100,219}, { 93,119}}, //0 idle 1
  {Jerma_ArcMain_Idle0, {102,  0,100,218}, { 91,118}}, //1 idle 2
  {Jerma_ArcMain_Idle1, {  0,  0, 95,221}, { 91,121}}, //2 idle 3
  {Jerma_ArcMain_Idle1, { 97,  0, 93,222}, { 92,122}}, //3 idle 4
  {Jerma_ArcMain_Idle2, {  0,  0, 91,222}, { 91,122}}, //4 idle 5

  {Jerma_ArcMain_Left0, {  0,  0,192,160}, {183, 60}}, //5 left 1
  {Jerma_ArcMain_Left1, {  0,  0,190,161}, {181, 61}}, //6 left 2

  {Jerma_ArcMain_Down0, {  0,  0,164,178}, {125, 78}}, //7 down 1
  {Jerma_ArcMain_Down1, {  0,  0,162,180}, {124, 79}}, //8 down 2

  {Jerma_ArcMain_Up0, {  0,  0,150,252}, {111,152}}, //9 up 1
  {Jerma_ArcMain_Up1, {  0,  0,154,249}, {114,149}}, //10 up 2

  {Jerma_ArcMain_Right0, {  0,  0,206,203}, {105,103}}, //11 right 1
  {Jerma_ArcMain_Right1, {  0,  0,202,204}, {106,104}}, //12 right 2
};

static const Animation char_jerma_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Jerma character functions
void Char_Jerma_SetFrame(void *user, u8 frame)
{
	Char_Jerma *this = (Char_Jerma*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_jerma_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Jerma_Tick(Character *character)
{
	Char_Jerma *this = (Char_Jerma*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Jerma_SetFrame);
	Character_Draw(character, &this->tex, &char_jerma_frame[this->frame]);
}

void Char_Jerma_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Jerma_Free(Character *character)
{
	Char_Jerma *this = (Char_Jerma*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Jerma_New(fixed_t x, fixed_t y)
{
	//Allocate jerma object
	Char_Jerma *this = Mem_Alloc(sizeof(Char_Jerma));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Jerma_New] Failed to allocate jerma object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Jerma_Tick;
	this->character.set_anim = Char_Jerma_SetAnim;
	this->character.free = Char_Jerma_Free;
	
	Animatable_Init(&this->character.animatable, char_jerma_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 7;

	//health bar color
	this->character.health_bar = 0xFF5EC2C6;
	
	this->character.focus_x = FIXED_DEC(5,1);
	this->character.focus_y = FIXED_DEC(-50,1);
	this->character.focus_zoom = FIXED_DEC(155,128);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\JERMA.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
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
