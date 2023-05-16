/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

int zoomout = 0;

//Boyfriend player types
enum
{
	BF_ArcMain_BF0,
	BF_ArcMain_BF1,
	BF_ArcMain_BF2,
	BF_ArcMain_BF3,
	BF_ArcMain_BF4,
	BF_ArcMain_BF5,
	BF_ArcMain_BF6,
	
	BF_ArcMain_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BF;

//Boyfriend player definitions
static const CharFrame char_bf_frame[] = {
	{BF_ArcMain_BF0, {  0,   0, 102,  99}, { 53,  92}}, //0 idle 1
	{BF_ArcMain_BF0, {103,   0, 102,  99}, { 53,  92}}, //1 idle 2
	{BF_ArcMain_BF0, {  0, 100, 102, 101}, { 53,  94}}, //2 idle 3
	{BF_ArcMain_BF0, {103, 100, 103, 104}, { 53,  97}}, //3 idle 4
	{BF_ArcMain_BF1, {  0,   0, 103, 104}, { 53,  97}}, //4 idle 5
	
	{BF_ArcMain_BF1, {104,   0,  96, 102}, { 56,  95}}, //5 left 1
	{BF_ArcMain_BF1, {  0, 105,  94, 102}, { 54,  95}}, //6 left 2
	
	{BF_ArcMain_BF1, { 95, 103,  94,  89}, { 52,  82}}, //7 down 1
	{BF_ArcMain_BF2, {  0,   0,  94,  90}, { 52,  83}}, //8 down 2
	
	{BF_ArcMain_BF2, { 95,   0,  93, 112}, { 41, 104}}, //9 up 1
	{BF_ArcMain_BF2, {  0,  91,  94, 111}, { 42, 103}}, //10 up 2
	
	{BF_ArcMain_BF2, { 95, 113, 102, 102}, { 41,  95}}, //11 right 1
	{BF_ArcMain_BF3, {  0,   0, 102, 102}, { 41,  95}}, //12 right 2
	
	{BF_ArcMain_BF3, {103,   0,  99, 105}, { 54,  98}}, //13 peace 1
	{BF_ArcMain_BF3, {  0, 103, 104, 103}, { 54,  96}}, //14 peace 2
	{BF_ArcMain_BF3, {105, 106, 104, 104}, { 54,  97}}, //15 peace 3
	
	{BF_ArcMain_BF4, {  0,  0,102,103}, { 53,  96}}, //16 shock
	
	{BF_ArcMain_BF5, {  0,   0,  93, 108}, { 52, 101}}, //17 left miss 1
	{BF_ArcMain_BF5, { 94,   0,  93, 108}, { 52, 101}}, //18 left miss 2
	
	{BF_ArcMain_BF5, {  0, 109,  95,  98}, { 50,  90}}, //19 down miss 1
	{BF_ArcMain_BF5, { 96, 109,  95,  97}, { 50,  89}}, //20 down miss 2
	
	{BF_ArcMain_BF6, {  0,   0,  90, 107}, { 44,  99}}, //21 up miss 1
	{BF_ArcMain_BF6, { 91,   0,  89, 108}, { 44, 100}}, //22 up miss 2
	
	{BF_ArcMain_BF6, {  0, 108,  99, 108}, { 42, 101}}, //23 right miss 1
	{BF_ArcMain_BF6, {100, 109, 101, 108}, { 43, 101}}, //24 right miss 2
};

static const Animation char_bf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {2, (const u8[]){ 16, ASCR_CHGANI, CharAnim_Special1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 17, 17, 18, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 19, 19, 20, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 21, 21, 22, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 23, 23, 24, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){ 13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
};

//Boyfriend player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Dlow)
	{
		if (stage.song_step == 0)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 96)
			this->character.focus_zoom = FIXED_DEC(857,1024);
		if (stage.song_step == 104)
			this->character.focus_zoom = FIXED_DEC(889,1024);
		if (stage.song_step == 111)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 127)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 400)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 416)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 432)
			this->character.focus_zoom = FIXED_DEC(840,1024);
		if (stage.song_step == 440)
			this->character.focus_zoom = FIXED_DEC(873,1024);
		if (stage.song_step == 448)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 464)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 480)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 496)
			this->character.focus_zoom = FIXED_DEC(840,1024);
		if (stage.song_step == 504)
			this->character.focus_zoom = FIXED_DEC(873,1024);
		if (stage.song_step == 512)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 632)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 638)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 1120)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 1128)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 1148)
			this->character.focus_zoom = FIXED_DEC(987,1024);
		if (stage.song_step == 1153)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 1400)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 1405)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 1416)
			this->character.focus_zoom = FIXED_DEC(1028,1024);
		if (stage.song_step == 1419)
			this->character.focus_zoom = FIXED_DEC(1068,1024);
		if (stage.song_step == 1425)
			this->character.focus_zoom = FIXED_DEC(814,1024);
	}
	if (stage.stage_id == StageId_Oversight)
	{
		if (stage.song_step == 0)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 384)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 387)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 390)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 392)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 400)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 416)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 419)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 422)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 424)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 432)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 448)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 451)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 454)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 456)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 464)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 480)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 483)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 486)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 488)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 496)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 511)
			this->character.focus_zoom = FIXED_DEC(922,1024);
		if (stage.song_step == 640)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 896)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 899)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 902)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 904)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 912)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 928)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 931)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 934)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 936)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 944)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 960)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 963)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 966)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 968)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 976)
			this->character.focus_zoom = FIXED_DEC(814,1024);
		if (stage.song_step == 992)
			this->character.focus_zoom = FIXED_DEC(865,1024);
		if (stage.song_step == 995)
			this->character.focus_zoom = FIXED_DEC(885,1024);
		if (stage.song_step == 998)
			this->character.focus_zoom = FIXED_DEC(905,1024);
		if (stage.song_step == 1000)
			this->character.focus_zoom = FIXED_DEC(946,1024);
		if (stage.song_step == 1008)
			this->character.focus_zoom = FIXED_DEC(814,1024);
	}
	if (stage.stage_id == StageId_DoubleKill)
	{
		if (stage.song_beat == 356)
		{
			this->character.focus_x = FIXED_DEC(-5,1);
			this->character.focus_y = FIXED_DEC(-54,1);
			this->character.focus_zoom = FIXED_DEC(1493,1024);
		}
		if (stage.song_beat == 420)
		{
			this->character.focus_x = FIXED_DEC(-92,1);
			this->character.focus_y = FIXED_DEC(-84,1);
			this->character.focus_zoom = FIXED_DEC(1086,1024);
		}
		if (stage.song_beat == 552)
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		if (stage.song_beat == 556)
			this->character.focus_zoom = FIXED_DEC(1086,1024);
		if ((stage.song_beat >= 916) && (this->character.focus_zoom != FIXED_DEC(542,1024)))
		{
			zoomout += 1;
			if (zoomout == 2)
			{
				this->character.focus_zoom -= FIXED_DEC(1,1024);
				zoomout = 0;
			}
		}
		if (stage.song_step == 3791)
		{
			this->character.focus_x = FIXED_DEC(-132,1);
			this->character.focus_y = FIXED_DEC(-84,1);
		}
	}
	if (stage.stage_id == StageId_O2)
	{
		if (stage.song_beat == 120)
			this->character.focus_x = FIXED_DEC(69,1);
	}
	if ((stage.stage_id == StageId_VotingTime) && (stage.song_step >= 16))
	{
		if (stage.camswitch == 1) //bf focus
		{
			this->character.focus_x = FIXED_DEC(-45,1);
			this->character.focus_y = FIXED_DEC(-77,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.camswitch == 2) //warchief focus
		{
			this->character.focus_x = FIXED_DEC(-339,1);
			this->character.focus_y = FIXED_DEC(-77,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.camswitch == 3) //redmungus focus
		{
			this->character.focus_x = FIXED_DEC(-74+18,1);
			this->character.focus_y = FIXED_DEC(-83,1);
			this->character.focus_zoom = FIXED_DEC(1696,1024);
		}
		if (stage.camswitch == 4) //jelqer focus
		{
			this->character.focus_x = FIXED_DEC(-313-18,1);
			this->character.focus_y = FIXED_DEC(-83,1);
			this->character.focus_zoom = FIXED_DEC(1696,1024);
		}
		if (stage.camswitch == 5) //zoom out
		{
			this->character.focus_x = FIXED_DEC(-190,1);
			this->character.focus_y = FIXED_DEC(-116,1);
			this->character.focus_zoom = FIXED_DEC(950,1024);
		}
	}
	if (stage.stage_id == StageId_Victory)
	{
		if (stage.song_step == 129)
		{
			this->character.focus_x = FIXED_DEC(-103,1);
			this->character.focus_zoom = FIXED_DEC(950,1024);
		}
	}
	if (stage.stage_id == StageId_LemonLime)
	{
		if (stage.song_step == 120)
			this->character.focus_zoom = FIXED_DEC(1493,1024);
		if (stage.song_step == 128)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 248)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 256)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 270)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 280)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 334)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 344)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 384)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 416)
			this->character.focus_zoom = FIXED_DEC(1493,1024);
		if (stage.song_step == 448)
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		if (stage.song_step == 472)
			this->character.focus_zoom = FIXED_DEC(1493,1024);
		if (stage.song_step == 504)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 508)
			this->character.focus_zoom = FIXED_DEC(1493,1024);
		if (stage.song_step == 512)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 768)
			this->character.focus_zoom = FIXED_DEC(1493,1024);
	}
	if (stage.stage_id == StageId_Chlorophyll)
	{
		if (stage.song_step == 22)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 32)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 54)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 64)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 86)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 96)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 118)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 160)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 184)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 192)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 216)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 224)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 248)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 256)
			this->character.focus_zoom = FIXED_DEC(1493,1024);
		if (stage.song_step == 260)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 272)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 288)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 336)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 352)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 384)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 388)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 496)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 512)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 528)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 544)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 560)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 570)
			this->character.focus_zoom = FIXED_DEC(1153,1024);
		if (stage.song_step == 576)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 656)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 672)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 688)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 698)
			this->character.focus_zoom = FIXED_DEC(1153,1024);
		if (stage.song_step == 704)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 768)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 772)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 896)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 900)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1008)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 1024)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1046)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1056)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1078)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1088)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1110)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1120)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1142)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1152)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1176)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1184)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1208)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1216)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1240)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1248)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1272)
			this->character.focus_zoom = FIXED_DEC(1153,1024);
	}
	if (stage.stage_id == StageId_Inflorescence)
	{
		if (stage.song_step == 128)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 200)
			this->character.focus_zoom = FIXED_DEC(1153,1024);
		if (stage.song_step == 216)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 232)
			this->character.focus_zoom = FIXED_DEC(1153,1024);
		if (stage.song_step == 248)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 256)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 416)
			this->character.focus_zoom = FIXED_DEC(1289,1024);
		if (stage.song_step == 430)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 640)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 896)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 1408)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
	}
	if (stage.stage_id == StageId_Stargazer)
	{
		if (stage.song_step == 128)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 256)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 512)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 516)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 640)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 646)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 768)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1024)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1264)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 1272)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1280)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1408)
			this->character.focus_zoom = FIXED_DEC(1090,1024);
		if (stage.song_step == 1414)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
		if (stage.song_step == 1536)
			this->character.focus_zoom = FIXED_DEC(1357,1024);
		if (stage.song_step == 1600)
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		if (stage.song_step == 1656)
			this->character.focus_zoom = FIXED_DEC(1221,1024);
	}
	
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
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_SussusToogus: //Beep Bap Bo
				if (stage.song_step == 1546)
					character->set_anim(character, PlayerAnim_Peace);
				break;
			case StageId_Dlow: //WHAT THE FUCK
				if (stage.song_step == 1424)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);
	if (stage.lights != 1)
	{
		if (((stage.stage_id != StageId_Defeat) && (stage.stage_id != StageId_DoubleKill) && (stage.stage_id != StageId_Victory)) || ((stage.stage_id == StageId_Defeat) && ((stage.song_step >= 1168) && (stage.song_step <= 1439))))
			Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
		else if ((stage.stage_id == StageId_DoubleKill) && ((stage.song_step <= 3407)))
			Character_DrawCol(character, &this->tex, &char_bf_frame[this->frame], 150, 150, 150);
		else if (stage.stage_id == StageId_Victory)
			Character_DrawCol(character, &this->tex, &char_bf_frame[this->frame], 175, 175, 175);
	}
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	if ((stage.stage_id != StageId_AlphaMoogus) && (stage.stage_id != StageId_ActinSus))
	{
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	}
	else
	{
	this->character.health_i = 10;
	
	//health bar color
	this->character.health_bar = 0xFF5EFF35;
	}
	
	if (((stage.stage_id >= StageId_SussusMoogus) && (stage.stage_id <= StageId_Meltdown)) || (stage.stage_id == StageId_Top10))
	{
	this->character.focus_x = FIXED_DEC(-54,1);
	this->character.focus_y = FIXED_DEC(-84,1);
	this->character.focus_zoom = FIXED_DEC(509,512);
	}
	else if ((stage.stage_id >= StageId_SussusToogus) && (stage.stage_id <= StageId_LightsDown))
	{
	this->character.focus_x = FIXED_DEC(-64,1);
	this->character.focus_y = FIXED_DEC(-77,1);
	this->character.focus_zoom = FIXED_DEC(1228,1024);
	}
	else if ((stage.stage_id >= StageId_Mando) && (stage.stage_id <= StageId_Oversight))
	{
	this->character.focus_x = FIXED_DEC(-96,1);
	this->character.focus_y = FIXED_DEC(-81,1);
	this->character.focus_zoom = FIXED_DEC(814,1024);
	}
	else if (stage.stage_id == StageId_DoubleKill)
	{
	this->character.focus_x = FIXED_DEC(-92,1);
	this->character.focus_y = FIXED_DEC(-84,1);
	this->character.focus_zoom = FIXED_DEC(1086,1024);
	}
	else if (stage.stage_id == StageId_O2)
	{
	this->character.focus_x = FIXED_DEC(-64,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	}
	else if (stage.stage_id == StageId_Victory)
	{
	this->character.focus_x = FIXED_DEC(-127,1);
	this->character.focus_y = FIXED_DEC(-92,1);
	this->character.focus_zoom = FIXED_DEC(1000,1024);
	}
	else if (stage.stage_id == StageId_AlphaMoogus)
	{
	this->character.focus_x = FIXED_DEC(-57,1);
	this->character.focus_y = FIXED_DEC(-76,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	}
	else if (stage.stage_id == StageId_InsaneStreamer)
	{
	this->character.focus_x = FIXED_DEC(-96,1);
	this->character.focus_y = FIXED_DEC(-108,1);
	this->character.focus_zoom = FIXED_DEC(275,256);
	}
	else if (stage.stage_id == StageId_SussusNuzzus)
	{
	this->character.focus_x = FIXED_DEC(-886,1);
	this->character.focus_y = FIXED_DEC(604,1);
	this->character.focus_zoom = FIXED_DEC(324,1024);
	}
	else if (stage.stage_id == StageId_Esculent)
	{
	this->character.focus_x = FIXED_DEC(-710,1);
	this->character.focus_y = FIXED_DEC(77,1);
	this->character.focus_zoom = FIXED_DEC(407,512);
	}
	else if (stage.stage_id == StageId_Crewicide)
	{
	this->character.focus_x = FIXED_DEC(-85,1);
	this->character.focus_y = FIXED_DEC(-95,1);
	this->character.focus_zoom = FIXED_DEC(549,512);
	}
	else if ((stage.stage_id >= StageId_LemonLime) && (stage.stage_id <= StageId_Stargazer))
	{
	this->character.focus_x = FIXED_DEC(-34,1);
	this->character.focus_y = FIXED_DEC(-68,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	}
	else
	{
	this->character.focus_x = FIXED_DEC(-50,1);
	this->character.focus_y = FIXED_DEC(-65,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	}
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BF.ARC;1");
		
	const char **pathp = (const char *[]){
		"bf0.tim",   //BF_ArcMain_BF0
		"bf1.tim",   //BF_ArcMain_BF1
		"bf2.tim",   //BF_ArcMain_BF2
		"bf3.tim",   //BF_ArcMain_BF3
		"bf4.tim",   //BF_ArcMain_BF4
		"bf5.tim",   //BF_ArcMain_BF5
		"bf6.tim",   //BF_ArcMain_BF6
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
