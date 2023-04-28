/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfv1.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFV1 character structure
enum
{
  GFV1_ArcMain_GF0,
  GFV1_ArcMain_GF1,
  GFV1_ArcMain_GF2,
  GFV1_ArcMain_GF3,
  GFV1_ArcMain_GF4,
  GFV1_ArcMain_GF5,
	
	GFV1_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFV1_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFV1;

//GFV1 character definitions
static const CharFrame char_gfv1_frame[] = {
  {GFV1_ArcMain_GF0, {  0,  0,176,125}, {159,120}}, //0 gf 1
  {GFV1_ArcMain_GF0, {  0,125,177,125}, {160,120}}, //1 gf 2
  {GFV1_ArcMain_GF1, {  0,  0,176,125}, {159,120}}, //2 gf 3
  {GFV1_ArcMain_GF1, {  0,125,176,125}, {159,120}}, //3 gf 4
  {GFV1_ArcMain_GF2, {  0,  0,176,125}, {159,120}}, //4 gf 5
  {GFV1_ArcMain_GF2, {  0,125,176,125}, {160,121}}, //5 gf 6
  
  {GFV1_ArcMain_GF3, {  0,  0,176,124}, {159,119}}, //6 gf 7
  {GFV1_ArcMain_GF3, {  0,124,177,124}, {160,119}}, //7 gf 8
  {GFV1_ArcMain_GF4, {  0,  0,176,124}, {159,119}}, //8 gf 9
  {GFV1_ArcMain_GF4, {  0,124,176,124}, {159,119}}, //9 gf 10
  {GFV1_ArcMain_GF5, {  0,  0,176,124}, {159,119}}, //10 gf 11
};

static const Animation char_gfv1_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{1, (const u8[]){ 0, 1, 1, 2, 3, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{1, (const u8[]){ 6, 7, 7, 8, 9, 10, 10, 5, 5, 5, 5, 5, 5, 5, 5, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFV1 character functions
void Char_GFV1_SetFrame(void *user, u8 frame)
{
	Char_GFV1 *this = (Char_GFV1*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfv1_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFV1_Tick(Character *character)
{
	Char_GFV1 *this = (Char_GFV1*)character;
	
	//Initialize Pico test
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{			
		//Perform dance
	    if ((stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0) && (character->animatable.anim  != CharAnim_Special1))
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
				character->set_anim(character, CharAnim_RightAlt);
			else
				character->set_anim(character, CharAnim_LeftAlt);
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFV1_SetFrame);
	Character_Draw(character, &this->tex, &char_gfv1_frame[this->frame]);
}

void Char_GFV1_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFV1_Free(Character *character)
{
	Char_GFV1 *this = (Char_GFV1*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFV1_New(fixed_t x, fixed_t y)
{
	//Allocate gfv1 object
	Char_GFV1 *this = Mem_Alloc(sizeof(Char_GFV1));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFV1_New] Failed to allocate gfv1 object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFV1_Tick;
	this->character.set_anim = Char_GFV1_SetAnim;
	this->character.free = Char_GFV1_Free;
	
	Animatable_Init(&this->character.animatable, char_gfv1_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFA5004A;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\GF\\GFV1.ARC;1");
		
	const char **pathp = (const char *[]){
  "gf0.tim",
  "gf1.tim",
  "gf2.tim",
  "gf3.tim",
  "gf4.tim",
  "gf5.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
