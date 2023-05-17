/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henryphone.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Henry Phone player types
enum
{
  HenryPhone_ArcMain_Idle0,
  HenryPhone_ArcMain_Idle1,
  HenryPhone_ArcMain_Idle2,
  HenryPhone_ArcMain_Left0,
  HenryPhone_ArcMain_Left1,
  HenryPhone_ArcMain_Down0,
  HenryPhone_ArcMain_Down1,
  HenryPhone_ArcMain_Up0,
  HenryPhone_ArcMain_Right0,
  HenryPhone_ArcMain_Right1,
  HenryPhone_ArcMain_LeftM0,
  HenryPhone_ArcMain_LeftM1,
  HenryPhone_ArcMain_DownM0,
  HenryPhone_ArcMain_DownM1,
  HenryPhone_ArcMain_UpM0,
  HenryPhone_ArcMain_UpM1,
  HenryPhone_ArcMain_RightM0,
  HenryPhone_ArcMain_RightM1,
  HenryPhone_ArcMain_RightM2,
  HenryPhone_ArcMain_Phone0,
  HenryPhone_ArcMain_Phone1,
  HenryPhone_ArcMain_Phone2,
  HenryPhone_ArcMain_Phone3,
  HenryPhone_ArcMain_Phone4,
  HenryPhone_ArcMain_Phone5,
  HenryPhone_ArcMain_Phone6,
  HenryPhone_ArcMain_Phone7,
  HenryPhone_ArcMain_Phone8,
  HenryPhone_ArcMain_Phone9,
  HenryPhone_ArcMain_Phone10,
  HenryPhone_ArcMain_Phone11,
  HenryPhone_ArcMain_Phone12,
  HenryPhone_ArcMain_Phone13,
  HenryPhone_ArcMain_Phone14,
  HenryPhone_ArcMain_Phone15,
  HenryPhone_ArcMain_Phone16,
	
	HenryPhone_ArcMain_Max,
};

#define HenryPhone_Arc_Max HenryPhone_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[HenryPhone_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_HenryPhone;

//Henry Phone player definitions
static const CharFrame char_henryphone_frame[] = {
  {HenryPhone_ArcMain_Idle0, {  0,  0,104,178}, {162,179}}, //0 idle 1
  {HenryPhone_ArcMain_Idle0, {104,  0,105,178}, {162,179}}, //1 idle 2
  {HenryPhone_ArcMain_Idle1, {  0,  0,101,179}, {160,180}}, //2 idle 3
  {HenryPhone_ArcMain_Idle1, {101,  0,100,181}, {160,182}}, //3 idle 4
  {HenryPhone_ArcMain_Idle2, {  0,  0,100,181}, {160,182}}, //4 idle 5

  {HenryPhone_ArcMain_Left0, {  0,  0,130,176}, {204,175}}, //5 left 1
  {HenryPhone_ArcMain_Left1, {  0,  0,130,174}, {201,172}}, //6 left 2

  {HenryPhone_ArcMain_Down0, {  0,  0,129,144}, {202,144}}, //7 down 1
  {HenryPhone_ArcMain_Down1, {  0,  0,128,146}, {203,146}}, //8 down 2

  {HenryPhone_ArcMain_Up0, {  0,  0,106,180}, {171,181}}, //9 up 1
  {HenryPhone_ArcMain_Up0, {106,  0,109,177}, {173,178}}, //10 up 2

  {HenryPhone_ArcMain_Right0, {  0,  0,134,163}, {176,165}}, //11 right 1
  {HenryPhone_ArcMain_Right1, {  0,  0,137,160}, {180,162}}, //12 right 2

  {HenryPhone_ArcMain_LeftM0, {  0,  0,127,161}, {208,160}}, //13 leftm 1
  {HenryPhone_ArcMain_LeftM0, {127,  0,128,161}, {204,160}}, //14 leftm 2
  {HenryPhone_ArcMain_LeftM1, {  0,  0,128,161}, {205,160}}, //15 leftm 3

  {HenryPhone_ArcMain_DownM0, {  0,  0,128,142}, {198,140}}, //16 downm 1
  {HenryPhone_ArcMain_DownM0, {128,  0,118,150}, {193,148}}, //17 downm 2
  {HenryPhone_ArcMain_DownM1, {  0,  0,123,146}, {195,144}}, //18 downm 3

  {HenryPhone_ArcMain_UpM0, {  0,  0,102,179}, {170,180}}, //19 upm 1
  {HenryPhone_ArcMain_UpM0, {102,  0,108,176}, {173,176}}, //20 upm 2
  {HenryPhone_ArcMain_UpM1, {  0,  0,106,176}, {172,177}}, //21 upm 3

  {HenryPhone_ArcMain_RightM0, {  0,  0,136,150}, {174,152}}, //22 rightm 1
  {HenryPhone_ArcMain_RightM1, {  0,  0,134,150}, {177,152}}, //23 rightm 2
  {HenryPhone_ArcMain_RightM2, {  0,  0,134,150}, {176,152}}, //24 rightm 3

  {HenryPhone_ArcMain_Phone0, {  0,  0, 92,174}, {161,176}}, //25 phone 1
  {HenryPhone_ArcMain_Phone0, { 92,  0, 92,175}, {161,176}}, //26 phone 2
  {HenryPhone_ArcMain_Phone1, {  0,  0, 89,176}, {160,178}}, //27 phone 3
  {HenryPhone_ArcMain_Phone1, { 89,  0, 86,181}, {160,183}}, //28 phone 4
  {HenryPhone_ArcMain_Phone2, {  0,  0, 86,182}, {160,183}}, //29 phone 5
  {HenryPhone_ArcMain_Phone2, { 86,  0, 86,180}, {160,182}}, //30 phone 6
  {HenryPhone_ArcMain_Phone3, {  0,  0, 91,165}, {160,166}}, //31 phone 7
  {HenryPhone_ArcMain_Phone3, { 91,  0, 91,165}, {160,166}}, //32 phone 8
  {HenryPhone_ArcMain_Phone4, {  0,  0, 90,165}, {160,166}}, //33 phone 9
  {HenryPhone_ArcMain_Phone4, { 90,  0, 91,165}, {160,166}}, //34 phone 10
  {HenryPhone_ArcMain_Phone5, {  0,  0, 90,165}, {159,166}}, //35 phone 11
  {HenryPhone_ArcMain_Phone5, { 90,  0, 91,165}, {160,166}}, //36 phone 12
  {HenryPhone_ArcMain_Phone6, {  0,  0, 91,165}, {160,166}}, //37 phone 13
  {HenryPhone_ArcMain_Phone6, { 91,  0, 82,180}, {160,182}}, //38 phone 14
  {HenryPhone_ArcMain_Phone6, {173,  0, 82,180}, {160,181}}, //39 phone 15
  {HenryPhone_ArcMain_Phone7, {  0,  0, 83,180}, {160,181}}, //40 phone 16
  {HenryPhone_ArcMain_Phone7, { 83,  0, 94,180}, {160,181}}, //41 phone 17
  {HenryPhone_ArcMain_Phone8, {  0,  0, 96,180}, {160,181}}, //42 phone 18
  {HenryPhone_ArcMain_Phone8, { 96,  0, 96,180}, {160,181}}, //43 phone 19
  {HenryPhone_ArcMain_Phone9, {  0,  0, 96,180}, {160,181}}, //44 phone 20
  {HenryPhone_ArcMain_Phone9, { 96,  0, 96,180}, {160,181}}, //45 phone 21
  {HenryPhone_ArcMain_Phone10, {  0,  0,106,187}, {160,181}}, //46 phone 22
  {HenryPhone_ArcMain_Phone10, {106,  0,108,183}, {160,181}}, //47 phone 23
  {HenryPhone_ArcMain_Phone11, {  0,  0,109,187}, {159,181}}, //48 phone 24
  {HenryPhone_ArcMain_Phone11, {109,  0,109,186}, {159,181}}, //49 phone 25
  {HenryPhone_ArcMain_Phone12, {  0,  0,109,185}, {159,180}}, //50 phone 26
  {HenryPhone_ArcMain_Phone12, {109,  0,109,185}, {159,180}}, //51 phone 27
  {HenryPhone_ArcMain_Phone13, {  0,  0, 95,179}, {159,180}}, //52 phone 28
  {HenryPhone_ArcMain_Phone13, { 95,  0, 96,237}, {159,238}}, //53 phone 29
  {HenryPhone_ArcMain_Phone14, {  0,  0,110,215}, {160,210}}, //54 phone 30
  {HenryPhone_ArcMain_Phone14, {110,  0,100,179}, {160,180}}, //55 phone 31
  {HenryPhone_ArcMain_Phone15, {  0,  0,109,185}, {160,180}}, //56 phone 32
  {HenryPhone_ArcMain_Phone15, {109,  0,101,179}, {160,180}}, //57 phone 33
  {HenryPhone_ArcMain_Phone16, {  0,  0,100,180}, {160,181}}, //58 phone 34
  {HenryPhone_ArcMain_Phone16, {100,  0,100,180}, {160,181}}, //59 phone 35
};

static const Animation char_henryphone_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 37, 38, 38, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 
	48, 48, 49, 49, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 51, 51, 52, 52, 51, 51, 53, 54, 55, 56, 57, 57, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 15, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 16, 17, 18, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 19, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 22, 23, 24, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Henry Phone player functions
void Char_HenryPhone_SetFrame(void *user, u8 frame)
{
	Char_HenryPhone *this = (Char_HenryPhone*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_henryphone_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_HenryPhone_Tick(Character *character)
{
	Char_HenryPhone *this = (Char_HenryPhone*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_GreatestPlan)
	{
		if (stage.song_step == 40)
		{
			this->character.health_i = 1;
			this->character.health_bar = 0xFFBDD7D8;
		}
		if (stage.song_step == 52)
		{
			this->character.focus_x = FIXED_DEC(-8,1);
			this->character.focus_y = FIXED_DEC(4,1);	
			this->character.focus_zoom = FIXED_DEC(1731,1024);
		}
	}
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (stage.stage_id != StageId_Spookpostor)
		{
			if (Animatable_Ended(&character->animatable) &&
				(character->animatable.anim != CharAnim_Left &&
				character->animatable.anim != CharAnim_LeftAlt &&
				character->animatable.anim != PlayerAnim_LeftMiss &&
				character->animatable.anim != CharAnim_Down &&
				character->animatable.anim != CharAnim_DownAlt &&
				character->animatable.anim != PlayerAnim_DownMiss &&
				character->animatable.anim != CharAnim_Up &&
				character->animatable.anim != CharAnim_UpAlt &&
				character->animatable.anim != PlayerAnim_UpMiss &&
				character->animatable.anim != CharAnim_Right &&
				character->animatable.anim != CharAnim_RightAlt &&
				character->animatable.anim != PlayerAnim_RightMiss) &&
				(stage.song_step & 0x7) == 0)
				character->set_anim(character, CharAnim_Idle);
		}
		else
		{
			if (Animatable_Ended(&character->animatable) &&
				(character->animatable.anim != CharAnim_Left &&
				character->animatable.anim != CharAnim_LeftAlt &&
				character->animatable.anim != PlayerAnim_LeftMiss &&
				character->animatable.anim != CharAnim_Down &&
				character->animatable.anim != CharAnim_DownAlt &&
				character->animatable.anim != PlayerAnim_DownMiss &&
				character->animatable.anim != CharAnim_Up &&
				character->animatable.anim != CharAnim_UpAlt &&
				character->animatable.anim != PlayerAnim_UpMiss &&
				character->animatable.anim != CharAnim_Right &&
				character->animatable.anim != CharAnim_RightAlt &&
				character->animatable.anim != PlayerAnim_RightMiss) &&
				(stage.song_step & 0xF) == 0)
				character->set_anim(character, CharAnim_Idle);
		}
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_GreatestPlan:
				if (stage.song_step == -29) //phone
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
		
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_HenryPhone_SetFrame);
	Character_Draw(character, &this->tex, &char_henryphone_frame[this->frame]);
}

void Char_HenryPhone_SetAnim(Character *character, u8 anim)
{
	Char_HenryPhone *this = (Char_HenryPhone*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_HenryPhone_Free(Character *character)
{
	Char_HenryPhone *this = (Char_HenryPhone*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_HenryPhone_New(fixed_t x, fixed_t y)
{
	//Allocate henry phone object
	Char_HenryPhone *this = Mem_Alloc(sizeof(Char_HenryPhone));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_HenryPhone_New] Failed to allocate henry phone object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_HenryPhone_Tick;
	this->character.set_anim = Char_HenryPhone_SetAnim;
	this->character.free = Char_HenryPhone_Free;
	
	Animatable_Init(&this->character.animatable, char_henryphone_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 7;

	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-1,1);
	this->character.focus_y = FIXED_DEC(0,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\HENRYPHN.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "right0.tim",
  "right1.tim",
  "leftm0.tim",
  "leftm1.tim",
  "downm0.tim",
  "downm1.tim",
  "upm0.tim",
  "upm1.tim",
  "rightm0.tim",
  "rightm1.tim",
  "rightm2.tim",
  "phone0.tim",
  "phone1.tim",
  "phone2.tim",
  "phone3.tim",
  "phone4.tim",
  "phone5.tim",
  "phone6.tim",
  "phone7.tim",
  "phone8.tim",
  "phone9.tim",
  "phone10.tim",
  "phone11.tim",
  "phone12.tim",
  "phone13.tim",
  "phone14.tim",
  "phone15.tim",
  "phone16.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
