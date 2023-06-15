/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cval.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

int zoomoutcvp = 0;
//Cval character structure
enum
{
  Cval_ArcMain_Idle0,
  Cval_ArcMain_Idle1,
  Cval_ArcMain_Idle2,
  Cval_ArcMain_Idle3,
  Cval_ArcMain_Idle4,
  Cval_ArcMain_Idle5,
  Cval_ArcMain_Left0,
  Cval_ArcMain_Left1,
  Cval_ArcMain_Left2,
  Cval_ArcMain_Left3,
  Cval_ArcMain_Down0,
  Cval_ArcMain_Down1,
  Cval_ArcMain_Down2,
  Cval_ArcMain_Up0,
  Cval_ArcMain_Up1,
  Cval_ArcMain_Up2,
  Cval_ArcMain_Right0,
  Cval_ArcMain_Right1,
  Cval_ArcMain_Right2,
  Cval_ArcMain_Hi0,
  Cval_ArcMain_Hi1,
  Cval_ArcMain_Hi2,
  Cval_ArcMain_Hi3,
  Cval_ArcMain_Hi4,
  Cval_ArcMain_Hi5,
	
	Cval_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cval_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cval;

//Cval character definitions
static const CharFrame char_cval_frame[] = {
  {Cval_ArcMain_Idle0, {  0,  0,195,171}, {160,177}}, //0 idle 1
  {Cval_ArcMain_Idle1, {  0,  0,194,171}, {159,178}}, //1 idle 2
  {Cval_ArcMain_Idle2, {  0,  0,186,172}, {159,180}}, //2 idle 3
  {Cval_ArcMain_Idle3, {  0,  0,180,172}, {159,180}}, //3 idle 4
  {Cval_ArcMain_Idle4, {  0,  0,180,172}, {159,180}}, //4 idle 5
  {Cval_ArcMain_Idle5, {  0,  0,180,173}, {159,179}}, //5 idle 6

  {Cval_ArcMain_Left0, {  0,  0,177,179}, {168,180}}, //6 left 1
  {Cval_ArcMain_Left1, {  0,  0,177,181}, {167,180}}, //7 left 2
  {Cval_ArcMain_Left2, {  0,  0,177,180}, {167,180}}, //8 left 3
  {Cval_ArcMain_Left3, {  0,  0,178,180}, {167,180}}, //9 left 4

  {Cval_ArcMain_Down0, {  0,  0,186,160}, {159,160}}, //10 down 1
  {Cval_ArcMain_Down1, {  0,  0,185,162}, {158,164}}, //11 down 2
  {Cval_ArcMain_Down2, {  0,  0,185,162}, {158,165}}, //12 down 3

  {Cval_ArcMain_Up0, {  0,  0,190,180}, {162,191}}, //13 up 1
  {Cval_ArcMain_Up1, {  0,  0,190,175}, {161,186}}, //14 up 2
  {Cval_ArcMain_Up2, {  0,  0,190,175}, {161,185}}, //15 up 3

  {Cval_ArcMain_Right0, {  0,  0,198,178}, {153,180}}, //16 right 1
  {Cval_ArcMain_Right1, {  0,  0,198,182}, {156,182}}, //17 right 2
  {Cval_ArcMain_Right2, {  0,  0,197,183}, {156,182}}, //18 right 3

  {Cval_ArcMain_Hi0, {  0,  0,173,169}, {183,180}}, //19 hi 1
  {Cval_ArcMain_Hi1, {  0,  0,170,169}, {185,180}}, //20 hi 2
  {Cval_ArcMain_Hi2, {  0,  0,166,169}, {180,180}}, //21 hi 3
  {Cval_ArcMain_Hi3, {  0,  0,162,169}, {176,180}}, //22 hi 4
  {Cval_ArcMain_Hi4, {  0,  0,161,169}, {176,180}}, //23 hi 5
  {Cval_ArcMain_Hi5, {  0,  0,161,169}, {176,180}}, //24 hi 6
};

static const Animation char_cval_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 13, 14, 15, 15, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 16, 17, 18, 18, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 19, 19, 20, 21, 22, 22, 23, 23, 24, 24, 24, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Special1
};

//Cval character functions
void Char_Cval_SetFrame(void *user, u8 frame)
{
	Char_Cval *this = (Char_Cval*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cval_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cval_Tick(Character *character)
{
	Char_Cval *this = (Char_Cval*)character;
	
	//Camera stuff
	if ((stage.stage_id == StageId_Chippin) || (stage.stage_id == StageId_Chipping))
	{
		if (stage.song_step == 0)
		{
			this->character.focus_x = FIXED_DEC(-10,1);
			this->character.focus_y = FIXED_DEC(-107,1);
			this->character.focus_zoom = FIXED_DEC(1018,1024);
		}
		if (stage.song_step == 448)
		{
			this->character.focus_x = FIXED_DEC(36,1);
			this->character.focus_y = FIXED_DEC(-147,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if ((stage.song_step > 449) && (this->character.focus_zoom != FIXED_DEC(543,1024)) && (stage.paused == false))
		{
			zoomoutcvp += 1;
			if (zoomoutcvp == 2)
			{
				this->character.focus_zoom -= FIXED_DEC(1,1024);
				zoomoutcvp = 0;
			}
		}
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Chippin: //Hi pip
				if (stage.song_step == 284)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Cval_SetFrame);
	Character_Draw(character, &this->tex, &char_cval_frame[this->frame]);
}

void Char_Cval_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cval_Free(Character *character)
{
	Char_Cval *this = (Char_Cval*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cval_New(fixed_t x, fixed_t y)
{
	//Allocate cval object
	Char_Cval *this = Mem_Alloc(sizeof(Char_Cval));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cval_New] Failed to allocate cval object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cval_Tick;
	this->character.set_anim = Char_Cval_SetAnim;
	this->character.free = Char_Cval_Free;
	
	Animatable_Init(&this->character.animatable, char_cval_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFF0064CE;
	
	this->character.focus_x = FIXED_DEC(-23,1);
	this->character.focus_y = FIXED_DEC(-102,1);
	this->character.focus_zoom = FIXED_DEC(787,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\CVAL.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "hi0.tim",
  "hi1.tim",
  "hi2.tim",
  "hi3.tim",
  "hi4.tim",
  "hi5.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
