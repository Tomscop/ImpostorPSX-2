/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "going2killevery1.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Going2KillEvery1 player types
enum
{
  Going2KillEvery1_ArcMain_Idle0,
  Going2KillEvery1_ArcMain_Idle1,
  Going2KillEvery1_ArcMain_Left0,
  Going2KillEvery1_ArcMain_Left1,
  Going2KillEvery1_ArcMain_Down0,
  Going2KillEvery1_ArcMain_Up0,
  Going2KillEvery1_ArcMain_Up1,
  Going2KillEvery1_ArcMain_Up2,
  Going2KillEvery1_ArcMain_Right0,
  Going2KillEvery1_ArcMain_LeftM0,
  Going2KillEvery1_ArcMain_LeftM1,
  Going2KillEvery1_ArcMain_DownM0,
  Going2KillEvery1_ArcMain_UpM0,
  Going2KillEvery1_ArcMain_UpM1,
  Going2KillEvery1_ArcMain_RightM0,
  Going2KillEvery1_ArcMain_IdleMad0,
  Going2KillEvery1_ArcMain_IdleMad1,
  Going2KillEvery1_ArcMain_LeftMad0,
  Going2KillEvery1_ArcMain_LeftMad1,
  Going2KillEvery1_ArcMain_DownMad0,
  Going2KillEvery1_ArcMain_DownMad1,
  Going2KillEvery1_ArcMain_DownMad2,
  Going2KillEvery1_ArcMain_UpMad0,
  Going2KillEvery1_ArcMain_RightMad0,
  Going2KillEvery1_ArcMain_RightMad1,
  Going2KillEvery1_ArcMain_LeftMadM0,
  Going2KillEvery1_ArcMain_DownMadM0,
  Going2KillEvery1_ArcMain_DownMadM1,
  Going2KillEvery1_ArcMain_UpMadM0,
  Going2KillEvery1_ArcMain_RightMadM0,
	
	Going2KillEvery1_ArcMain_Max,
};

#define Going2KillEvery1_Arc_Max Going2KillEvery1_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Going2KillEvery1_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_Going2KillEvery1;

//Going2KillEvery1 player definitions
static const CharFrame char_going2killevery1_frame[] = {
  {Going2KillEvery1_ArcMain_Idle0, {  0,  0,107,107}, {177, 83}}, //0 idle 1
  {Going2KillEvery1_ArcMain_Idle0, {107,  0,102,109}, {172, 85}}, //1 idle 2
  {Going2KillEvery1_ArcMain_Idle0, {  0,109, 99,113}, {169, 89}}, //2 idle 3
  {Going2KillEvery1_ArcMain_Idle0, { 99,109,100,114}, {167, 90}}, //3 idle 4
  {Going2KillEvery1_ArcMain_Idle1, {  0,  0,100,114}, {167, 90}}, //4 idle 5
  {Going2KillEvery1_ArcMain_Idle1, {100,  0,100,114}, {167, 90}}, //5 idle 6

  {Going2KillEvery1_ArcMain_Left0, {  0,  0,182,114}, {256, 90}}, //6 left 1
  {Going2KillEvery1_ArcMain_Left0, {  0,114,169,114}, {246, 90}}, //7 left 2
  {Going2KillEvery1_ArcMain_Left1, {  0,  0,166,114}, {243, 90}}, //8 left 3

  {Going2KillEvery1_ArcMain_Down0, {  0,  0,113,111}, {185, 88}}, //9 down 1
  {Going2KillEvery1_ArcMain_Down0, {113,  0,102,111}, {177, 89}}, //10 down 2
  {Going2KillEvery1_ArcMain_Down0, {  0,111,102,111}, {175, 89}}, //11 down 3

  {Going2KillEvery1_ArcMain_Up0, {  0,  0,139,160}, {211,136}}, //12 up 1
  {Going2KillEvery1_ArcMain_Up1, {  0,  0,142,137}, {213,113}}, //13 up 2
  {Going2KillEvery1_ArcMain_Up2, {  0,  0,142,134}, {213,111}}, //14 up 3

  {Going2KillEvery1_ArcMain_Right0, {  0,  0, 92,112}, {158, 89}}, //15 right 1
  {Going2KillEvery1_ArcMain_Right0, { 92,  0,104,112}, {168, 89}}, //16 right 2
  {Going2KillEvery1_ArcMain_Right0, {  0,112,106,112}, {169, 89}}, //17 right 3

  {Going2KillEvery1_ArcMain_LeftM0, {  0,  0,182,138}, {257,113}}, //18 leftm 1
  {Going2KillEvery1_ArcMain_LeftM1, {  0,  0,184,137}, {251,112}}, //19 leftm 2

  {Going2KillEvery1_ArcMain_DownM0, {  0,  0,111,132}, {187,109}}, //20 downm 1
  {Going2KillEvery1_ArcMain_DownM0, {111,  0,113,132}, {186,109}}, //21 downm 2

  {Going2KillEvery1_ArcMain_UpM0, {  0,  0,140,158}, {214,134}}, //22 upm 1
  {Going2KillEvery1_ArcMain_UpM1, {  0,  0,138,158}, {207,135}}, //23 upm 2

  {Going2KillEvery1_ArcMain_RightM0, {  0,  0, 95,124}, {157,102}}, //24 rightm 1
  {Going2KillEvery1_ArcMain_RightM0, { 95,  0, 92,124}, {160,102}}, //25 rightm 2

  {Going2KillEvery1_ArcMain_IdleMad0, {  0,  0,114,112}, {174+3, 84+4}}, //26 idlemad 1
  {Going2KillEvery1_ArcMain_IdleMad0, {114,  0,122,110}, {175+3, 82+4}}, //27 idlemad 2
  {Going2KillEvery1_ArcMain_IdleMad0, {  0,112,120,110}, {171+3, 82+4}}, //28 idlemad 3
  {Going2KillEvery1_ArcMain_IdleMad0, {120,112,116,114}, {170+3, 86+4}}, //29 idlemad 4
  {Going2KillEvery1_ArcMain_IdleMad1, {  0,  0,125,118}, {175+3, 90+4}}, //30 idlemad 5
  {Going2KillEvery1_ArcMain_IdleMad1, {125,  0,115,118}, {176+3, 90+4}}, //31 idlemad 6
  {Going2KillEvery1_ArcMain_IdleMad1, {  0,118,116,118}, {175+3, 90+4}}, //32 idlemad 7
  {Going2KillEvery1_ArcMain_IdleMad1, {116,118,116,118}, {175+3, 90+4}}, //33 idlemad 8

  {Going2KillEvery1_ArcMain_LeftMad0, {  0,  0,187,112}, {244+3, 84+4}}, //34 leftmad 1
  {Going2KillEvery1_ArcMain_LeftMad0, {  0,112,175,114}, {237+3, 86+4}}, //35 leftmad 2
  {Going2KillEvery1_ArcMain_LeftMad1, {  0,  0,173,113}, {236+3, 86+4}}, //36 leftmad 3

  {Going2KillEvery1_ArcMain_DownMad0, {  0,  0,146,130}, {219+3, 90+4}}, //37 downmad 1
  {Going2KillEvery1_ArcMain_DownMad1, {  0,  0,145,130}, {217+3, 90+4}}, //38 downmad 2
  {Going2KillEvery1_ArcMain_DownMad2, {  0,  0,146,130}, {216+3, 90+4}}, //39 downmad 3

  {Going2KillEvery1_ArcMain_UpMad0, {  0,  0,106,111}, {156+3, 84+4}}, //40 upmad 1
  {Going2KillEvery1_ArcMain_UpMad0, {106,  0,104,110}, {156+3, 83+4}}, //41 upmad 2
  {Going2KillEvery1_ArcMain_UpMad0, {  0,111,105,110}, {159+3, 83+4}}, //42 upmad 3

  {Going2KillEvery1_ArcMain_RightMad0, {  0,  0,151,117}, {180+3, 91+4}}, //43 rightmad 1
  {Going2KillEvery1_ArcMain_RightMad0, {  0,117,153,117}, {181+3, 91+4}}, //44 rightmad 2
  {Going2KillEvery1_ArcMain_RightMad1, {  0,  0,147,117}, {184+3, 91+4}}, //45 rightmad 3

  {Going2KillEvery1_ArcMain_LeftMadM0, {  0,  0,182,117}, {234+3, 85+4}}, //46 leftmadm 1
  {Going2KillEvery1_ArcMain_LeftMadM0, {  0,117,180,117}, {231+3, 85+4}}, //47 leftmadm 2

  {Going2KillEvery1_ArcMain_DownMadM0, {  0,  0,145,132}, {223+3, 93+4}}, //48 downmadm 1
  {Going2KillEvery1_ArcMain_DownMadM1, {  0,  0,146,132}, {222+3, 93+4}}, //49 downmadm 2

  {Going2KillEvery1_ArcMain_UpMadM0, {  0,  0,110,115}, {155+3, 85+4}}, //50 upmadm 1
  {Going2KillEvery1_ArcMain_UpMadM0, {110,  0,108,114}, {157+3, 84+4}}, //51 upmadm 2

  {Going2KillEvery1_ArcMain_RightMadM0, {  0,  0,148,122}, {179+3, 94+4}}, //52 rightmadm 1
  {Going2KillEvery1_ArcMain_RightMadM0, {  0,122,149,122}, {182+3, 94+4}}, //53 rightmadm 2
};

static const Animation char_going2killevery1_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 8, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 11, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, 14, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 15, 16, 17, 17, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 18, 19, 19, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 20, 21, 21, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 22, 23, 23, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 24, 25, 25, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

static const Animation char_going2killevery1_anim2[PlayerAnim_Max] = {
	{2, (const u8[]){ 26, 27, 28, 29, 30, 31, 32, 33, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 34, 34, 35, 36, 36, 36, 36, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 37, 37, 38, 39, 39, 39, 39, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 40, 40, 41, 42, 42, 42, 42, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 43, 43, 44, 45, 45, 45, 45, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 46, 47, 47, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 48, 49, 49, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 50, 51, 51, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 52, 53, 53, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Going2KillEvery1 player functions
void Char_Going2KillEvery1_SetFrame(void *user, u8 frame)
{
	Char_Going2KillEvery1 *this = (Char_Going2KillEvery1*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_going2killevery1_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Going2KillEvery1_Tick(Character *character)
{
	Char_Going2KillEvery1 *this = (Char_Going2KillEvery1*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Who)
	{
		if (stage.camswitch == 0)
		{
			this->character.focus_x = FIXED_DEC(-240,1);
			this->character.focus_y = FIXED_DEC(-85,1);
			this->character.focus_zoom = FIXED_DEC(950,1024);
		}
		if (stage.camswitch == 1)
		{
			this->character.focus_x = FIXED_DEC(-149,1);
			this->character.focus_y = FIXED_DEC(-55,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.camswitch == 2)
		{
			this->character.focus_x = FIXED_DEC(-336,1);
			this->character.focus_y = FIXED_DEC(-55,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
	}
	
	//Mad switch thing
	if (stage.stage_id == StageId_Who)
	{
		if (stage.song_step == -29)
			Animatable_Init(&this->character.animatable, char_going2killevery1_anim);
		if (stage.song_step == 872)
			Animatable_Init(&this->character.animatable, char_going2killevery1_anim2);
		if (stage.song_step == 985)
			Animatable_Init(&this->character.animatable, char_going2killevery1_anim);
		if (stage.song_step == 1022)
			Animatable_Init(&this->character.animatable, char_going2killevery1_anim2);
		if (stage.song_step == 1055)
			Animatable_Init(&this->character.animatable, char_going2killevery1_anim);
		if (stage.song_step == 1080)
			Animatable_Init(&this->character.animatable, char_going2killevery1_anim2);
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
	Animatable_Animate(&character->animatable, (void*)this, Char_Going2KillEvery1_SetFrame);
	Character_Draw(character, &this->tex, &char_going2killevery1_frame[this->frame]);
}

void Char_Going2KillEvery1_SetAnim(Character *character, u8 anim)
{
	Char_Going2KillEvery1 *this = (Char_Going2KillEvery1*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Going2KillEvery1_Free(Character *character)
{
	Char_Going2KillEvery1 *this = (Char_Going2KillEvery1*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Going2KillEvery1_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_Going2KillEvery1 *this = Mem_Alloc(sizeof(Char_Going2KillEvery1));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Going2KillEvery1_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Going2KillEvery1_Tick;
	this->character.set_anim = Char_Going2KillEvery1_SetAnim;
	this->character.free = Char_Going2KillEvery1_Free;
	
	Animatable_Init(&this->character.animatable, char_going2killevery1_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 4;
	
	//health bar color
	this->character.health_bar = 0xFFEAEAF5;
	
	
	this->character.focus_x = FIXED_DEC(-240,1);
	this->character.focus_y = FIXED_DEC(-85,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\GOING2KI.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "leftm0.tim",
  "leftm1.tim",
  "downm0.tim",
  "upm0.tim",
  "upm1.tim",
  "rightm0.tim",
  "idlemad0.tim",
  "idlemad1.tim",
  "leftmad0.tim",
  "leftmad1.tim",
  "downmad0.tim",
  "downmad1.tim",
  "downmad2.tim",
  "upmad0.tim",
  "rightma0.tim",
  "rightma1.tim",
  "leftmam0.tim",
  "downmam0.tim",
  "downmam1.tim",
  "upmadm0.tim",
  "rightmm0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
