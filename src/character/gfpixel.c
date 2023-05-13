/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfpixel.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "speaker.h"

//GF Pixel character structure
enum
{
  GFPixel_ArcMain_GF0,
  GFPixel_ArcMain_GF1,
  GFPixel_ArcMain_GF2,
	
	GFPixel_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFPixel_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFPixel;

//GF character definitions
static const CharFrame char_gfpixel_frame[] = {
  {GFPixel_ArcMain_GF0, {  0,  0,108, 99}, {160,160}}, //0 gf 1
  {GFPixel_ArcMain_GF0, {121,  0,108, 99}, {160,160}}, //1 gf 2
  {GFPixel_ArcMain_GF0, {  0,105,108, 99}, {160,160}}, //2 gf 3
  {GFPixel_ArcMain_GF0, {121,105,108, 99}, {160,160}}, //3 gf 4
  {GFPixel_ArcMain_GF1, {  0,  0,108, 99}, {160,160}}, //4 gf 5
  {GFPixel_ArcMain_GF1, {121,  0,108, 99}, {160,160}}, //5 gf 6
  {GFPixel_ArcMain_GF1, {  0,105,108, 99}, {160,160}}, //6 gf 7
  {GFPixel_ArcMain_GF1, {121,105,108, 99}, {160,160}}, //7 gf 8
  {GFPixel_ArcMain_GF2, {  0,  0,108, 99}, {160,160}}, //8 gf 9
};

static const Animation char_gfpixel_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Idle
	{1, (const u8[]){ 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, ASCR_BACK, 1}}, //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_UpAlt
	{1, (const u8[]){ 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, ASCR_BACK, 1}}, //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Left}},                           //CharAnim_RightAlt
};

//GF Pixel character functions
void Char_GFPixel_SetFrame(void *user, u8 frame)
{
	Char_GFPixel *this = (Char_GFPixel*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfpixel_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFPixel_Tick(Character *character)
{
	Char_GFPixel *this = (Char_GFPixel*)character;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform dance
		if ((stage.song_step % stage.gf_speed) == 0)
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_Left)
				character->set_anim(character, CharAnim_Right);
			else
				character->set_anim(character, CharAnim_Left);
		}
	}
	
	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFPixel_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfpixel_frame[this->frame], parallax);
}

void Char_GFPixel_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim != CharAnim_Idle && anim != CharAnim_Left && anim != CharAnim_Right)
		return;
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFPixel_Free(Character *character)
{
	Char_GFPixel *this = (Char_GFPixel*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFPixel_New(fixed_t x, fixed_t y)
{
	//Allocate gf pixel object
	Char_GFPixel *this = Mem_Alloc(sizeof(Char_GFPixel));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFPixel_New] Failed to allocate gf pixel object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFPixel_Tick;
	this->character.set_anim = Char_GFPixel_SetAnim;
	this->character.free = Char_GFPixel_Free;
	
	Animatable_Init(&this->character.animatable, char_gfpixel_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(16,1);
	this->character.focus_y = FIXED_DEC(-50,1);
	this->character.focus_zoom = FIXED_DEC(13,10);
	
	this->character.size = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\GF\\GFPIXEL.ARC;1");
	
	const char **pathp = (const char *[]){
		"gf0.tim",
		"gf1.tim",
		"gf2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
