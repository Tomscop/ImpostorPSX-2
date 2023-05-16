/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pip.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Pip player types
enum
{
  Pip_ArcMain_Idle0,
  Pip_ArcMain_Idle1,
  Pip_ArcMain_Idle2,
  Pip_ArcMain_Idle3,
  Pip_ArcMain_Idle4,
  Pip_ArcMain_Idle5,
  Pip_ArcMain_Left0,
  Pip_ArcMain_Left1,
  Pip_ArcMain_Left2,
  Pip_ArcMain_Down0,
  Pip_ArcMain_Down1,
  Pip_ArcMain_Down2,
  Pip_ArcMain_Up0,
  Pip_ArcMain_Up1,
  Pip_ArcMain_Up2,
  Pip_ArcMain_Right0,
  Pip_ArcMain_Right1,
  Pip_ArcMain_Right2,
  Pip_ArcMain_IdleI0,
  Pip_ArcMain_IdleI1,
  Pip_ArcMain_IdleI2,
  Pip_ArcMain_IdleI3,
  Pip_ArcMain_IdleI4,
  Pip_ArcMain_IdleI5,
  Pip_ArcMain_LeftI0,
  Pip_ArcMain_LeftI1,
  Pip_ArcMain_LeftI2,
  Pip_ArcMain_DownI0,
  Pip_ArcMain_DownI1,
  Pip_ArcMain_DownI2,
  Pip_ArcMain_UpI0,
  Pip_ArcMain_UpI1,
  Pip_ArcMain_UpI2,
  Pip_ArcMain_RightI0,
  Pip_ArcMain_RightI1,
  Pip_ArcMain_RightI2,
	
	Pip_ArcMain_Max,
};

#define Pip_Arc_Max Pip_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Pip_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_Pip;

//Pip player definitions
static const CharFrame char_pip_frame[] = {
  {Pip_ArcMain_Idle0, {  0,  0,188,166}, {160,158}}, //0 idle 1
  {Pip_ArcMain_Idle1, {  0,  0,188,167}, {159,159}}, //1 idle 2
  {Pip_ArcMain_Idle2, {  0,  0,183,165}, {157,159}}, //2 idle 3
  {Pip_ArcMain_Idle3, {  0,  0,181,163}, {157,159}}, //3 idle 4
  {Pip_ArcMain_Idle4, {  0,  0,181,163}, {157,160}}, //4 idle 5
  {Pip_ArcMain_Idle5, {  0,  0,181,163}, {157,159}}, //5 idle 6

  {Pip_ArcMain_Left0, {  0,  0,171,161}, {158-18,160+1}}, //6 left 1
  {Pip_ArcMain_Left1, {  0,  0,172,161}, {160-18,160+1}}, //7 left 2
  {Pip_ArcMain_Left2, {  0,  0,172,161}, {160-18,160+1}}, //8 left 3

  {Pip_ArcMain_Down0, {  0,  0,173,167}, {151-2,154}}, //9 down 1
  {Pip_ArcMain_Down1, {  0,  0,173,167}, {151-2,156}}, //10 down 2
  {Pip_ArcMain_Down2, {  0,  0,173,168}, {151-2,157}}, //11 down 3

  {Pip_ArcMain_Up0, {  0,  0,194,159}, {153+18,165}}, //12 up 1
  {Pip_ArcMain_Up1, {  0,  0,195,157}, {154+18,163}}, //13 up 2
  {Pip_ArcMain_Up2, {  0,  0,196,157}, {154+18,163}}, //14 up 3

  {Pip_ArcMain_Right0, {  0,  0,182,164}, {148+19,162}}, //15 right 1
  {Pip_ArcMain_Right1, {  0,  0,181,164}, {145+19,162}}, //16 right 2
  {Pip_ArcMain_Right2, {  0,  0,181,164}, {145+19,162}}, //17 right 3
  
  {Pip_ArcMain_IdleI0, {  0,  0,188,166}, {160,158}}, //18 idlei 1
  {Pip_ArcMain_IdleI1, {  0,  0,188,167}, {159,159}}, //19 idlei 2
  {Pip_ArcMain_IdleI2, {  0,  0,183,165}, {157,159}}, //20 idlei 3
  {Pip_ArcMain_IdleI3, {  0,  0,181,163}, {157,159}}, //21 idlei 4
  {Pip_ArcMain_IdleI4, {  0,  0,181,163}, {157,160}}, //22 idlei 5
  {Pip_ArcMain_IdleI5, {  0,  0,181,163}, {157,159}}, //23 idlei 6

  {Pip_ArcMain_LeftI0, {  0,  0,171,161}, {158-18,160+1}}, //24 lefti 1
  {Pip_ArcMain_LeftI1, {  0,  0,172,161}, {159-18,160+1}}, //25 lefti 2
  {Pip_ArcMain_LeftI2, {  0,  0,173,161}, {160-18,160+1}}, //26 lefti 3

  {Pip_ArcMain_DownI0, {  0,  0,173,167}, {151-2,154}}, //27 downi 1
  {Pip_ArcMain_DownI1, {  0,  0,173,167}, {151-2,156}}, //28 downi 2
  {Pip_ArcMain_DownI2, {  0,  0,173,168}, {151-2,157}}, //29 downi 3

  {Pip_ArcMain_UpI0, {  0,  0,194,159}, {153+18,165}}, //30 upi 1
  {Pip_ArcMain_UpI1, {  0,  0,195,157}, {154+18,163}}, //31 upi 2
  {Pip_ArcMain_UpI2, {  0,  0,196,157}, {154+18,163}}, //32 upi 3

  {Pip_ArcMain_RightI0, {  0,  0,191,164}, {148+19,162}}, //33 righti 1
  {Pip_ArcMain_RightI1, {  0,  0,184,164}, {145+19,162}}, //34 righti 2
  {Pip_ArcMain_RightI2, {  0,  0,182,164}, {145+19,162}}, //35 righti 3
};

static const Animation char_pip_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 8, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 11, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, 14, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 15, 16, 17, 17, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

static const Animation char_pip_anim2[PlayerAnim_Max] = {
	{1, (const u8[]){ 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 22, 22, 22, 23, 23, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 24, 25, 26, 26, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 27, 28, 29, 29, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 30, 31, 32, 32, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 33, 34, 35, 35, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

//Pip player functions
void Char_Pip_SetFrame(void *user, u8 frame)
{
	Char_Pip *this = (Char_Pip*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_pip_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Pip_Tick(Character *character)
{
	Char_Pip *this = (Char_Pip*)character;
	
	//Camera stuff
	if ((stage.stage_id == StageId_Chippin) || (stage.stage_id == StageId_Chipping))
	{
		if (stage.song_step == 448)
		{
			this->character.focus_x = FIXED_DEC(-48,1);
			this->character.focus_y = FIXED_DEC(-140,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if ((stage.song_step > 448) && (this->character.focus_zoom != FIXED_DEC(543,1024)))
			this->character.focus_zoom = stage.opponent->focus_zoom;
	}
	
	//switch sprites
	if (stage.song_step == -29)
		Animatable_Init(&this->character.animatable, char_pip_anim);
	if (stage.song_step == 316)
		Animatable_Init(&this->character.animatable, char_pip_anim2);
	
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
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_Pip_SetFrame);
	Character_DrawFlipped(character, &this->tex, &char_pip_frame[this->frame]);
}

void Char_Pip_SetAnim(Character *character, u8 anim)
{
	Char_Pip *this = (Char_Pip*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Pip_Free(Character *character)
{
	Char_Pip *this = (Char_Pip*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Pip_New(fixed_t x, fixed_t y)
{
	//Allocate pip object
	Char_Pip *this = Mem_Alloc(sizeof(Char_Pip));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Pip_New] Failed to allocate pip object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Pip_Tick;
	this->character.set_anim = Char_Pip_SetAnim;
	this->character.free = Char_Pip_Free;
	
	Animatable_Init(&this->character.animatable, char_pip_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	//health bar color
	this->character.health_bar = 0xFFFDAB46;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-100,1);
	this->character.focus_zoom = FIXED_DEC(1018,1024);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\PIP.ARC;1");
		
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
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "idlei0.tim",
  "idlei1.tim",
  "idlei2.tim",
  "idlei3.tim",
  "idlei4.tim",
  "idlei5.tim",
  "lefti0.tim",
  "lefti1.tim",
  "lefti2.tim",
  "downi0.tim",
  "downi1.tim",
  "downi2.tim",
  "upi0.tim",
  "upi1.tim",
  "upi2.tim",
  "righti0.tim",
  "righti1.tim",
  "righti2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
