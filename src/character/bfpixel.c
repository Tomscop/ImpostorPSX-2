/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfpixel.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Pixel player types
enum
{
  BFPixel_ArcMain_Idle0,
  BFPixel_ArcMain_Left0,
  BFPixel_ArcMain_Down0,
  BFPixel_ArcMain_Up0,
  BFPixel_ArcMain_Right0,
  BFPixel_ArcMain_LeftM0,
  BFPixel_ArcMain_DownM0,
  BFPixel_ArcMain_UpM0,
  BFPixel_ArcMain_RightM0,
  BFPixel_ArcMain_IdleS0,
  BFPixel_ArcMain_LeftS0,
  BFPixel_ArcMain_DownS0,
  BFPixel_ArcMain_UpS0,
  BFPixel_ArcMain_RightS0,
  BFPixel_ArcMain_LeftSM0,
  BFPixel_ArcMain_DownSM0,
  BFPixel_ArcMain_UpSM0,
  BFPixel_ArcMain_RighSM0,
  BFPixel_ArcMain_Shoot0,
	
	BFPixel_ArcMain_Max,
};

#define BFPixel_Arc_Max BFPixel_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFPixel_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFPixel;

//Boyfriend Pixel player definitions
static const CharFrame char_bfpixel_frame[] = {
  {BFPixel_ArcMain_Idle0, {  0,  0, 54, 55}, {167,154}}, //0 idle 1
  {BFPixel_ArcMain_Idle0, { 55,  0, 52, 56}, {166,154}}, //1 idle 2
  {BFPixel_ArcMain_Idle0, {108,  0, 51, 57}, {164,156}}, //2 idle 3
  {BFPixel_ArcMain_Idle0, {160,  0, 51, 58}, {164,157}}, //3 idle 4
  {BFPixel_ArcMain_Idle0, {  0, 59, 52, 58}, {165,157}}, //4 idle 5

  {BFPixel_ArcMain_Left0, {  0,  0, 52, 58}, {168,157}}, //5 left 1
  {BFPixel_ArcMain_Left0, { 53,  0, 51, 58}, {167,157}}, //6 left 2

  {BFPixel_ArcMain_Down0, {  0,  0, 51, 54}, {165,153}}, //7 down 1
  {BFPixel_ArcMain_Down0, { 52,  0, 51, 55}, {166,154}}, //8 down 2

  {BFPixel_ArcMain_Up0, {  0,  0, 53, 61}, {163,160}}, //9 up 1
  {BFPixel_ArcMain_Up0, { 54,  0, 54, 60}, {164,159}}, //10 up 2

  {BFPixel_ArcMain_Right0, {  0,  0, 51, 57}, {161,156}}, //11 right 1
  {BFPixel_ArcMain_Right0, { 52,  0, 52, 57}, {162,156}}, //12 right 2

  {BFPixel_ArcMain_LeftM0, {  0,  0, 50, 59}, {167,158}}, //13 leftm 1
  {BFPixel_ArcMain_LeftM0, { 51,  0, 51, 58}, {167,157}}, //14 leftm 2

  {BFPixel_ArcMain_DownM0, {  0,  0, 52, 57}, {167,156}}, //15 downm 1
  {BFPixel_ArcMain_DownM0, { 53,  0, 51, 56}, {166,155}}, //16 downm 2

  {BFPixel_ArcMain_UpM0, {  0,  0, 53, 61}, {163,160}}, //17 upm 1
  {BFPixel_ArcMain_UpM0, { 54,  0, 54, 60}, {164,159}}, //18 upm 2

  {BFPixel_ArcMain_RightM0, {  0,  0, 53, 59}, {162,158}}, //19 rightm 1
  {BFPixel_ArcMain_RightM0, { 54,  0, 52, 58}, {162,157}}, //20 rightm 2
  
  {BFPixel_ArcMain_IdleS0, {  0,  0, 54, 55}, {165+2,154}}, //21 idles 1
  {BFPixel_ArcMain_IdleS0, { 55,  0, 52, 55}, {164+2,154}}, //22 idles 2
  {BFPixel_ArcMain_IdleS0, {108,  0, 52, 57}, {163+2,156}}, //23 idles 3
  {BFPixel_ArcMain_IdleS0, {161,  0, 52, 58}, {163+2,157}}, //24 idles 4
  {BFPixel_ArcMain_IdleS0, {  0, 59, 53, 58}, {164+2,157}}, //25 idles 5

  {BFPixel_ArcMain_LeftS0, {  0,  0, 52, 58}, {166+2,157}}, //26 lefts 1
  {BFPixel_ArcMain_LeftS0, { 53,  0, 51, 58}, {165+2,157}}, //27 lefts 2

  {BFPixel_ArcMain_DownS0, {  0,  0, 52, 57}, {164+2,153}}, //28 downs 1
  {BFPixel_ArcMain_DownS0, { 53,  0, 50, 57}, {163+2,154}}, //29 downs 2

  {BFPixel_ArcMain_UpS0, {  0,  0, 56, 61}, {164+2,160}}, //30 ups 1
  {BFPixel_ArcMain_UpS0, { 57,  0, 57, 60}, {165+2,159}}, //31 ups 2

  {BFPixel_ArcMain_RightS0, {  0,  0, 49, 58}, {157+2,156}}, //32 rights 1
  {BFPixel_ArcMain_RightS0, { 50,  0, 50, 58}, {158+2,156}}, //33 rights 2

  {BFPixel_ArcMain_LeftSM0, {  0,  0, 50, 58}, {165+2,157}}, //34 leftsm 1
  {BFPixel_ArcMain_LeftSM0, { 51,  0, 51, 58}, {165+2,157}}, //35 leftsm 2

  {BFPixel_ArcMain_DownSM0, {  0,  0, 51, 60}, {164+2,156}}, //36 downsm 1
  {BFPixel_ArcMain_DownSM0, { 52,  0, 50, 58}, {163+2,155}}, //37 downsm 2

  {BFPixel_ArcMain_UpSM0, {  0,  0, 55, 61}, {163+2,160}}, //38 upsm 1
  {BFPixel_ArcMain_UpSM0, { 56,  0, 57, 60}, {165+2,159}}, //39 upsm 2

  {BFPixel_ArcMain_RighSM0, {  0,  0, 51, 60}, {158+2,158}}, //40 righsm 1
  {BFPixel_ArcMain_RighSM0, { 52,  0, 50, 59}, {158+2,157}}, //41 righsm 2

  {BFPixel_ArcMain_Shoot0, {  0,  0, 59, 57}, {167+2,156}}, //42 shoot 1
  {BFPixel_ArcMain_Shoot0, { 60,  0, 61, 57}, {169+2,156}}, //43 shoot 2
  {BFPixel_ArcMain_Shoot0, {122,  0, 62, 57}, {170+2,156}}, //44 shoot 3
  {BFPixel_ArcMain_Shoot0, {185,  0, 55, 58}, {166+2,157}}, //45 shoot 4
};

static const Animation char_bfpixel_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {2, (const u8[]){ 42, 43, 44, 45, 45, 45, 45, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 15, 16, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 17, 18, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 19, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

static const Animation char_bfpixel_anim2[PlayerAnim_Max] = {
	{2, (const u8[]){ 21, 22, 23, 24, 25, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 26, 27, 27, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 28, 29, 29, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 30, 31, 31, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 32, 33, 33, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {2, (const u8[]){ 42, 43, 44, 45, 45, 45, 45, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 34, 35, 35, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 36, 37, 37, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 38, 39, 39, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 40, 41, 41, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Pixel player functions
void Char_BFPixel_SetFrame(void *user, u8 frame)
{
	Char_BFPixel *this = (Char_BFPixel*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfpixel_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFPixel_Tick(Character *character)
{
	Char_BFPixel *this = (Char_BFPixel*)character;
	
	if(character->animatable.anim  != CharAnim_Special1)
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
	
	if ((stage.stage_id == StageId_Rivals) && (stage.song_step == 1034))
		Animatable_Init(&this->character.animatable, char_bfpixel_anim2);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Rivals: //Sussy
				if ((stage.song_step == 1034) && stage.flag & STAGE_FLAG_JUST_STEP)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFPixel_SetFrame);
	Character_Draw(character, &this->tex, &char_bfpixel_frame[this->frame]);
}

void Char_BFPixel_SetAnim(Character *character, u8 anim)
{
	Char_BFPixel *this = (Char_BFPixel*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFPixel_Free(Character *character)
{
	Char_BFPixel *this = (Char_BFPixel*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFPixel_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriendpixel object
	Char_BFPixel *this = Mem_Alloc(sizeof(Char_BFPixel));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFPixel_New] Failed to allocate boyfriendpixel object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFPixel_Tick;
	this->character.set_anim = Char_BFPixel_SetAnim;
	this->character.free = Char_BFPixel_Free;
	
	if (stage.stage_id != StageId_Chewmate)
		Animatable_Init(&this->character.animatable, char_bfpixel_anim);
	else
		Animatable_Init(&this->character.animatable, char_bfpixel_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 0;
	
	//health bar color
	this->character.health_bar = 0xFF7BD6F6;

	this->character.focus_x = FIXED_DEC(-364,1);
	this->character.focus_y = FIXED_DEC(-296,1);
	this->character.focus_zoom = FIXED_DEC(511,512);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(2,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFPIXEL.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftm0.tim",
  "downm0.tim",
  "upm0.tim",
  "rightm0.tim",
  "idles0.tim",
  "lefts0.tim",
  "downs0.tim",
  "ups0.tim",
  "rights0.tim",
  "leftsm0.tim",
  "downsm0.tim",
  "upsm0.tim",
  "righsm0.tim",
  "shoot0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
