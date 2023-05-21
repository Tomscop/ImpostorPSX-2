/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ziffy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Ziffy player types
enum
{
  Ziffy_ArcMain_Idle0,
  Ziffy_ArcMain_Idle1,
  Ziffy_ArcMain_Idle2,
  Ziffy_ArcMain_Idle3,
  Ziffy_ArcMain_Left0,
  Ziffy_ArcMain_Left1,
  Ziffy_ArcMain_Down0,
  Ziffy_ArcMain_Down1,
  Ziffy_ArcMain_Up0,
  Ziffy_ArcMain_Up1,
  Ziffy_ArcMain_Right0,
  Ziffy_ArcMain_Right1,
  Ziffy_ArcMain_LeftAlt0,
  Ziffy_ArcMain_LeftAlt1,
  Ziffy_ArcMain_DownAlt0,
  Ziffy_ArcMain_DownAlt1,
  Ziffy_ArcMain_UpAlt0,
  Ziffy_ArcMain_UpAlt1,
  Ziffy_ArcMain_RightAlt0,
  Ziffy_ArcMain_RightAlt1,
  Ziffy_ArcMain_Help0,
  Ziffy_ArcMain_Help1,
  Ziffy_ArcMain_Help2,
  Ziffy_ArcMain_Help3,
  Ziffy_ArcMain_Help4,
  Ziffy_ArcMain_Help5,
  Ziffy_ArcMain_Help6,
  Ziffy_ArcMain_Help7,
  Ziffy_ArcMain_Help8,
  Ziffy_ArcMain_Help9,
  Ziffy_ArcMain_Help10,
  Ziffy_ArcMain_Help11,
  Ziffy_ArcMain_Help12,
  Ziffy_ArcMain_Help13,
  Ziffy_ArcMain_Help14,
  Ziffy_ArcMain_Help15,
  Ziffy_ArcMain_Help16,
  Ziffy_ArcMain_Help17,
  Ziffy_ArcMain_Help18,
  Ziffy_ArcMain_Help19,
  Ziffy_ArcMain_Help20,
  Ziffy_ArcMain_Help21,
	
	Ziffy_ArcMain_Max,
};

#define Ziffy_Arc_Max Ziffy_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Ziffy_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_Ziffy;

//Ziffy player definitions
static const CharFrame char_ziffy_frame[] = {
  {Ziffy_ArcMain_Idle0, {  0,  0, 71,140}, {157,154}}, //0 idle 1
  {Ziffy_ArcMain_Idle0, { 71,  0, 71,140}, {157,154}}, //1 idle 2
  {Ziffy_ArcMain_Idle0, {142,  0, 72,138}, {159,152}}, //2 idle 3
  {Ziffy_ArcMain_Idle1, {  0,  0, 72,138}, {159,151}}, //3 idle 4
  {Ziffy_ArcMain_Idle1, { 72,  0, 70,139}, {159,152}}, //4 idle 5
  {Ziffy_ArcMain_Idle1, {142,  0, 70,138}, {159,152}}, //5 idle 6
  {Ziffy_ArcMain_Idle2, {  0,  0, 67,145}, {157,159}}, //6 idle 7
  {Ziffy_ArcMain_Idle2, { 67,  0, 67,146}, {157,159}}, //7 idle 8
  {Ziffy_ArcMain_Idle2, {134,  0, 65,147}, {155,160}}, //8 idle 9
  {Ziffy_ArcMain_Idle3, {  0,  0, 66,147}, {156,160}}, //9 idle 10
  {Ziffy_ArcMain_Idle3, { 66,  0, 65,146}, {155,159}}, //10 idle 11
  {Ziffy_ArcMain_Idle3, {131,  0, 65,146}, {155,159}}, //11 idle 12

  {Ziffy_ArcMain_Left0, {  0,  0, 66,150}, {155,163}}, //12 left 1
  {Ziffy_ArcMain_Left0, { 66,  0, 66,151}, {155,163}}, //13 left 2
  {Ziffy_ArcMain_Left0, {132,  0, 67,151}, {157,163}}, //14 left 3
  {Ziffy_ArcMain_Left1, {  0,  0, 67,151}, {157,163}}, //15 left 4

  {Ziffy_ArcMain_Down0, {  0,  0, 66,141}, {153,155}}, //16 down 1
  {Ziffy_ArcMain_Down0, { 66,  0, 66,140}, {153,154}}, //17 down 2
  {Ziffy_ArcMain_Down0, {132,  0, 67,138}, {154,152}}, //18 down 3
  {Ziffy_ArcMain_Down1, {  0,  0, 67,138}, {154,152}}, //19 down 4

  {Ziffy_ArcMain_Up0, {  0,  0, 65,158}, {150,171}}, //20 up 1
  {Ziffy_ArcMain_Up0, { 65,  0, 64,158}, {150,171}}, //21 up 2
  {Ziffy_ArcMain_Up0, {129,  0, 66,157}, {151,169}}, //22 up 3
  {Ziffy_ArcMain_Up1, {  0,  0, 66,156}, {151,169}}, //23 up 4

  {Ziffy_ArcMain_Right0, {  0,  0, 65,141}, {151,154}}, //24 right 1
  {Ziffy_ArcMain_Right0, { 65,  0, 64,141}, {151,154}}, //25 right 2
  {Ziffy_ArcMain_Right0, {129,  0, 65,142}, {152,156}}, //26 right 3
  {Ziffy_ArcMain_Right1, {  0,  0, 65,141}, {152,156}}, //27 right 4

  {Ziffy_ArcMain_LeftAlt0, {  0,  0, 65,151}, {154,163}}, //28 lefta 1
  {Ziffy_ArcMain_LeftAlt0, { 65,  0, 65,150}, {154,163}}, //29 lefta 2
  {Ziffy_ArcMain_LeftAlt0, {130,  0, 67,151}, {156,163}}, //30 lefta 3
  {Ziffy_ArcMain_LeftAlt1, {  0,  0, 67,150}, {156,163}}, //31 lefta 4

  {Ziffy_ArcMain_DownAlt0, {  0,  0, 66,140}, {153,155}}, //32 downa 1
  {Ziffy_ArcMain_DownAlt0, { 66,  0, 66,141}, {153,155}}, //33 downa 2
  {Ziffy_ArcMain_DownAlt0, {132,  0, 67,138}, {154,152}}, //34 downa 3
  {Ziffy_ArcMain_DownAlt1, {  0,  0, 67,138}, {154,152}}, //35 downa 4

  {Ziffy_ArcMain_UpAlt0, {  0,  0, 64,159}, {150,172}}, //36 upa 1
  {Ziffy_ArcMain_UpAlt0, { 64,  0, 64,159}, {150,172}}, //37 upa 2
  {Ziffy_ArcMain_UpAlt0, {128,  0, 66,156}, {151,170}}, //38 upa 3
  {Ziffy_ArcMain_UpAlt1, {  0,  0, 66,156}, {151,170}}, //39 upa 4

  {Ziffy_ArcMain_RightAlt0, {  0,  0, 64,140}, {150,153}}, //40 righta 1
  {Ziffy_ArcMain_RightAlt0, { 64,  0, 64,141}, {151,154}}, //41 righta 2
  {Ziffy_ArcMain_RightAlt0, {128,  0, 65,141}, {152,156}}, //42 righta 3
  {Ziffy_ArcMain_RightAlt1, {  0,  0, 65,141}, {152,156}}, //43 righta 4

  {Ziffy_ArcMain_Help0, {  0,  0, 65,146}, {154,159}}, //44 help 1
  {Ziffy_ArcMain_Help0, { 65,  0, 65,146}, {154,158}}, //45 help 2
  {Ziffy_ArcMain_Help0, {130,  0, 67,143}, {156,156}}, //46 help 3
  {Ziffy_ArcMain_Help1, {  0,  0, 66,144}, {156,157}}, //47 help 4
  {Ziffy_ArcMain_Help1, { 66,  0, 67,147}, {156,159}}, //48 help 5
  {Ziffy_ArcMain_Help1, {133,  0, 67,148}, {157,160}}, //49 help 6
  {Ziffy_ArcMain_Help2, {  0,  0, 68,147}, {157,160}}, //50 help 7
  {Ziffy_ArcMain_Help2, { 68,  0, 69,147}, {158,159}}, //51 help 8
  {Ziffy_ArcMain_Help2, {137,  0, 69,146}, {158,159}}, //52 help 9
  {Ziffy_ArcMain_Help3, {  0,  0, 68,144}, {158,157}}, //53 help 10
  {Ziffy_ArcMain_Help3, { 68,  0, 69,143}, {158,157}}, //54 help 11
  {Ziffy_ArcMain_Help3, {137,  0, 68,152}, {157,164}}, //55 help 12
  {Ziffy_ArcMain_Help4, {  0,  0, 69,151}, {158,164}}, //56 help 13
  {Ziffy_ArcMain_Help4, { 69,  0, 69,151}, {157,163}}, //57 help 14
  {Ziffy_ArcMain_Help4, {138,  0, 70,146}, {158,159}}, //58 help 15
  {Ziffy_ArcMain_Help5, {  0,  0, 69,147}, {158,159}}, //59 help 16
  {Ziffy_ArcMain_Help5, { 69,  0, 70,147}, {158,159}}, //60 help 17
  {Ziffy_ArcMain_Help5, {139,  0, 69,147}, {158,159}}, //61 help 18
  {Ziffy_ArcMain_Help6, {  0,  0, 70,146}, {158,159}}, //62 help 19
  {Ziffy_ArcMain_Help6, { 70,  0, 69,146}, {158,159}}, //63 help 20
  {Ziffy_ArcMain_Help6, {139,  0, 69,146}, {159,159}}, //64 help 21
  {Ziffy_ArcMain_Help7, {  0,  0, 70,143}, {158,156}}, //65 help 22
  {Ziffy_ArcMain_Help7, { 70,  0, 69,142}, {158,156}}, //66 help 23
  {Ziffy_ArcMain_Help7, {139,  0, 69,151}, {149,163}}, //67 help 24
  {Ziffy_ArcMain_Help8, {  0,  0, 72,151}, {149,164}}, //68 help 25
  {Ziffy_ArcMain_Help8, { 72,  0, 72,150}, {149,162}}, //69 help 26
  {Ziffy_ArcMain_Help8, {144,  0, 72,151}, {150,163}}, //70 help 27
  {Ziffy_ArcMain_Help9, {  0,  0, 72,150}, {150,163}}, //71 help 28
  {Ziffy_ArcMain_Help9, { 72,  0, 72,149}, {150,161}}, //72 help 29
  {Ziffy_ArcMain_Help9, {144,  0, 65,146}, {154,159}}, //73 help 30
  {Ziffy_ArcMain_Help10, {  0,  0, 70,144}, {159,157}}, //74 help 31
  {Ziffy_ArcMain_Help10, { 70,  0, 70,143}, {159,156}}, //75 help 32
  {Ziffy_ArcMain_Help10, {140,  0, 70,143}, {159,155}}, //76 help 33
  {Ziffy_ArcMain_Help11, {  0,  0, 70,142}, {160,154}}, //77 help 34
  {Ziffy_ArcMain_Help11, { 70,  0, 70,142}, {160,154}}, //78 help 35
  {Ziffy_ArcMain_Help11, {140,  0, 76,138}, {165,150}}, //79 help 36
  {Ziffy_ArcMain_Help12, {  0,  0, 65,141}, {152,153}}, //80 help 37
  {Ziffy_ArcMain_Help12, { 65,  0, 69,147}, {150,160}}, //81 help 38
  {Ziffy_ArcMain_Help12, {134,  0, 67,148}, {149,161}}, //82 help 39
  {Ziffy_ArcMain_Help13, {  0,  0, 68,148}, {150,161}}, //83 help 40
  {Ziffy_ArcMain_Help13, { 68,  0, 67,148}, {149,161}}, //84 help 41
  {Ziffy_ArcMain_Help13, {135,  0, 67,148}, {150,161}}, //85 help 42
  {Ziffy_ArcMain_Help14, {  0,  0, 67,148}, {150,161}}, //86 help 43
  {Ziffy_ArcMain_Help14, { 67,  0, 67,148}, {149,161}}, //87 help 44
  {Ziffy_ArcMain_Help14, {134,  0, 67,148}, {150,161}}, //88 help 45
  {Ziffy_ArcMain_Help15, {  0,  0, 68,148}, {150,161}}, //89 help 46
  {Ziffy_ArcMain_Help15, { 68,  0, 68,148}, {149,160}}, //90 help 47
  {Ziffy_ArcMain_Help15, {136,  0, 70,151}, {149,163}}, //91 help 48
  {Ziffy_ArcMain_Help16, {  0,  0, 69,149}, {150,161}}, //92 help 49
  {Ziffy_ArcMain_Help16, { 69,  0, 67,148}, {149,161}}, //93 help 50
  {Ziffy_ArcMain_Help16, {136,  0, 70,147}, {150,160}}, //94 help 51
  {Ziffy_ArcMain_Help17, {  0,  0, 71,146}, {150,159}}, //95 help 52
  {Ziffy_ArcMain_Help17, { 71,  0, 73,144}, {150,156}}, //96 help 53
  {Ziffy_ArcMain_Help17, {144,  0, 72,144}, {150,156}}, //97 help 54
  {Ziffy_ArcMain_Help18, {  0,  0, 65,145}, {153,158}}, //98 help 55
  {Ziffy_ArcMain_Help18, { 65,  0, 64,144}, {152,158}}, //99 help 56
  {Ziffy_ArcMain_Help18, {129,  0, 65,145}, {152,158}}, //100 help 57
  {Ziffy_ArcMain_Help19, {  0,  0, 65,145}, {152,158}}, //101 help 58
  {Ziffy_ArcMain_Help19, { 65,  0, 64,146}, {152,158}}, //102 help 59
  {Ziffy_ArcMain_Help19, {129,  0, 64,146}, {152,158}}, //103 help 60
  {Ziffy_ArcMain_Help20, {  0,  0, 65,146}, {152,158}}, //104 help 61
  {Ziffy_ArcMain_Help20, { 65,  0, 65,145}, {152,157}}, //105 help 62
  {Ziffy_ArcMain_Help20, {130,  0, 65,148}, {152,161}}, //106 help 63
  {Ziffy_ArcMain_Help21, {  0,  0, 65,146}, {152,159}}, //107 help 64
  {Ziffy_ArcMain_Help21, { 65,  0, 64,145}, {151,158}}, //108 help 65
};

static const Animation char_ziffy_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 10, 11, ASCR_CHGANI, CharAnim_Special2}}, //CharAnim_Idle
	{1, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 1}},             //CharAnim_Left
	{1, (const u8[]){ 28, 29, 30, 31, ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 16, 17, 18, 19, ASCR_BACK, 1}},             //CharAnim_Down
	{1, (const u8[]){ 32, 33, 34, 35, ASCR_BACK, 1}},       //CharAnim_DownAlt
	{1, (const u8[]){ 20, 21, 22, 23, ASCR_BACK, 1}},             //CharAnim_Up
	{1, (const u8[]){ 36, 37, 38, 39, ASCR_BACK, 1}},       //CharAnim_UpAlt
	{1, (const u8[]){ 24, 25, 26, 27, ASCR_BACK, 1}},             //CharAnim_Right
	{1, (const u8[]){ 40, 41, 42, 43, ASCR_BACK, 1}},       //CharAnim_RightAlt
	
	{2, (const u8[]){ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 108, ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
	{1, (const u8[]){ 10, 11, ASCR_CHGANI, CharAnim_Special2}},       //CharAnim_Special2
};

//Ziffy player functions
void Char_Ziffy_SetFrame(void *user, u8 frame)
{
	Char_Ziffy *this = (Char_Ziffy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ziffy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Ziffy_Tick(Character *character)
{
	Char_Ziffy *this = (Char_Ziffy*)character;
	
	if (character->animatable.anim != CharAnim_Special1)
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
		if ((character->animatable.anim != CharAnim_Left &&
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
			case StageId_Torture:
				if (stage.song_step == 1024) //ROZEBUD ROZEBUD HELP ME THEYRE GONNA KILL ME
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_Ziffy_SetFrame);
	Character_Draw(character, &this->tex, &char_ziffy_frame[this->frame]);
}

void Char_Ziffy_SetAnim(Character *character, u8 anim)
{
	Char_Ziffy *this = (Char_Ziffy*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Ziffy_Free(Character *character)
{
	Char_Ziffy *this = (Char_Ziffy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Ziffy_New(fixed_t x, fixed_t y)
{
	//Allocate ziffy object
	Char_Ziffy *this = Mem_Alloc(sizeof(Char_Ziffy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Ziffy_New] Failed to allocate ziffy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Ziffy_Tick;
	this->character.set_anim = Char_Ziffy_SetAnim;
	this->character.free = Char_Ziffy_Free;
	
	Animatable_Init(&this->character.animatable, char_ziffy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;
	
	//health bar color
	this->character.health_bar = 0xFFFC972B;
	
	this->character.focus_x = FIXED_DEC(-167,1);
	this->character.focus_y = FIXED_DEC(205,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\ZIFFY.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "lefta0.tim",
  "lefta1.tim",
  "downa0.tim",
  "downa1.tim",
  "upa0.tim",
  "upa1.tim",
  "righta0.tim",
  "righta1.tim",
  "help0.tim",
  "help1.tim",
  "help2.tim",
  "help3.tim",
  "help4.tim",
  "help5.tim",
  "help6.tim",
  "help7.tim",
  "help8.tim",
  "help9.tim",
  "help10.tim",
  "help11.tim",
  "help12.tim",
  "help13.tim",
  "help14.tim",
  "help15.tim",
  "help16.tim",
  "help17.tim",
  "help18.tim",
  "help19.tim",
  "help20.tim",
  "help21.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
