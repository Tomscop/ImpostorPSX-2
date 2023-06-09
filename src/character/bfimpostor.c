/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfimpostor.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Impostor player types
enum
{
  BFImpostor_ArcMain_Idle0,
  BFImpostor_ArcMain_Idle1,
  BFImpostor_ArcMain_Left0,
  BFImpostor_ArcMain_Down0,
  BFImpostor_ArcMain_Up0,
  BFImpostor_ArcMain_Right0,
  BFImpostor_ArcMain_LeftM0,
  BFImpostor_ArcMain_DownM0,
  BFImpostor_ArcMain_UpM0,
  BFImpostor_ArcMain_RightM0,
  BFImpostor_ArcMain_Shoot0,
  BFImpostor_ArcMain_Shoot1,
  BFImpostor_ArcMain_Shoot2,
  BFImpostor_ArcMain_Shoot3,
  BFImpostor_ArcMain_Shoot4,
	
	BFImpostor_ArcMain_Max,
};

#define BFImpostor_Arc_Max BFImpostor_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFImpostor_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFImpostor;

//Boyfriend Impostor player definitions
static const CharFrame char_bfimpostor_frame[] = {
  {BFImpostor_ArcMain_Idle0, {  0,  0,107, 99}, {159,156}}, //0 idle 1
  {BFImpostor_ArcMain_Idle0, {107,  0,107, 99}, {159,156}}, //1 idle 2
  {BFImpostor_ArcMain_Idle0, {  0, 99,106,100}, {158,157}}, //2 idle 3
  {BFImpostor_ArcMain_Idle0, {106, 99,105,104}, {156,160}}, //3 idle 4
  {BFImpostor_ArcMain_Idle1, {  0,  0,104,104}, {156,160}}, //4 idle 5

  {BFImpostor_ArcMain_Left0, {  0,  0, 85,106}, {146,159}}, //5 left 1
  {BFImpostor_ArcMain_Left0, { 85,  0, 85,106}, {145,158}}, //6 left 2

  {BFImpostor_ArcMain_Down0, {  0,  0, 94, 90}, {144,147}}, //7 down 1
  {BFImpostor_ArcMain_Down0, { 94,  0, 91, 91}, {144,148}}, //8 down 2

  {BFImpostor_ArcMain_Up0, {  0,  0,118,112}, {167,167}}, //9 up 1
  {BFImpostor_ArcMain_Up0, {118,  0,120,111}, {168,166}}, //10 up 2

  {BFImpostor_ArcMain_Right0, {  0,  0,102,102}, {142,158}}, //11 right 1
  {BFImpostor_ArcMain_Right0, {102,  0,102,102}, {143,159}}, //12 right 2

  {BFImpostor_ArcMain_LeftM0, {  0,  0, 86,105}, {144,157}}, //13 leftm 1
  {BFImpostor_ArcMain_LeftM0, { 86,  0, 86,107}, {144,159}}, //14 leftm 2
  {BFImpostor_ArcMain_LeftM0, {  0,107, 85,106}, {144,158}}, //15 leftm 3
  {BFImpostor_ArcMain_LeftM0, { 85,107, 85,105}, {144,157}}, //16 leftm 4

  {BFImpostor_ArcMain_DownM0, {  0,  0, 91, 97}, {144,154}}, //17 downm 1
  {BFImpostor_ArcMain_DownM0, { 91,  0, 90, 98}, {143,154}}, //18 downm 2

  {BFImpostor_ArcMain_UpM0, {  0,  0,122,112}, {171,167}}, //19 upm 1
  {BFImpostor_ArcMain_UpM0, {122,  0,123,111}, {172,166}}, //20 upm 2

  {BFImpostor_ArcMain_RightM0, {  0,  0,102,105}, {142,160}}, //21 rightm 1
  {BFImpostor_ArcMain_RightM0, {102,  0,102,105}, {141,160}}, //22 rightm 2
  {BFImpostor_ArcMain_RightM0, {  0,105,103,105}, {142,161}}, //23 rightm 3

  {BFImpostor_ArcMain_Shoot0, {  0,  0,126,100}, {186,156}}, //24 shoot 1
  {BFImpostor_ArcMain_Shoot0, {  0,100,130,101}, {190,156}}, //25 shoot 2
  {BFImpostor_ArcMain_Shoot1, {  0,  0,130,101}, {191,156}}, //26 shoot 3
  {BFImpostor_ArcMain_Shoot1, {  0,101,179,133}, {236,188}}, //27 shoot 4
  {BFImpostor_ArcMain_Shoot2, {  0,  0,215,133}, {272,188}}, //28 shoot 5
  {BFImpostor_ArcMain_Shoot3, {  0,  0, 88,132}, {145,187}}, //29 shoot 6
  {BFImpostor_ArcMain_Shoot3, { 88,  0, 88,132}, {145,187}}, //30 shoot 7
  {BFImpostor_ArcMain_Shoot4, {  0,  0,125, 99}, {179,154}}, //31 shoot 8
  {BFImpostor_ArcMain_Shoot4, {125,  0,124, 99}, {179,155}}, //32 shoot 9
};

static const Animation char_bfimpostor_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 24, 24, 25, 26, 27, 28, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, ASCR_BACK, 1}},       //CharAnim_Special1
	{2, (const u8[]){ 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 13, 14, 15, 16, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 17, 18, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 19, 20, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 21, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Impostor player functions
void Char_BFImpostor_SetFrame(void *user, u8 frame)
{
	Char_BFImpostor *this = (Char_BFImpostor*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfimpostor_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFImpostor_Tick(Character *character)
{
	Char_BFImpostor *this = (Char_BFImpostor*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_TomongusTuesday)
	{
		if (stage.song_step == 196)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 983)
		{
			this->character.focus_x = FIXED_DEC(-214,1);
			this->character.focus_y = FIXED_DEC(-150,1);
			this->character.focus_zoom = FIXED_DEC(1162,1024);
		}
	}
	
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2) && (character->animatable.anim != CharAnim_Special3))
	{
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
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_TomongusTuesday: //KILLS YOU
				if (stage.song_step == 991)
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 1009)
					character->set_anim(character, CharAnim_Special2);
				break;
			default:
				break;
		}
		
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFImpostor_SetFrame);
	Character_Draw(character, &this->tex, &char_bfimpostor_frame[this->frame]);
}

void Char_BFImpostor_SetAnim(Character *character, u8 anim)
{
	Char_BFImpostor *this = (Char_BFImpostor*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFImpostor_Free(Character *character)
{
	Char_BFImpostor *this = (Char_BFImpostor*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFImpostor_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend impostor object
	Char_BFImpostor *this = Mem_Alloc(sizeof(Char_BFImpostor));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFImpostor_New] Failed to allocate boyfriend impostor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFImpostor_Tick;
	this->character.set_anim = Char_BFImpostor_SetAnim;
	this->character.free = Char_BFImpostor_Free;
	
	Animatable_Init(&this->character.animatable, char_bfimpostor_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;

	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-162,1);
	this->character.focus_y = FIXED_DEC(-147,1);
	this->character.focus_zoom = FIXED_DEC(1357,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFIMP.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftm0.tim",
  "downm0.tim",
  "upm0.tim",
  "rightm0.tim",
  "shoot0.tim",
  "shoot1.tim",
  "shoot2.tim",
  "shoot3.tim",
  "shoot4.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
