/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cvaltorture.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//CvalTorture character structure
enum
{
  CvalTorture_ArcMain_Idle0,
  CvalTorture_ArcMain_Left0,
  CvalTorture_ArcMain_Down0,
  CvalTorture_ArcMain_Up0,
  CvalTorture_ArcMain_Right0,
  CvalTorture_ArcMain_GetOut0,
  CvalTorture_ArcMain_GetOut1,
  CvalTorture_ArcMain_GetOut2,
  CvalTorture_ArcMain_GetOut3,
  CvalTorture_ArcMain_GetOut4,
  CvalTorture_ArcMain_GetOut5,
	
	CvalTorture_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[CvalTorture_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_CvalTorture;

//CvalTorture character definitions
static const CharFrame char_cvaltorture_frame[] = {
  {CvalTorture_ArcMain_Idle0, {  0,  0,113,121}, {160,170}}, //0 idle 1
  {CvalTorture_ArcMain_Idle0, {114,  0,113,121}, {160,170}}, //1 idle 2

  {CvalTorture_ArcMain_Left0, {  0,  0,115,122}, {169-18,171}}, //2 left 1
  {CvalTorture_ArcMain_Left0, {116,  0,115,121}, {170-18,170}}, //3 left 2
  {CvalTorture_ArcMain_Left0, {  0,123,114,121}, {170-18,170}}, //4 left 3
  {CvalTorture_ArcMain_Left0, {115,123,115,121}, {170-18,170}}, //5 left 4

  {CvalTorture_ArcMain_Down0, {  0,  0,113,118}, {161-4,167}}, //6 down 1
  {CvalTorture_ArcMain_Down0, {114,  0,114,119}, {162-4,168}}, //7 down 2
  {CvalTorture_ArcMain_Down0, {  0,120,114,119}, {162-4,168}}, //8 down 3

  {CvalTorture_ArcMain_Up0, {  0,  0,114,124}, {159+6,172+1}}, //9 up 1
  {CvalTorture_ArcMain_Up0, {115,  0,114,123}, {158+6,171+1}}, //10 up 2
  {CvalTorture_ArcMain_Up0, {  0,125,114,123}, {158+6,171+1}}, //11 up 3

  {CvalTorture_ArcMain_Right0, {  0,  0,114,122}, {153+17,171}}, //12 right 1
  {CvalTorture_ArcMain_Right0, {115,  0,114,121}, {151+17,170}}, //13 right 2
  {CvalTorture_ArcMain_Right0, {  0,123,113,121}, {151+17,170}}, //14 right 3

  {CvalTorture_ArcMain_GetOut0, {  0,  0,113,120}, {212-52,169}}, //15 getout 1
  {CvalTorture_ArcMain_GetOut0, {114,  0,114,120}, {214-52,169}}, //16 getout 2
  {CvalTorture_ArcMain_GetOut0, {  0,121,114,121}, {214-52,169}}, //17 getout 3
  {CvalTorture_ArcMain_GetOut0, {115,121,113,121}, {214-52,169}}, //18 getout 4
  {CvalTorture_ArcMain_GetOut1, {  0,  0,113,121}, {214-52,169}}, //19 getout 5
  {CvalTorture_ArcMain_GetOut1, {114,  0,115,121}, {214-52,169}}, //20 getout 6
  {CvalTorture_ArcMain_GetOut1, {  0,122,115,117}, {212-52,165}}, //21 getout 7
  {CvalTorture_ArcMain_GetOut1, {116,122,116,133}, {189-52,192}}, //22 getout 8
  {CvalTorture_ArcMain_GetOut2, {  0,  0,116,144}, {189-52,191}}, //23 getout 9
  {CvalTorture_ArcMain_GetOut2, {117,  0,117,140}, {192-52,189}}, //24 getout 10
  {CvalTorture_ArcMain_GetOut3, {  0,  0,126,133}, {197-52,182}}, //25 getout 11
  {CvalTorture_ArcMain_GetOut3, {127,  0,122,118}, {192-52,170}}, //26 getout 12
  {CvalTorture_ArcMain_GetOut4, {  0,  0,122,118}, {192-52,171}}, //27 getout 13
  {CvalTorture_ArcMain_GetOut4, {123,  0,123,154}, {166-52,199}}, //28 getout 14
  {CvalTorture_ArcMain_GetOut5, {  0,  0,123,154}, {167-52,198}}, //29 getout 15
  {CvalTorture_ArcMain_GetOut5, {124,  0,123,153}, {167-52,198}}, //30 getout 16
};

static const Animation char_cvaltorture_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 2, 2, 3, 3, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 6, 6, 7, 7, 8, 8, 7, 7, 8, 8, 7, 7, 8, 8, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 9, 9, 10, 10, 11, 11, 10, 10, 11, 11, 10, 10, 11, 11, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 12, 12, 13, 13, 14, 14, 13, 13, 14, 14, 13, 13, 14, 14, 13, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 15, 15, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 22, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 26, 26, 27, 27, ASCR_BACK, 1}},         //CharAnim_Special2
	{1, (const u8[]){ 28, 28, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, ASCR_BACK, 1}},         //CharAnim_Special3
};

//CvalTorture character functions
void Char_CvalTorture_SetFrame(void *user, u8 frame)
{
	Char_CvalTorture *this = (Char_CvalTorture*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cvaltorture_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_CvalTorture_Tick(Character *character)
{
	Char_CvalTorture *this = (Char_CvalTorture*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Torture)
	{
		if (stage.camswitch == 1) //cval
		{
			this->character.focus_x = FIXED_DEC(19,1);
			this->character.focus_y = FIXED_DEC(-237,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.camswitch == 2) //pip
		{
			this->character.focus_x = FIXED_DEC(-51,1);
			this->character.focus_y = FIXED_DEC(-237,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.camswitch == 3) //ROZEBUD
		{
			this->character.focus_x = FIXED_DEC(-8,1);
			this->character.focus_y = FIXED_DEC(-251,1);
			this->character.focus_zoom = FIXED_DEC(965,1024);
		}
		if (stage.camswitch == 4) //end
		{
			this->character.focus_x = FIXED_DEC(-8,1);
			this->character.focus_y = FIXED_DEC(-237,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if ((stage.song_step == 258) || (stage.song_step == 494) || (stage.song_step == 508) || (stage.song_step == 674) || (stage.song_step == 928) || (stage.song_step == 1088) || (stage.song_step == 1156) || (stage.song_step == 1200) || (stage.song_step == 1470)) //cval
			stage.camswitch = 1;
		if ((stage.song_step == 382) || (stage.song_step == 506) || (stage.song_step == 518) || (stage.song_step == 811) || (stage.song_step == 1120) || (stage.song_step == 1195) || (stage.song_step == 1342)) //pip
			stage.camswitch = 2;
		if (stage.song_beat == 255) //rozebud
			stage.camswitch = 3;
		if (stage.song_step == 1478) //end
			stage.camswitch = 4;
	}
	
	//Perform idle dance
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2) && (character->animatable.anim != CharAnim_Special3))
	{
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Torture: //GTFO
				if (stage.song_step == 1045) //what are you doing
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 1066) //get the hell out of here
					character->set_anim(character, CharAnim_Special2);
				if (stage.song_step == 1085) //AAAA
					character->set_anim(character, CharAnim_Special3);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_CvalTorture_SetFrame);
	Character_DrawFlipped(character, &this->tex, &char_cvaltorture_frame[this->frame]);
}

void Char_CvalTorture_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_CvalTorture_Free(Character *character)
{
	Char_CvalTorture *this = (Char_CvalTorture*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_CvalTorture_New(fixed_t x, fixed_t y)
{
	//Allocate cvaltorture object
	Char_CvalTorture *this = Mem_Alloc(sizeof(Char_CvalTorture));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_CvalTorture_New] Failed to allocate cvaltorture object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_CvalTorture_Tick;
	this->character.set_anim = Char_CvalTorture_SetAnim;
	this->character.free = Char_CvalTorture_Free;
	
	Animatable_Init(&this->character.animatable, char_cvaltorture_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFF0064CE;
	
	this->character.focus_x = FIXED_DEC(-8,1);
	this->character.focus_y = FIXED_DEC(-238,1);
	this->character.focus_zoom = FIXED_DEC(1357,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\CVALTORT.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "getout0.tim",
  "getout1.tim",
  "getout2.tim",
  "getout3.tim",
  "getout4.tim",
  "getout5.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
