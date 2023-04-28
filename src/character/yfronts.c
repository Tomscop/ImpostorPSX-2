/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "yfronts.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Y Fronts character structure
enum
{
  YFronts_ArcMain_Idle0,
  YFronts_ArcMain_Idle1,
  YFronts_ArcMain_Left0,
  YFronts_ArcMain_Down0,
  YFronts_ArcMain_Up0,
  YFronts_ArcMain_Right0,
  YFronts_ArcMain_Right1,
  YFronts_ArcMain_IdleM0,
  YFronts_ArcMain_IdleM1,
  YFronts_ArcMain_LeftM0,
  YFronts_ArcMain_LeftM1,
  YFronts_ArcMain_DownM0,
  YFronts_ArcMain_UpM0,
  YFronts_ArcMain_UpM1,
  YFronts_ArcMain_RightM0,
  YFronts_ArcMain_RightM1,
	
	YFronts_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[YFronts_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_YFronts;

//Y Fronts character definitions
static const CharFrame char_yfronts_frame[] = {
  {YFronts_ArcMain_Idle0, {  0,  0,101,109}, {134, 97}}, //0 idle 1
  {YFronts_ArcMain_Idle0, {101,  0, 95,111}, {133, 99}}, //1 idle 2
  {YFronts_ArcMain_Idle0, {  0,111, 94,114}, {135,102}}, //2 idle 3
  {YFronts_ArcMain_Idle0, { 94,111, 97,115}, {140,103}}, //3 idle 4
  {YFronts_ArcMain_Idle1, {  0,  0, 96,115}, {139,103}}, //4 idle 5
  {YFronts_ArcMain_Idle1, { 96,  0, 96,115}, {140,103}}, //5 idle 6

  {YFronts_ArcMain_Left0, {  0,  0,107,112}, {155-10,100}}, //6 left 1
  {YFronts_ArcMain_Left0, {107,  0,113,112}, {157-10,100}}, //7 left 2
  {YFronts_ArcMain_Left0, {  0,112,113,112}, {157-10,100}}, //8 left 3

  {YFronts_ArcMain_Down0, {  0,  0,109,112}, {134-2,100}}, //9 down 1
  {YFronts_ArcMain_Down0, {109,  0,107,116}, {135-2,104}}, //10 down 2
  {YFronts_ArcMain_Down0, {  0,116,107,117}, {136-2,105}}, //11 down 3

  {YFronts_ArcMain_Up0, {  0,  0, 99,124}, {165-16,112}}, //12 up 1
  {YFronts_ArcMain_Up0, { 99,  0, 95,118}, {160-16,106}}, //13 up 2
  {YFronts_ArcMain_Up0, {  0,124, 95,117}, {160-16,105}}, //14 up 3

  {YFronts_ArcMain_Right0, {  0,  0,155,115}, {122+6,103}}, //15 right 1
  {YFronts_ArcMain_Right0, {  0,115,147,115}, {128+6,103}}, //16 right 2
  {YFronts_ArcMain_Right1, {  0,  0,145,115}, {129+6,103}}, //17 right 3
  
  {YFronts_ArcMain_IdleM0, {  0,  0,115,111}, {148-2, 99}}, //18 idlem 1
  {YFronts_ArcMain_IdleM0, {115,  0,115,111}, {150-2, 99}}, //19 idlem 2
  {YFronts_ArcMain_IdleM0, {  0,111,114,112}, {151-2,100}}, //20 idlem 3
  {YFronts_ArcMain_IdleM0, {114,111,114,114}, {152-2,102}}, //21 idlem 4
  {YFronts_ArcMain_IdleM1, {  0,  0,112,115}, {149-2,103}}, //22 idlem 5
  {YFronts_ArcMain_IdleM1, {112,  0,113,115}, {148-2,103}}, //23 idlem 6
  {YFronts_ArcMain_IdleM1, {  0,115,113,115}, {148-2,103}}, //24 idlem 7

  {YFronts_ArcMain_LeftM0, {  0,  0,220,114}, {173+6,101}}, //25 leftm 1
  {YFronts_ArcMain_LeftM0, {  0,114,206,113}, {168+6,101}}, //26 leftm 2
  {YFronts_ArcMain_LeftM1, {  0,  0,204,113}, {167+6,101}}, //27 leftm 3

  {YFronts_ArcMain_DownM0, {  0,  0,127,118}, {151-2,105}}, //28 downm 1
  {YFronts_ArcMain_DownM0, {127,  0,117,113}, {144-2,100}}, //29 downm 2
  {YFronts_ArcMain_DownM0, {  0,118,115,113}, {144-2,100}}, //30 downm 3

  {YFronts_ArcMain_UpM0, {  0,  0,183,122}, {176-2,109}}, //31 upm 1
  {YFronts_ArcMain_UpM0, {  0,122,169,119}, {169-2,107}}, //32 upm 2
  {YFronts_ArcMain_UpM1, {  0,  0,167,118}, {168-2,106}}, //33 upm 3

  {YFronts_ArcMain_RightM0, {  0,  0,151,114}, {128+7,102}}, //34 rightm 1
  {YFronts_ArcMain_RightM0, {  0,114,143,114}, {127+7,102}}, //35 rightm 2
  {YFronts_ArcMain_RightM1, {  0,  0,142,114}, {127+7,102}}, //36 rightm 3
};

static const Animation char_yfronts_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 11, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, 14, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 15, 16, 17, 17, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_yfronts_anim2[CharAnim_Max] = {
	{2, (const u8[]){ 18, 19, 20, 21, 22, 23, 24, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 25, 26, 27, 27, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 28, 29, 30, 30, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 31, 32, 33, 33, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 34, 35, 36, 36, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Y Fronts character functions
void Char_YFronts_SetFrame(void *user, u8 frame)
{
	Char_YFronts *this = (Char_YFronts*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_yfronts_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_YFronts_Tick(Character *character)
{
	Char_YFronts *this = (Char_YFronts*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Who)
	{
		if (stage.camswitch == 0)
		{
			this->character.focus_x = FIXED_DEC(31,1);
			this->character.focus_y = FIXED_DEC(-95,1);
			this->character.focus_zoom = FIXED_DEC(950,1024);
		}
		if (stage.camswitch == 1)
		{
			this->character.focus_x = FIXED_DEC(122,1);
			this->character.focus_y = FIXED_DEC(-65,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.camswitch == 2)
		{
			this->character.focus_x = FIXED_DEC(-65,1);
			this->character.focus_y = FIXED_DEC(-65,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.song_step == 384)
			stage.camswitch = 1;
		if (stage.song_step == 396)
			stage.camswitch = 2;
		if (stage.song_step == 402)
			stage.camswitch = 1;
		if (stage.song_step == 411)
			stage.camswitch = 2;
		if (stage.song_step == 424)
			stage.camswitch = 1;
		if (stage.song_step == 448)
			stage.camswitch = 0;
	}
	
	//Mad switch thing
	if (stage.stage_id == StageId_Who)
	{
		if (stage.song_step == -29)
			Animatable_Init(&this->character.animatable, char_yfronts_anim);
		if (stage.song_step == 852)
			Animatable_Init(&this->character.animatable, char_yfronts_anim2);
		if (stage.song_step == 1030) //changed a bit to look better this is just a note in case it ends up sucking so i know
			Animatable_Init(&this->character.animatable, char_yfronts_anim);
		if (stage.song_step == 1067)
			Animatable_Init(&this->character.animatable, char_yfronts_anim2);
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_YFronts_SetFrame);
	Character_Draw(character, &this->tex, &char_yfronts_frame[this->frame]);
}

void Char_YFronts_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_YFronts_Free(Character *character)
{
	Char_YFronts *this = (Char_YFronts*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_YFronts_New(fixed_t x, fixed_t y)
{
	//Allocate yfronts object
	Char_YFronts *this = Mem_Alloc(sizeof(Char_YFronts));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_YFronts_New] Failed to allocate yfronts object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_YFronts_Tick;
	this->character.set_anim = Char_YFronts_SetAnim;
	this->character.free = Char_YFronts_Free;
	
	Animatable_Init(&this->character.animatable, char_yfronts_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFF6066FF;
	
	this->character.focus_x = FIXED_DEC(31,1);
	this->character.focus_y = FIXED_DEC(-95,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\YFRONTS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "right1.tim",
  "idlem0.tim",
  "idlem1.tim",
  "leftm0.tim",
  "leftm1.tim",
  "downm0.tim",
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
