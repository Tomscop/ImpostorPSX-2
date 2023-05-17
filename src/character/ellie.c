/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ellie.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Ellie character structure
enum
{
  Ellie_ArcMain_Idle0,
  Ellie_ArcMain_Idle1,
  Ellie_ArcMain_Idle2,
  Ellie_ArcMain_Left0,
  Ellie_ArcMain_Left1,
  Ellie_ArcMain_Down0,
  Ellie_ArcMain_Up0,
  Ellie_ArcMain_Right0,
  Ellie_ArcMain_Sup0,
  Ellie_ArcMain_Sup1,
  Ellie_ArcMain_Sup2,
  Ellie_ArcMain_Sup3,
  Ellie_ArcMain_Sup4,
  Ellie_ArcMain_Sup5,
  Ellie_ArcMain_Sup6,
  Ellie_ArcMain_Sup7,
  Ellie_ArcMain_Sup8,
  Ellie_ArcMain_Sup9,
  Ellie_ArcMain_Sup10,
  Ellie_ArcMain_Sup11,
  Ellie_ArcMain_Sup12,
  Ellie_ArcMain_Sup13,
  Ellie_ArcMain_Sup14,
  Ellie_ArcMain_Sup15,
  Ellie_ArcMain_Sup16,
  Ellie_ArcMain_Cutscene0,
  Ellie_ArcMain_Cutscene1,
  Ellie_ArcMain_Cutscene2,
  Ellie_ArcMain_Cutscene3,
  Ellie_ArcMain_Cutscene4,
  Ellie_ArcMain_Cutscene5,
	
	Ellie_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Ellie_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Ellie;

//Ellie character definitions
static const CharFrame char_ellie_frame[] = {
  {Ellie_ArcMain_Idle0, {  0,  0,104,169}, {153,133}}, //0 idle 1
  {Ellie_ArcMain_Idle0, {104,  0,103,169}, {153,133}}, //1 idle 2
  {Ellie_ArcMain_Idle1, {  0,  0,105,171}, {153,134}}, //2 idle 3
  {Ellie_ArcMain_Idle1, {105,  0,107,174}, {153,138}}, //3 idle 4
  {Ellie_ArcMain_Idle2, {  0,  0,107,174}, {153,138}}, //4 idle 5
  {Ellie_ArcMain_Idle2, {107,  0,107,175}, {153,138}}, //5 idle 6

  {Ellie_ArcMain_Left0, {  0,  0,164,170}, {213,132}}, //6 left 1
  {Ellie_ArcMain_Left1, {  0,  0,166,172}, {212,134}}, //7 left 2

  {Ellie_ArcMain_Down0, {  0,  0,116,158}, {140,121}}, //8 down 1
  {Ellie_ArcMain_Down0, {116,  0,111,160}, {138,123}}, //9 down 2

  {Ellie_ArcMain_Up0, {  0,  0, 94,186}, {136,151}}, //10 up 1
  {Ellie_ArcMain_Up0, { 94,  0, 96,184}, {138,149}}, //11 up 2

  {Ellie_ArcMain_Right0, {  0,  0,123,171}, {139,133}}, //12 right 1
  {Ellie_ArcMain_Right0, {123,  0,121,172}, {138,134}}, //13 right 2

  {Ellie_ArcMain_Sup0, {  0,  0,147,242}, {140,403}}, //14 sup 1
  {Ellie_ArcMain_Sup1, {  0,  0,147,242}, {140,230}}, //15 sup 2
  {Ellie_ArcMain_Sup2, {  0,  0,147,242}, {140,235}}, //16 sup 3
  {Ellie_ArcMain_Sup3, {  0,  0,147,242}, {140,235}}, //17 sup 4
  {Ellie_ArcMain_Sup4, {  0,  0,146,241}, {140,234}}, //18 sup 5
  {Ellie_ArcMain_Sup5, {  0,  0,147,241}, {141,235}}, //19 sup 6
  {Ellie_ArcMain_Sup6, {  0,  0,147,241}, {141,235}}, //20 sup 7
  {Ellie_ArcMain_Sup7, {  0,  0,147,245}, {141,236}}, //21 sup 8
  {Ellie_ArcMain_Sup8, {  0,  0,152,255}, {145,230}}, //22 sup 9
  {Ellie_ArcMain_Sup9, {  0,  0,160,255}, {153,217}}, //23 sup 10
  {Ellie_ArcMain_Sup10, {  0,  0,160,255}, {153,217}}, //24 sup 11
  {Ellie_ArcMain_Sup11, {  0,  0,160,255}, {153,218}}, //25 sup 12
  {Ellie_ArcMain_Sup12, {  0,  0,159,255}, {152,217}}, //26 sup 13
  {Ellie_ArcMain_Sup13, {  0,  0,159,255}, {152,217}}, //27 sup 14
  {Ellie_ArcMain_Sup14, {  0,  0,160,255}, {153,217}}, //28 sup 15
  {Ellie_ArcMain_Sup15, {  0,  0,108,174}, {153,137}}, //29 sup 16
  {Ellie_ArcMain_Sup15, {108,  0,107,175}, {153,137}}, //30 sup 17
  {Ellie_ArcMain_Sup16, {  0,  0,108,174}, {153,136}}, //31 sup 18
  {Ellie_ArcMain_Sup16, {108,  0,107,174}, {153,136}}, //32 sup 19

  {Ellie_ArcMain_Cutscene0, {  0,  0,108,174}, {154,138}}, //33 cutscene 1
  {Ellie_ArcMain_Cutscene0, {108,  0,109,174}, {154,138}}, //34 cutscene 2
  {Ellie_ArcMain_Cutscene1, {  0,  0,110,173}, {156,137}}, //35 cutscene 3
  {Ellie_ArcMain_Cutscene1, {110,  0,106,172}, {155,136}}, //36 cutscene 4
  {Ellie_ArcMain_Cutscene2, {  0,  0,106,172}, {155,136}}, //37 cutscene 5
  {Ellie_ArcMain_Cutscene2, {106,  0,109,173}, {155,137}}, //38 cutscene 6
  {Ellie_ArcMain_Cutscene3, {  0,  0,109,173}, {155,137}}, //39 cutscene 7
  {Ellie_ArcMain_Cutscene3, {109,  0,110,173}, {155,137}}, //40 cutscene 8
  {Ellie_ArcMain_Cutscene4, {  0,  0,110,173}, {155,137}}, //41 cutscene 9
  {Ellie_ArcMain_Cutscene4, {110,  0,110,173}, {156,137}}, //42 cutscene 10
  {Ellie_ArcMain_Cutscene5, {  0,  0,110,173}, {156,137}}, //43 cutscene 11
  {Ellie_ArcMain_Cutscene5, {110,  0,110,174}, {156,137}}, //44 cutscene 12
};

static const Animation char_ellie_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 7, 7,  ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 11, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 12, 13, 13, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt

	{1, (const u8[]){ 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 18, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, ASCR_BACK, 1}},   //CharAnim_Special1
	{1, (const u8[]){ 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 41, 41, 42, 42, 43, 43, 44, ASCR_CHGANI, CharAnim_Special3}},   //CharAnim_Special2
	{2, (const u8[]){ 44, ASCR_CHGANI, CharAnim_Special3}},   //CharAnim_Special3
};

//Ellie character functions
void Char_Ellie_SetFrame(void *user, u8 frame)
{
	Char_Ellie *this = (Char_Ellie*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ellie_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Ellie_Tick(Character *character)
{
	Char_Ellie *this = (Char_Ellie*)character;
	
	//Perform idle dance
	if ((character->animatable.anim != CharAnim_Special2) || (character->animatable.anim != CharAnim_Special3))
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Reinforcements:
				if (stage.song_step == 696) //sup
					character->set_anim(character, CharAnim_Special1);
//				if (stage.song_step == idk) //armed cutscene
//					character->set_anim(character, CharAnim_Special2);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Ellie_SetFrame);
	if ((stage.song_step >= 698) || (character->animatable.anim == CharAnim_Special1))
		Character_Draw(character, &this->tex, &char_ellie_frame[this->frame]);
}

void Char_Ellie_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Ellie_Free(Character *character)
{
	Char_Ellie *this = (Char_Ellie*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Ellie_New(fixed_t x, fixed_t y)
{
	//Allocate ellie object
	Char_Ellie *this = Mem_Alloc(sizeof(Char_Ellie));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Ellie_New] Failed to allocate ellie object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Ellie_Tick;
	this->character.set_anim = Char_Ellie_SetAnim;
	this->character.free = Char_Ellie_Free;
	
	Animatable_Init(&this->character.animatable, char_ellie_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFBDD7D8;
	
	this->character.focus_x = FIXED_DEC(-208,1);
	this->character.focus_y = FIXED_DEC(-55,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\ELLIE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "sup0.tim",
  "sup1.tim",
  "sup2.tim",
  "sup3.tim",
  "sup4.tim",
  "sup5.tim",
  "sup6.tim",
  "sup7.tim",
  "sup8.tim",
  "sup9.tim",
  "sup10.tim",
  "sup11.tim",
  "sup12.tim",
  "sup13.tim",
  "sup14.tim",
  "sup15.tim",
  "sup16.tim",
  "cutscene0.tim",
  "cutscene1.tim",
  "cutscene2.tim",
  "cutscene3.tim",
  "cutscene4.tim",
  "cutscene5.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}