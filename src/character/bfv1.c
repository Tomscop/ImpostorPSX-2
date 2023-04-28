/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfv1.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend V1 player types
enum
{
  BFV1_ArcMain_Idle0,
  BFV1_ArcMain_Left0,
  BFV1_ArcMain_Down0,
  BFV1_ArcMain_Up0,
  BFV1_ArcMain_Right0,
  BFV1_ArcMain_LeftM0,
  BFV1_ArcMain_DownM0,
  BFV1_ArcMain_UpM0,
  BFV1_ArcMain_RightM0,
	
	BFV1_ArcMain_Max,
};

#define BFV1_Arc_Max BFV1_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFV1_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFV1;

//Boyfriend V1 player definitions
static const CharFrame char_bfv1_frame[] = {
  {BFV1_ArcMain_Idle0, {  0,  0, 95, 68}, {158,122}}, //0 idle 1
  {BFV1_ArcMain_Idle0, { 95,  0, 96, 69}, {159,122}}, //1 idle 2
  {BFV1_ArcMain_Idle0, {  0, 69, 94, 69}, {158,122}}, //2 idle 3
  {BFV1_ArcMain_Idle0, { 94, 69, 94, 69}, {158,122}}, //3 idle 4
  {BFV1_ArcMain_Idle0, {  0,138, 95, 69}, {158,122}}, //4 idle 5

  {BFV1_ArcMain_Left0, {  0,  0, 97, 69}, {161,121}}, //5 left 1
  {BFV1_ArcMain_Left0, { 97,  0, 94, 69}, {158,121}}, //6 left 2

  {BFV1_ArcMain_Down0, {  0,  0, 91, 68}, {158,119}}, //7 down 1
  {BFV1_ArcMain_Down0, { 91,  0, 90, 69}, {158,120}}, //8 down 2

  {BFV1_ArcMain_Up0, {  0,  0, 83, 79}, {152,128}}, //9 up 1
  {BFV1_ArcMain_Up0, { 83,  0, 84, 78}, {153,127}}, //10 up 2

  {BFV1_ArcMain_Right0, {  0,  0, 84, 69}, {148,121}}, //11 right 1
  {BFV1_ArcMain_Right0, { 84,  0, 84, 69}, {148,121}}, //12 right 2

  {BFV1_ArcMain_LeftM0, {  0,  0, 97,106}, {160,162}}, //13 leftm 1
  {BFV1_ArcMain_LeftM0, { 97,  0, 97,109}, {160,164}}, //14 leftm 2

  {BFV1_ArcMain_DownM0, {  0,  0, 91, 69}, {157,128}}, //15 downm 1
  {BFV1_ArcMain_DownM0, { 91,  0, 90, 97}, {157,155}}, //16 downm 2
  {BFV1_ArcMain_DownM0, {  0, 97, 91, 96}, {157,154}}, //17 downm 3

  {BFV1_ArcMain_UpM0, {  0,  0, 88,108}, {151,164}}, //18 upm 1
  {BFV1_ArcMain_UpM0, { 88,  0, 87,108}, {150,165}}, //19 upm 2

  {BFV1_ArcMain_RightM0, {  0,  0, 85,109}, {149,164}}, //20 rightm 1
  {BFV1_ArcMain_RightM0, { 85,  0, 85,107}, {149,162}}, //21 rightm 2
};

static const Animation char_bfv1_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 5, 13, 13, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 15, 16, 16, 17, 17, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 9, 18, 18, 19, 19, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 11, 20, 20, 21, 21, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend V1 player functions
void Char_BFV1_SetFrame(void *user, u8 frame)
{
	Char_BFV1 *this = (Char_BFV1*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfv1_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFV1_Tick(Character *character)
{
	Char_BFV1 *this = (Char_BFV1*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFV1_SetFrame);
	Character_Draw(character, &this->tex, &char_bfv1_frame[this->frame]);
}

void Char_BFV1_SetAnim(Character *character, u8 anim)
{
	Char_BFV1 *this = (Char_BFV1*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFV1_Free(Character *character)
{
	Char_BFV1 *this = (Char_BFV1*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFV1_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend lava object
	Char_BFV1 *this = Mem_Alloc(sizeof(Char_BFV1));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFV1_New] Failed to allocate boyfriend lava object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFV1_Tick;
	this->character.set_anim = Char_BFV1_SetAnim;
	this->character.free = Char_BFV1_Free;
	
	Animatable_Init(&this->character.animatable, char_bfv1_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 10;
	
	//health bar color
	this->character.health_bar = 0xFF5EFF35;
	
	this->character.focus_x = FIXED_DEC(-141,1);
	this->character.focus_y = FIXED_DEC(-131,1);
	this->character.focus_zoom = FIXED_DEC(1221,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFV1.ARC;1");
		
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
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
