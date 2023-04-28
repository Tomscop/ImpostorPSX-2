/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "brown.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Brown character structure
enum
{
  Brown_ArcMain_Idle0,
  Brown_ArcMain_Idle1,
  Brown_ArcMain_Idle2,
  Brown_ArcMain_Idle3,
  Brown_ArcMain_Idle4,
  Brown_ArcMain_Idle5,
  Brown_ArcMain_Left0,
  Brown_ArcMain_Left1,
  Brown_ArcMain_Down0,
  Brown_ArcMain_Down1,
  Brown_ArcMain_Up0,
  Brown_ArcMain_Up1,
  Brown_ArcMain_Up2,
  Brown_ArcMain_Right0,
	
	Brown_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Brown_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Brown;

//Brown character definitions
static const CharFrame char_brown_frame[] = {
  {Brown_ArcMain_Idle0, {  0,  0, 99,108}, {170,118}}, //0 idle 1
  {Brown_ArcMain_Idle0, { 99,  0, 98,107}, {169,118}}, //1 idle 2
  {Brown_ArcMain_Idle0, {  0,108, 99,108}, {170,118}}, //2 idle 3
  {Brown_ArcMain_Idle0, { 99,108, 99,108}, {170,117}}, //3 idle 4
  {Brown_ArcMain_Idle1, {  0,  0, 99,108}, {169,117}}, //4 idle 5
  {Brown_ArcMain_Idle1, { 99,  0, 98,108}, {169,115}}, //5 idle 6
  {Brown_ArcMain_Idle1, {  0,109, 97,109}, {169,114}}, //6 idle 7
  {Brown_ArcMain_Idle1, { 97,109, 97,109}, {169,114}}, //7 idle 8
  {Brown_ArcMain_Idle2, {  0,  0, 96,109}, {168,113}}, //8 idle 9
  {Brown_ArcMain_Idle2, { 96,  0, 96,108}, {168,114}}, //9 idle 10
  {Brown_ArcMain_Idle2, {  0,110, 95,110}, {168,114}}, //10 idle 11
  {Brown_ArcMain_Idle2, { 95,110, 96,109}, {168,114}}, //11 idle 12
  {Brown_ArcMain_Idle3, {  0,  0, 95,109}, {168,116}}, //12 idle 13
  {Brown_ArcMain_Idle3, { 95,  0, 95,109}, {168,117}}, //13 idle 14
  {Brown_ArcMain_Idle3, {  0,109, 95,109}, {168,118}}, //14 idle 15
  {Brown_ArcMain_Idle3, { 95,109, 95,109}, {168,118}}, //15 idle 16
  {Brown_ArcMain_Idle4, {  0,  0, 97,109}, {169,119}}, //16 idle 17
  {Brown_ArcMain_Idle4, { 97,  0, 97,109}, {169,119}}, //17 idle 18
  {Brown_ArcMain_Idle4, {  0,109, 98,108}, {170,119}}, //18 idle 19
  {Brown_ArcMain_Idle4, { 98,109, 98,108}, {170,119}}, //19 idle 20
  {Brown_ArcMain_Idle5, {  0,  0, 99,108}, {170,119}}, //20 idle 21
  {Brown_ArcMain_Idle5, { 99,  0, 99,108}, {170,119}}, //21 idle 22
  {Brown_ArcMain_Idle5, {  0,108, 99,108}, {170,119}}, //22 idle 23

  {Brown_ArcMain_Left0, {  0,  0,128,107}, {200,103}}, //23 left 1
  {Brown_ArcMain_Left0, {  0,107,133,106}, {205,100}}, //24 left 2
  {Brown_ArcMain_Left0, {133,107,121,112}, {190,113}}, //25 left 3
  {Brown_ArcMain_Left1, {  0,  0,117,112}, {187,114}}, //26 left 4
  {Brown_ArcMain_Left1, {117,  0,117,114}, {187,116}}, //27 left 5
  {Brown_ArcMain_Left1, {  0,114,111,113}, {187,116}}, //28 left 6

  {Brown_ArcMain_Down0, {  0,  0,114, 97}, {157, 83}}, //29 down 1
  {Brown_ArcMain_Down0, {114,  0,113, 96}, {156, 81}}, //30 down 2
  {Brown_ArcMain_Down0, {  0, 99,120, 99}, {171, 98}}, //31 down 3
  {Brown_ArcMain_Down0, {120, 99,108,116}, {165,110}}, //32 down 4
  {Brown_ArcMain_Down1, {  0,  0,102,121}, {162,111}}, //33 down 5
  {Brown_ArcMain_Down1, {102,  0, 98,121}, {160,111}}, //34 down 6

  {Brown_ArcMain_Up0, {  0,  0, 93,124}, {169,130}}, //35 up 1
  {Brown_ArcMain_Up0, { 93,  0, 93,125}, {170,134}}, //36 up 2
  {Brown_ArcMain_Up0, {  0,126, 91,126}, {167,127}}, //37 up 3
  {Brown_ArcMain_Up0, { 91,126, 91,125}, {168,122}}, //38 up 4
  {Brown_ArcMain_Up1, {  0,  0, 90,133}, {168,121}}, //39 up 5
  {Brown_ArcMain_Up1, { 90,  0, 91,128}, {168,122}}, //40 up 6
  {Brown_ArcMain_Up2, {  0,  0, 92,123}, {168,122}}, //41 up 7

  {Brown_ArcMain_Right0, {  0,  0,110,100}, {162,115}}, //42 right 1
  {Brown_ArcMain_Right0, {110,  0,109,104}, {159,116}}, //43 right 2
  {Brown_ArcMain_Right0, {  0,104, 90,101}, {149,115}}, //44 right 3
  {Brown_ArcMain_Right0, { 90,104, 76,103}, {143,114}}, //45 right 4
  {Brown_ArcMain_Right0, {166,104, 76,103}, {145,114}}, //46 right 5
};

static const Animation char_brown_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 6, 7, 7, 7, 7, 8, 8, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 15, 15, 15, 16, 17, 17, 18, 18, 18, 19, 20, 20, 21, 22, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 23, 24, 25, 26, 27, 28, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 29, 30, 31, 31, 32, 32, 33, 33, 34, 34, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 41, 41, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 42, 43, 44, 45, 46, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Brown character functions
void Char_Brown_SetFrame(void *user, u8 frame)
{
	Char_Brown *this = (Char_Brown*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_brown_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Brown_Tick(Character *character)
{
	Char_Brown *this = (Char_Brown*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Brown_SetFrame);
	Character_Draw(character, &this->tex, &char_brown_frame[this->frame]);
}

void Char_Brown_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Brown_Free(Character *character)
{
	Char_Brown *this = (Char_Brown*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Brown_New(fixed_t x, fixed_t y)
{
	//Allocate brown object
	Char_Brown *this = Mem_Alloc(sizeof(Char_Brown));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Brown_New] Failed to allocate brown object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Brown_Tick;
	this->character.set_anim = Char_Brown_SetAnim;
	this->character.free = Char_Brown_Free;
	
	Animatable_Init(&this->character.animatable, char_brown_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFF874540;
	
	this->character.focus_x = FIXED_DEC(-253,1);
	this->character.focus_y = FIXED_DEC(-112,1);
	this->character.focus_zoom = FIXED_DEC(407,512);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONET2\\BROWN.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
