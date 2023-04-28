/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "earthbound.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Earthbound background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_battle, arc_battle_ptr[20];
	
	//Battle state
	Gfx_Tex tex_battle;
	u8 battle_frame, battle_tex_id;
	Animatable battle_animatable;

} Back_Earthbound;

//Battle animation and rects
static const CharFrame battle_frame[] = {
  {0, {  0,  0,124,109}, {160,160}}, //0 battle 1
  {0, {124,  0,124,109}, {160,160}}, //1 battle 2
  {0, {  0,109,124,109}, {160,160}}, //2 battle 3
  {0, {124,109,124,109}, {160,160}}, //3 battle 4
  {1, {  0,  0,124,109}, {160,160}}, //4 battle 5
  {1, {123,  0,124,109}, {160,160}}, //5 battle 6
  {1, {  0,109,124,109}, {160,160}}, //6 battle 7
  {1, {124,109,124,109}, {160,160}}, //7 battle 8
  {2, {  0,  0,124,109}, {160,160}}, //8 battle 9
  {2, {124,  0,124,109}, {160,160}}, //9 battle 10
  {2, {  0,109,124,109}, {160,160}}, //10 battle 11
  {2, {124,109,124,109}, {160,160}}, //11 battle 12
  {3, {  0,  0,124,109}, {160,160}}, //12 battle 13
  {3, {124,  0,124,109}, {160,160}}, //13 battle 14
  {3, {  0,109,124,109}, {160,160}}, //14 battle 15
  {3, {124,109,124,109}, {160,160}}, //15 battle 16
  {4, {  0,  0,124,109}, {160,160}}, //16 battle 17
  {4, {124,  0,124,109}, {160,160}}, //17 battle 18
  {4, {  0,109,124,109}, {160,160}}, //18 battle 19
  {4, {123,109,124,109}, {160,160}}, //19 battle 20
  {5, {  0,  0,124,109}, {160,160}}, //20 battle 21
  {5, {124,  0,124,109}, {160,160}}, //21 battle 22
  {5, {  0,109,124,109}, {160,160}}, //22 battle 23
  {5, {123,109,124,109}, {160,160}}, //23 battle 24
  {6, {  0,  0,124,109}, {160,160}}, //24 battle 25
  {6, {124,  0,124,109}, {160,160}}, //25 battle 26
  {6, {  0,109,124,109}, {160,160}}, //26 battle 27
  {6, {124,109,124,109}, {160,160}}, //27 battle 28
  {7, {  0,  0,124,109}, {160,160}}, //28 battle 29
  {7, {124,  0,124,109}, {160,160}}, //29 battle 30
  {7, {  0,109,124,109}, {160,160}}, //30 battle 31
  {7, {124,109,124,109}, {160,160}}, //31 battle 32
  {8, {  0,  0,124,109}, {160,160}}, //32 battle 33
  {8, {123,  0,124,109}, {160,160}}, //33 battle 34
  {8, {  0,109,124,109}, {160,160}}, //34 battle 35
  {8, {124,109,124,109}, {160,160}}, //35 battle 36
  {9, {  0,  0,124,109}, {160,160}}, //36 battle 37
  {9, {123,  0,124,109}, {160,160}}, //37 battle 38
  {9, {  0,109,124,109}, {160,160}}, //38 battle 39
  {9, {124,109,124,109}, {160,160}}, //39 battle 40
  {10, {  0,  0,124,109}, {160,160}}, //40 battle 41
  {10, {124,  0,124,109}, {160,160}}, //41 battle 42
  {10, {  0,109,124,109}, {160,160}}, //42 battle 43
  {10, {124,109,124,109}, {160,160}}, //43 battle 44
  {11, {  0,  0,124,109}, {160,160}}, //44 battle 45
  {11, {124,  0,124,109}, {160,160}}, //45 battle 46
  {11, {  0,109,124,109}, {160,160}}, //46 battle 47
  {11, {123,109,124,109}, {160,160}}, //47 battle 48
  {12, {  0,  0,124,109}, {160,160}}, //48 battle 49
  {12, {124,  0,124,109}, {160,160}}, //49 battle 50
  {12, {  0,109,124,109}, {160,160}}, //50 battle 51
  {12, {123,109,124,109}, {160,160}}, //51 battle 52
  {13, {  0,  0,124,109}, {160,160}}, //52 battle 53
  {13, {124,  0,124,109}, {160,160}}, //53 battle 54
  {13, {  0,109,124,109}, {160,160}}, //54 battle 55
  {13, {124,109,124,109}, {160,160}}, //55 battle 56
  {14, {  0,  0,124,109}, {160,160}}, //56 battle 57
  {14, {124,  0,124,109}, {160,160}}, //57 battle 58
  {14, {  0,109,124,109}, {160,160}}, //58 battle 59
  {14, {124,109,124,109}, {160,160}}, //59 battle 60
  {15, {  0,  0,124,109}, {160,160}}, //60 battle 61
  {15, {123,  0,124,109}, {160,160}}, //61 battle 62
  {15, {  0,109,124,109}, {160,160}}, //62 battle 63
  {15, {124,109,124,109}, {160,160}}, //63 battle 64
  {16, {  0,  0,124,109}, {160,160}}, //64 battle 65
  {16, {124,  0,124,109}, {160,160}}, //65 battle 66
  {16, {  0,109,124,109}, {160,160}}, //66 battle 67
  {16, {124,109,124,109}, {160,160}}, //67 battle 68
  {17, {  0,  0,124,109}, {160,160}}, //68 battle 69
  {17, {124,  0,124,109}, {160,160}}, //69 battle 70
  {17, {  0,109,124,109}, {160,160}}, //70 battle 71
  {17, {124,109,124,109}, {160,160}}, //71 battle 72
  {18, {  0,  0,124,109}, {160,160}}, //72 battle 73
  {18, {124,  0,124,109}, {160,160}}, //73 battle 74
  {18, {  0,109,124,109}, {160,160}}, //74 battle 75
  {18, {123,109,124,109}, {160,160}}, //75 battle 76
  {19, {  0,  0,124,109}, {160,160}}, //76 battle 77
  {19, {124,  0,124,109}, {160,160}}, //77 battle 78
};

static const Animation battle_anim[] = {
	{3, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, ASCR_CHGANI, 0}}, //Idle
};

//Battle functions
void Earthbound_Battle_SetFrame(void *user, u8 frame)
{
	Back_Earthbound *this = (Back_Earthbound*)user;
	
	//Check if this is a new frame
	if (frame != this->battle_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &battle_frame[this->battle_frame = frame];
		if (cframe->tex != this->battle_tex_id)
			Gfx_LoadTex(&this->tex_battle, this->arc_battle_ptr[this->battle_tex_id = cframe->tex], 0);
	}
}

void Earthbound_Battle_Draw(Back_Earthbound *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &battle_frame[this->battle_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, FIXED_DEC(1280,1), FIXED_DEC(1120,1)};
	Debug_StageMoveDebug(&dst, 5, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_battle, &src, &dst, stage.camera.bzoom);
}

void Back_Earthbound_DrawBG(StageBack *back)
{
	Back_Earthbound *this = (Back_Earthbound*)back;

	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Animate and draw battle
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == -29))
		Animatable_SetAnim(&this->battle_animatable, 0);
	
	Animatable_Animate(&this->battle_animatable, (void*)this, Earthbound_Battle_SetFrame);
	Earthbound_Battle_Draw(this, FIXED_DEC(0+160,1) - fx, FIXED_DEC(0+160,1) - fy);
}

void Back_Earthbound_Free(StageBack *back)
{
	Back_Earthbound *this = (Back_Earthbound*)back;
	
	//Free battle archive
	Mem_Free(this->arc_battle);
}

StageBack *Back_Earthbound_New(void)
{
	//Allocate background structure
	Back_Earthbound *this = (Back_Earthbound*)Mem_Alloc(sizeof(Back_Earthbound));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Earthbound_DrawBG;
	this->back.free = Back_Earthbound_Free;
	
	//Load battle textures
	this->arc_battle = IO_Read("\\BG2\\EARTHBND.ARC;1");
	this->arc_battle_ptr[0] = Archive_Find(this->arc_battle, "battle0.tim");
	this->arc_battle_ptr[1] = Archive_Find(this->arc_battle, "battle1.tim");
	this->arc_battle_ptr[2] = Archive_Find(this->arc_battle, "battle2.tim");
	this->arc_battle_ptr[3] = Archive_Find(this->arc_battle, "battle3.tim");
	this->arc_battle_ptr[4] = Archive_Find(this->arc_battle, "battle4.tim");
	this->arc_battle_ptr[5] = Archive_Find(this->arc_battle, "battle5.tim");
	this->arc_battle_ptr[6] = Archive_Find(this->arc_battle, "battle6.tim");
	this->arc_battle_ptr[7] = Archive_Find(this->arc_battle, "battle7.tim");
	this->arc_battle_ptr[8] = Archive_Find(this->arc_battle, "battle8.tim");
	this->arc_battle_ptr[9] = Archive_Find(this->arc_battle, "battle9.tim");
	this->arc_battle_ptr[10] = Archive_Find(this->arc_battle, "battle10.tim");
	this->arc_battle_ptr[11] = Archive_Find(this->arc_battle, "battle11.tim");
	this->arc_battle_ptr[12] = Archive_Find(this->arc_battle, "battle12.tim");
	this->arc_battle_ptr[13] = Archive_Find(this->arc_battle, "battle13.tim");
	this->arc_battle_ptr[14] = Archive_Find(this->arc_battle, "battle14.tim");
	this->arc_battle_ptr[15] = Archive_Find(this->arc_battle, "battle15.tim");
	this->arc_battle_ptr[16] = Archive_Find(this->arc_battle, "battle16.tim");
	this->arc_battle_ptr[17] = Archive_Find(this->arc_battle, "battle17.tim");
	this->arc_battle_ptr[18] = Archive_Find(this->arc_battle, "battle18.tim");
	this->arc_battle_ptr[19] = Archive_Find(this->arc_battle, "battle19.tim");
	
	//Initialize battle state
	Animatable_Init(&this->battle_animatable, battle_anim);
	Animatable_SetAnim(&this->battle_animatable, 0);
	this->battle_frame = this->battle_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
