/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "fabs.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Fabs character structure
enum
{
  Fabs_ArcMain_Idle0,
  Fabs_ArcMain_Idle1,
  Fabs_ArcMain_Idle2,
  Fabs_ArcMain_Left0,
  Fabs_ArcMain_Left1,
  Fabs_ArcMain_Down0,
  Fabs_ArcMain_Up0,
  Fabs_ArcMain_Right0,
  Fabs_ArcMain_Right1,
	
	Fabs_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Fabs_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Fabs;

//Fabs character definitions
static const CharFrame char_fabs_frame[] = {
  {Fabs_ArcMain_Idle0, {  0,  0,115,177}, {159,148}}, //0 idle 1
  {Fabs_ArcMain_Idle0, {115,  0,110,176}, {154,148}}, //1 idle 2
  {Fabs_ArcMain_Idle1, {  0,  0,109,178}, {156,150}}, //2 idle 3
  {Fabs_ArcMain_Idle1, {109,  0,112,179}, {159,151}}, //3 idle 4
  {Fabs_ArcMain_Idle2, {  0,  0,113,179}, {160,151}}, //4 idle 5

  {Fabs_ArcMain_Left0, {  0,  0,161,161}, {204,135}}, //5 left 1
  {Fabs_ArcMain_Left1, {  0,  0,160,163}, {198,137}}, //6 left 2

  {Fabs_ArcMain_Down0, {  0,  0,160, 82}, {136, 63}}, //7 down 1
  {Fabs_ArcMain_Down0, {  0, 82,148, 92}, {131, 73}}, //8 down 2

  {Fabs_ArcMain_Up0, {  0,  0, 80,196}, {135,166}}, //9 up 1
  {Fabs_ArcMain_Up0, { 80,  0, 94,180}, {147,152}}, //10 up 2

  {Fabs_ArcMain_Right0, {  0,  0,147,128}, {139,109}}, //11 right 1
  {Fabs_ArcMain_Right1, {  0,  0,138,147}, {140,128}}, //12 right 2
};

static const Animation char_fabs_anim[CharAnim_Max] = {
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

//Fabs character functions
void Char_Fabs_SetFrame(void *user, u8 frame)
{
	Char_Fabs *this = (Char_Fabs*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_fabs_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Fabs_Tick(Character *character)
{
	Char_Fabs *this = (Char_Fabs*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Fabs_SetFrame);
	if ((stage.song_step >= 1428) && (stage.song_step <= 1439))
		Character_DrawBlend(character, &this->tex, &char_fabs_frame[this->frame]);
	else
		Character_Draw(character, &this->tex, &char_fabs_frame[this->frame]);
}

void Char_Fabs_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Fabs_Free(Character *character)
{
	Char_Fabs *this = (Char_Fabs*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Fabs_New(fixed_t x, fixed_t y)
{
	//Allocate fabs object
	Char_Fabs *this = Mem_Alloc(sizeof(Char_Fabs));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Fabs_New] Failed to allocate fabs object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Fabs_Tick;
	this->character.set_anim = Char_Fabs_SetAnim;
	this->character.free = Char_Fabs_Free;
	
	Animatable_Init(&this->character.animatable, char_fabs_anim);
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
	this->arc_main = IO_Read("\\OPPONENT\\FABS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
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
