/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfdrip.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Drip player types
enum
{
  BFDrip_ArcMain_Idle0,
  BFDrip_ArcMain_Idle1,
  BFDrip_ArcMain_Left0,
  BFDrip_ArcMain_Left1,
  BFDrip_ArcMain_Down0,
  BFDrip_ArcMain_Down1,
  BFDrip_ArcMain_Up0,
  BFDrip_ArcMain_Up1,
  BFDrip_ArcMain_Right0,
  BFDrip_ArcMain_Right1,
  BFDrip_ArcMain_LeftM0,
  BFDrip_ArcMain_LeftM1,
  BFDrip_ArcMain_DownM0,
  BFDrip_ArcMain_DownM1,
  BFDrip_ArcMain_UpM0,
  BFDrip_ArcMain_UpM1,
  BFDrip_ArcMain_RightM0,
  BFDrip_ArcMain_RightM1,
	
	BFDrip_ArcMain_Max,
};

#define BFDrip_Arc_Max BFDrip_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFDrip_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFDrip;

//Boyfriend Drip player definitions
static const CharFrame char_bfdrip_frame[] = {
  {BFDrip_ArcMain_Idle0, {  0,  0, 98,113}, {159,153}}, //0 idle 1
  {BFDrip_ArcMain_Idle0, { 98,  0, 97,114}, {158,154}}, //1 idle 2
  {BFDrip_ArcMain_Idle0, {  0,114, 97,118}, {158,157}}, //2 idle 3
  {BFDrip_ArcMain_Idle0, { 97,114, 98,120}, {158,159}}, //3 idle 4
  {BFDrip_ArcMain_Idle1, {  0,  0, 98,120}, {158,159}}, //4 idle 5
  {BFDrip_ArcMain_Idle1, { 98,  0, 98,120}, {158,159}}, //5 idle 6
  {BFDrip_ArcMain_Idle1, {  0,120, 98,118}, {158,158}}, //6 idle 7

  {BFDrip_ArcMain_Left0, {  0,  0, 90,120}, {160,160}}, //7 left 1
  {BFDrip_ArcMain_Left0, { 90,  0, 91,119}, {159,159}}, //8 left 2
  {BFDrip_ArcMain_Left0, {  0,120, 92,120}, {159,159}}, //9 left 3
  {BFDrip_ArcMain_Left0, { 92,120, 92,120}, {159,159}}, //10 left 4
  {BFDrip_ArcMain_Left1, {  0,  0, 95,120}, {159,159}}, //11 left 5

  {BFDrip_ArcMain_Down0, {  0,  0,100,106}, {161,146}}, //12 down 1
  {BFDrip_ArcMain_Down0, {100,  0,100,109}, {160,149}}, //13 down 2
  {BFDrip_ArcMain_Down0, {  0,109,100,111}, {160,150}}, //14 down 3
  {BFDrip_ArcMain_Down0, {100,109, 99,111}, {159,150}}, //15 down 4
  {BFDrip_ArcMain_Down1, {  0,  0, 99,113}, {159,152}}, //16 down 5

  {BFDrip_ArcMain_Up0, {  0,  0, 98,124}, {155,164}}, //17 up 1
  {BFDrip_ArcMain_Up0, { 98,  0, 98,123}, {155,163}}, //18 up 2
  {BFDrip_ArcMain_Up0, {  0,124, 97,122}, {155,162}}, //19 up 3
  {BFDrip_ArcMain_Up0, { 97,124, 97,122}, {155,162}}, //20 up 4
  {BFDrip_ArcMain_Up1, {  0,  0, 98,122}, {155,162}}, //21 up 5
  {BFDrip_ArcMain_Up1, { 98,  0, 96,121}, {155,160}}, //22 up 6

  {BFDrip_ArcMain_Right0, {  0,  0,103,115}, {154,155}}, //23 right 1
  {BFDrip_ArcMain_Right0, {103,  0,102,117}, {154,157}}, //24 right 2
  {BFDrip_ArcMain_Right0, {  0,117,102,118}, {154,158}}, //25 right 3
  {BFDrip_ArcMain_Right0, {102,117,103,118}, {154,158}}, //26 right 4
  {BFDrip_ArcMain_Right1, {  0,  0,102,118}, {155,157}}, //27 right 5

  {BFDrip_ArcMain_LeftM0, {  0,  0, 93,119}, {160,159}}, //28 leftm 1
  {BFDrip_ArcMain_LeftM0, { 93,  0, 96,119}, {161,159}}, //29 leftm 2
  {BFDrip_ArcMain_LeftM0, {  0,119, 99,119}, {160,159}}, //30 leftm 3
  {BFDrip_ArcMain_LeftM0, { 99,119, 97,119}, {158,159}}, //31 leftm 4
  {BFDrip_ArcMain_LeftM1, {  0,  0, 95,119}, {160,159}}, //32 leftm 5
  {BFDrip_ArcMain_LeftM1, { 95,  0, 95,119}, {160,159}}, //33 leftm 6

  {BFDrip_ArcMain_DownM0, {  0,  0, 94,113}, {159,151}}, //34 downm 1
  {BFDrip_ArcMain_DownM0, { 94,  0, 96,118}, {158,157}}, //35 downm 2
  {BFDrip_ArcMain_DownM0, {  0,118, 96,118}, {159,156}}, //36 downm 3
  {BFDrip_ArcMain_DownM0, { 96,118, 96,115}, {159,154}}, //37 downm 4
  {BFDrip_ArcMain_DownM1, {  0,  0, 96,115}, {159,153}}, //38 downm 5
  {BFDrip_ArcMain_DownM1, { 96,  0, 95,114}, {158,153}}, //39 downm 6
  {BFDrip_ArcMain_DownM1, {  0,115, 96,114}, {158,153}}, //40 downm 7

  {BFDrip_ArcMain_UpM0, {  0,  0,104,125}, {160,159}}, //41 upm 1
  {BFDrip_ArcMain_UpM0, {104,  0,105,125}, {159,158}}, //42 upm 2
  {BFDrip_ArcMain_UpM0, {  0,125,105,116}, {159,150}}, //43 upm 3
  {BFDrip_ArcMain_UpM0, {105,125,105,116}, {159,150}}, //44 upm 4
  {BFDrip_ArcMain_UpM1, {  0,  0,104,118}, {158,151}}, //45 upm 5
  {BFDrip_ArcMain_UpM1, {104,  0,104,118}, {159,151}}, //46 upm 6
  {BFDrip_ArcMain_UpM1, {  0,118,104,118}, {159,152}}, //47 upm 7

  {BFDrip_ArcMain_RightM0, {  0,  0,106,118}, {157,158}}, //48 rightm 1
  {BFDrip_ArcMain_RightM0, {106,  0,106,118}, {158,158}}, //49 rightm 2
  {BFDrip_ArcMain_RightM0, {  0,118,103,118}, {159,158}}, //50 rightm 3
  {BFDrip_ArcMain_RightM0, {103,118,102,118}, {159,158}}, //51 rightm 4
  {BFDrip_ArcMain_RightM1, {  0,  0,103,118}, {159,158}}, //52 rightm 5
};

static const Animation char_bfdrip_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 6, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 9, 10, 10, 10, 11, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 12, 13, 14, 15, 15, 15, 16, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 17, 18, 19, 20, 20, 21, 22, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 23, 24, 25, 26, 26, 26, 27, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 28, 29, 30, 31, 32, 33, 33, 33, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 34, 34, 35, 36, 37, 38, 39, 40, 40, 40, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 41, 42, 43, 44, 45, 46, 47, 47, 47, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 48, 49, 50, 51, 52, 52, 52, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Drip player functions
void Char_BFDrip_SetFrame(void *user, u8 frame)
{
	Char_BFDrip *this = (Char_BFDrip*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfdrip_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFDrip_Tick(Character *character)
{
	Char_BFDrip *this = (Char_BFDrip*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Drippypop)
	{
		if (stage.song_step == -29)
		{
			this->character.focus_x = FIXED_DEC(-172,1);
			this->character.focus_y = FIXED_DEC(-141,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.song_beat == 286)
		{
			this->character.focus_x = FIXED_DEC(-185,1);
			this->character.focus_y = FIXED_DEC(-203,1);
			this->character.focus_zoom = FIXED_DEC(1516,1024);
		}
		if (stage.song_beat == 304)
		{
			this->character.focus_x = FIXED_DEC(-172,1);
			this->character.focus_y = FIXED_DEC(-141,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.song_beat == 318)
		{
			this->character.focus_x = FIXED_DEC(-185,1);
			this->character.focus_y = FIXED_DEC(-203,1);
			this->character.focus_zoom = FIXED_DEC(1516,1024);
		}
		if (stage.song_beat == 336)
		{
			this->character.focus_x = FIXED_DEC(-172,1);
			this->character.focus_y = FIXED_DEC(-141,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		}
		if (stage.song_beat == 384)
		{
			this->character.focus_x = FIXED_DEC(-185,1);
			this->character.focus_y = FIXED_DEC(-203,1);
			this->character.focus_zoom = FIXED_DEC(1516,1024);
		}
		if (stage.song_beat == 401)
		{
			this->character.focus_x = FIXED_DEC(-172,1);
			this->character.focus_y = FIXED_DEC(-141,1);
			this->character.focus_zoom = FIXED_DEC(1221,1024);
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
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFDrip_SetFrame);
	Character_Draw(character, &this->tex, &char_bfdrip_frame[this->frame]);
}

void Char_BFDrip_SetAnim(Character *character, u8 anim)
{
	Char_BFDrip *this = (Char_BFDrip*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFDrip_Free(Character *character)
{
	Char_BFDrip *this = (Char_BFDrip*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFDrip_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFDrip *this = Mem_Alloc(sizeof(Char_BFDrip));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFDrip_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFDrip_Tick;
	this->character.set_anim = Char_BFDrip_SetAnim;
	this->character.free = Char_BFDrip_Free;
	
	Animatable_Init(&this->character.animatable, char_bfdrip_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	
	this->character.focus_x = FIXED_DEC(-172,1);
	this->character.focus_y = FIXED_DEC(-141,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFDRIP.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
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
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
