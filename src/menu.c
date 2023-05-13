/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "menu.h"

#include "mem.h"
#include "main.h"
#include "timer.h"
#include "io.h"
#include "gfx.h"
#include "audio.h"
#include "pad.h"
#include "archive.h"
#include "mutil.h"
#include "network.h"

#include "font.h"
#include "trans.h"
#include "loadscr.h"

#include "stage.h"
#include "save.h"
#include "character/gf.h"

#include "stdlib.h"

static u32 Sounds[8];
static char scoredisp[30];
//move stuff
int starmove = 0;
int starfgx = 0;
int starbgx = 20;
int storymove, storymove1, storymove2, storymove3, storymove4, storymove1l, storymove2l, storymove3l, storymove4l = 0;
int storyx = 152;
int storyy = 132;
int ship = 0;
int main1, main2, main3, main4 = 0;
boolean disk = false;
//Menu messages
static const char *funny_messages[][2] = {
	{"TRUST THE", "FUCKING PLAN"},
	{"IM", "ABOUT TO BLOW"},
	{"JANUARY ONETH", "ALWAYS ON TIME"},
	{"SUPER", "DUPER"},
	{"CHUNUGS", "MASTA"},
	{"BREEZY", "MELEE"},
	{"HI", "GUYS"},
	{"MONOTONE", "ATTACK"},
	{"WE LOVE", "CHUNGUSMASTAH"},
	{"VS", "NEWGROUND"},
	{"FPS PLUS", "COMING IN CLUTCH"},
	{"FUCK YOU", "FUCKIN DICK"},
	{"FABS FINALLY GOT", "SLEEP"},
	{"MERRY IMPOSTOR", "AND A HAPPY NEW YEAR"},
	{"CALL ME MONO", "IM ON THE ATTACK"},
	{"MOD HAS BUGS", "FUCK YOU"},
	{"SASTER", "OH MY GOD SASTER"},
	{"GOD FEARS", "TAN IMPOSTOR"},
	{"THE DOC", "IS OUT"},
	{"CALL ME", "ETHAN"},
	{"HAPPY BIRTHDAY", "RED MUNGUS"},
	{"SHOUTOUT TO", "SILLY SQUAD"},
	{"IM HUMBAY", "IN THE MOD"},
	{"MONOTONE ATTACK", "PEAK SONG"},
	{"RESPECT THE TEAM", "WE WORKED HARD"},
	{"DONT TRACE", "PLEASE GUYS"},
	{"IM GOING TO KILL YOU", "IM BEING GENUINE"},
	{"NICKT", "BEAT SUSSUS MOOGUS"},
	{"PLAY VOTING TIME", "DONT LISTEN TO ETHAN"},
	{"IM CVAL", "IM FABS"},
	{"I LEAKED THE MOD", "transfer.sh/urWK1P/impostor.rar"},
	{"DRIPPYPOP OLD", "MY GOAT"},
};

//Menu state
static struct
{
	//Menu state
	u8 page, next_page, freeplayoptions;
	boolean page_swap;
	u8 select, next_select, last_select;
	boolean idk;
	
	fixed_t freeplaypage;
	fixed_t scroll;
	fixed_t trans_time;
	
	//Page specific state
	union
	{
		struct
		{
			u8 funny_message;
		} opening;
		struct
		{
			fixed_t logo_bump;
			fixed_t fade, fadespd;
		} title;
		struct
		{
			fixed_t fade, fadespd;
		} story;
		struct
		{
			fixed_t back_r, back_g, back_b;
		} freeplay;
	} page_state;
	
	union
	{
		struct
		{
			u8 id, diff;
			boolean story;
			boolean last;
		} stage;
	} page_param;
	
	//Menu assets
	Gfx_Tex tex_starbg, tex_starfg, tex_border, tex_ng, tex_story, tex_title, tex_stuff, tex_buttons, tex_disk;
	FontData font_bold, font_arial, font_cdr, font_sus;
	
	Character *gf; //Title Girlfriend
} menu;

//Internal menu functions
char menu_text_buffer[0x100];

static const char *Menu_LowerIf(const char *text, boolean lower)
{
	//Copy text
	char *dstp = menu_text_buffer;
	if (lower)
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'A' && *srcp <= 'Z')
				*dstp++ = *srcp | 0x20;
			else
				*dstp++ = *srcp;
		}
	}
	else
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'a' && *srcp <= 'z')
				*dstp++ = *srcp & ~0x20;
			else
				*dstp++ = *srcp;
		}
	}
	
	//Terminate text
	*dstp++ = '\0';
	return menu_text_buffer;
}

static const char *Menu_LowerIf2(const char *text, boolean lower)
{
	//Copy text
	char *dstp = menu_text_buffer;
	if (lower)
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'A' && *srcp <= 'Z')
				*dstp++ = *srcp;
			else
				*dstp++ = *srcp;
		}
	}
	else
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'a' && *srcp <= 'z')
				*dstp++ = *srcp;
			else
				*dstp++ = *srcp;
		}
	}
	
	//Terminate text
	*dstp++ = '\0';
	return menu_text_buffer;
}

static void Menu_DrawBack()
{
	RECT starfg_src = {0, 0, 255, 132};
	RECT starbg_src = {0, 0, 231, 129};
	RECT starfg_dst = {starfgx, 0, 462, 239};
	RECT starbg_dst = {starbgx, 3, 419, 233};
	RECT starfg2_dst = {starfgx + 470, 0, 462, 239};
	RECT starbg2_dst = {starbgx + 425, 3, 419, 233};
	Gfx_DrawTex(&menu.tex_starfg, &starfg_src, &starfg_dst);
	Gfx_DrawTex(&menu.tex_starbg, &starbg_src, &starbg_dst);
	Gfx_DrawTex(&menu.tex_starfg, &starfg_src, &starfg2_dst);
	Gfx_DrawTex(&menu.tex_starbg, &starbg_src, &starbg2_dst);
	
	Gfx_SetClear(0, 0, 0);;
	
	starmove += 1;
	if (starmove == 7)
		starfgx -= 1;
	if (starmove == 14)
	{
		starfgx -= 1;
		starbgx -= 1;
	}
	if (starfgx <= -470)
		starfgx = 0;
	if (starbgx <= -405)
		starbgx = 20;
	if (starmove == 14)
		starmove = 0;
}

static int increase_Story(int length, int thesong)
{
	int result = 0;
	int testresult = 0;

	for (int i = 0; i < length; i++)
	{
		testresult = stage.prefs.savescore[thesong + i][menu.page_param.stage.diff];

		if (testresult == 0)
			return 0;

		result += stage.prefs.savescore[thesong + i][menu.page_param.stage.diff];
	}

	return result * 10;
}

static void Menu_DrawWeek(const char *week, s32 y)
{
	//Draw label
	if (week == NULL)
	{
		return;
	}
	else if (week == "11")
	{
		//Tomongus
		RECT label_src = {0, 0, 98, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, 160 - (98 / 2), y);
	}
	else if (week == "12")
	{
		//Boo!
		RECT label_src = {102, 0, 36, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, 160 - (36 / 2), y);
	}
	else if (week == "13")
	{
		//Henry
		RECT label_src = {46, 32, 46, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, 160 - (46 / 2), y);
	}
	else if (week == "10")
	{
		//Defeat
		RECT label_src = {142, 0, 22, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, 160 - (22 / 2), y);
	}
	else
	{
		//Week
		RECT label_src = {0, 32, 42, 32};
		Gfx_BlitTex(&menu.tex_story, &label_src, 160 - (66 / 2), y);
		
		//Number
		for (; *week != '\0'; week++)
		{
			//Draw number
			u8 i = *week - '0';
			
			RECT num_src = {(174 + (i * 12)) - 12, 0, 10, 32};
			Gfx_BlitTex(&menu.tex_story, &num_src, (160 - (66 / 2)) + 56, y);
		}
	}
}

static void Menu_DrawShip(void)
{
	//Draw ship
	if (ship == 0) //Right
	{
		RECT ship_src = {  0, 84, 52, 38};
		RECT ship_dst = {126,121, 52, 38};
		Gfx_DrawTex(&menu.tex_stuff, &ship_src, &ship_dst);
	}
	if (ship == 1) //Left
	{
		RECT ship_src = { 42,140, 52, 38};
		RECT ship_dst = {142,121, 52, 38};
		Gfx_DrawTex(&menu.tex_stuff, &ship_src, &ship_dst);
	}
	if (ship == 2) //Up
	{
		RECT ship_src = { 56, 84, 38, 52};
		RECT ship_dst = {141,122, 38, 52};
		Gfx_DrawTex(&menu.tex_stuff, &ship_src, &ship_dst);
	}
	if (ship == 3) //Down
	{
		RECT ship_src = {  0,126, 38, 52};
		RECT ship_dst = {141,106, 38, 52};
		Gfx_DrawTex(&menu.tex_stuff, &ship_src, &ship_dst);
	}
}

static void Menu_DrawStory(void)
{
	//Draw circles
	RECT circle_src = { 95,167, 16, 16};
	RECT circle0_dst = {storyx + 0,storyy + 0, 16, 16};
	RECT circle1_dst = {storyx + 98,storyy + 0, 16, 16};
	RECT circle2_dst = {storyx + 196,storyy + 0, 16, 16};
	RECT circle3_dst = {storyx + 294,storyy + 0, 16, 16};
	RECT circle4_dst = {storyx + 392,storyy + 0, 16, 16};
	RECT circle5_dst = {storyx + 0,storyy + 98, 16, 16};
	RECT circle6_dst = {storyx + -98,storyy + 98, 16, 16};
	RECT circle7_dst = {storyx + -196,storyy + 98, 16, 16};
	RECT circle8_dst = {storyx + 0,storyy + -98, 16, 16};
	RECT circle9_dst = {storyx + 196,storyy + -98, 16, 16};
	RECT circle10_dst = {storyx + 196,storyy + 98, 16, 16};
	RECT circle11_dst = {storyx + 294,storyy + 98, 16, 16};
	
	//Draw horizontal lines
	RECT horiline_src = {  0,179, 82,  4};
	RECT horiline0_dst = {storyx + 16,storyy + 6, 82,  4};
	RECT horiline1_dst = {storyx + 114,storyy + 6, 82,  4};
	RECT horiline2_dst = {storyx + 212,storyy + 6, 82,  4};
	RECT horiline3_dst = {storyx + 310,storyy + 6, 82,  4};
	RECT horiline4_dst = {storyx + -82,storyy + 104, 82,  4};
	RECT horiline5_dst = {storyx + -180,storyy + 104, 82,  4};
	
	//Draw vertical lines
	RECT vertline_src = { 97, 76,  4, 82};
	RECT vertline0_dst = {storyx + 6,storyy + -82,  4, 82};
	RECT vertline1_dst = {storyx + 6,storyy + 16,  4, 82};
	RECT vertline2_dst = {storyx + 202,storyy + -82,  4, 82};
	RECT vertline3_dst = {storyx + 202,storyy + 16,  4, 82};
	RECT vertline4_dst = {storyx + 300,storyy + 16,  4, 82};
	
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle0_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle1_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle2_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle3_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle4_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle5_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle6_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle7_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle8_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle9_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle10_dst);
	Gfx_DrawTex(&menu.tex_stuff, &circle_src, &circle11_dst);
	
	Gfx_DrawTex(&menu.tex_stuff, &horiline_src, &horiline0_dst);
	Gfx_DrawTex(&menu.tex_stuff, &horiline_src, &horiline1_dst);
	Gfx_DrawTex(&menu.tex_stuff, &horiline_src, &horiline2_dst);
	Gfx_DrawTex(&menu.tex_stuff, &horiline_src, &horiline3_dst);
	Gfx_DrawTex(&menu.tex_stuff, &horiline_src, &horiline4_dst);
	Gfx_DrawTex(&menu.tex_stuff, &horiline_src, &horiline5_dst);
	
	Gfx_DrawTex(&menu.tex_stuff, &vertline_src, &vertline0_dst);
	Gfx_DrawTex(&menu.tex_stuff, &vertline_src, &vertline1_dst);
	Gfx_DrawTex(&menu.tex_stuff, &vertline_src, &vertline2_dst);
	Gfx_DrawTex(&menu.tex_stuff, &vertline_src, &vertline3_dst);
	Gfx_DrawTex(&menu.tex_stuff, &vertline_src, &vertline4_dst);
}

static void Menu_DrawDefeat(void)
{
	//Draw number
	RECT zero_src = {0, 67, 10, 15};
	RECT zero_dst = {5 + 15, 42, 16, 24};
	RECT one_src = {11, 67, 8, 15};
	RECT one_dst = {5 + 15, 42, 13, 24};
	RECT two_src = {20, 67, 10, 15};
	RECT two_dst = {5 + 15, 42, 16, 24};
	RECT three_src = {31, 67, 10, 15};
	RECT three_dst = {5 + 15, 42, 16, 24};
	RECT four_src = {43, 67, 10, 15};
	RECT four_dst = {5 + 15, 42, 16, 24};
	RECT five_src = {54, 67, 10, 15};
	RECT five_dst = {5 + 15, 42, 16, 24};
	if (menu.select == 5)
		Gfx_DrawTex(&menu.tex_stuff, &zero_src, &zero_dst);
	else if (menu.select == 4)
		Gfx_DrawTex(&menu.tex_stuff, &one_src, &one_dst);
	else if (menu.select == 3)
		Gfx_DrawTex(&menu.tex_stuff, &two_src, &two_dst);
	else if (menu.select == 2)
		Gfx_DrawTex(&menu.tex_stuff, &three_src, &three_dst);
	else if (menu.select == 1)
		Gfx_DrawTex(&menu.tex_stuff, &four_src, &four_dst);
	else if (menu.select == 0)
		Gfx_DrawTex(&menu.tex_stuff, &five_src, &five_dst);
	
	//Draw text
	RECT combo_src = {0, 35, 55, 15};
	RECT combo_dst = {76 + 15, 42, 88, 24};
	RECT break_src = {0, 51, 66, 15};
	RECT break_dst = {183 + 15, 42, 107, 24};
	RECT slash_src = {65, 67, 10, 15};
	RECT slash_dst = {23 + 15, 42, 16, 24};
	RECT five2_dst = {41 + 15, 42, 16, 24};
	Gfx_DrawTex(&menu.tex_stuff, &combo_src, &combo_dst);
	Gfx_DrawTex(&menu.tex_stuff, &break_src, &break_dst);
	Gfx_DrawTex(&menu.tex_stuff, &slash_src, &slash_dst);
	Gfx_DrawTex(&menu.tex_stuff, &five_src, &five2_dst);
	
	//Draw dead people
	RECT ghost_src = {0, 0, 28, 30};
	RECT ghost_dst = {0 + 41, 150, 28, 30};
	RECT ghost2_dst = {42 + 41, 150, 28, 30};
	RECT dead_src = {29, 0, 28, 30};
	RECT dead_dst = {84 + 41, 150, 28, 30};
	RECT dead2_dst = {126 + 41, 150, 28, 30};
	RECT bone_src = {58, 0, 28, 30};
	RECT bone_dst = {168 + 41, 150, 28, 30};
	RECT bone2_dst = {210 + 41, 150, 28, 30};
	Gfx_DrawTex(&menu.tex_stuff, &ghost_src, &ghost_dst);
	Gfx_DrawTex(&menu.tex_stuff, &ghost_src, &ghost2_dst);
	Gfx_DrawTex(&menu.tex_stuff, &dead_src, &dead_dst);
	Gfx_DrawTex(&menu.tex_stuff, &dead_src, &dead2_dst);
	Gfx_DrawTex(&menu.tex_stuff, &bone_src, &bone_dst);
	Gfx_DrawTex(&menu.tex_stuff, &bone_src, &bone2_dst);
	
	//Draw arrow
	RECT arrow_src = {0, 31, 28, 3};
	RECT arrow_dst = {(42 * menu.select) + 41,138, 28, 3};
	if (menu.select == 0)
		Gfx_DrawTex(&menu.tex_stuff, &arrow_src, &arrow_dst);
	else if (menu.select == 1)
		Gfx_DrawTex(&menu.tex_stuff, &arrow_src, &arrow_dst);
	else if (menu.select == 2)
		Gfx_DrawTex(&menu.tex_stuff, &arrow_src, &arrow_dst);
	else if (menu.select == 3)
		Gfx_DrawTex(&menu.tex_stuff, &arrow_src, &arrow_dst);
	else if (menu.select == 4)
		Gfx_DrawTex(&menu.tex_stuff, &arrow_src, &arrow_dst);
	else if (menu.select == 5)
		Gfx_DrawTex(&menu.tex_stuff, &arrow_src, &arrow_dst);
}

void diskchange()
{
	if (disk == true)
	{
		//Draw text
		menu.font_sus.draw(&menu.font_sus,
			"Please change disk",
			72+66+5,
			84+21,
			FontAlign_Left
		);
		if (menu.page == MenuPage_Freeplay)
		{
		menu.font_sus.draw(&menu.font_sus,
			"to play this song",
			72+66+4,
			84+21+14,
			FontAlign_Left
		);
		}
		else if (menu.page == MenuPage_Story)
		{
		menu.font_sus.draw(&menu.font_sus,
			"to play this week",
			72+66+4,
			84+21+14,
			FontAlign_Left
		);
		}
		
		RECT disk_src = {  0,  0,176, 72};
		RECT disk_dst = { 72, 84,176, 72};
		Gfx_DrawTex(&menu.tex_disk, &disk_src, &disk_dst);
	}
}

void idk()
{
	if (menu.select == 0)
	{
		storyx = 152;
		storyy = 132;
	}
	if (menu.select == 1)
	{
		storyx = 54;
		storyy = 132;
	}
	if (menu.select == 2)
	{
		storyx = -44;
		storyy = 132;
	}
	if (menu.select == 3)
	{
		storyx = -142;
		storyy = 132;
	}
	if (menu.select == 4)
	{
		storyx = -240;
		storyy = 132;
	}
	if (menu.select == 5)
	{
		storyx = 152;
		storyy = 34;
	}
	if (menu.select == 6)
	{
		storyx = 250;
		storyy = 34;
	}
	if (menu.select == 7)
	{
		storyx = 348;
		storyy = 34;
	}
	if (menu.select == 8)
	{
		storyx = 152;
		storyy = 230;
	}
	if (menu.select == 9)
	{
		storyx = -44;
		storyy = 34;
	}
	if (menu.select == 10)
	{
		storyx = -44;
		storyy = 230;
	}
	if (menu.select == 11)
	{
		storyx = -142;
		storyy = 34;
	}
}
//Menu functions
void Menu_Load(MenuPage page)
{
	stage.stage_id = StageId_Temp;
	//Load menu assets
	IO_Data menu_arc = IO_Read("\\MENU\\MENU.ARC;1");
	Gfx_LoadTex(&menu.tex_starbg,  Archive_Find(menu_arc, "starbg.tim"),  0);
	Gfx_LoadTex(&menu.tex_starfg,  Archive_Find(menu_arc, "starfg.tim"),  0);
	Gfx_LoadTex(&menu.tex_border,  Archive_Find(menu_arc, "border.tim"),  0);
	Gfx_LoadTex(&menu.tex_ng,    Archive_Find(menu_arc, "ng.tim"),    0);
	Gfx_LoadTex(&menu.tex_story, Archive_Find(menu_arc, "story.tim"), 0);
	Gfx_LoadTex(&menu.tex_title, Archive_Find(menu_arc, "title.tim"), 0);
	Gfx_LoadTex(&menu.tex_stuff, Archive_Find(menu_arc, "stuff.tim"), 0);
	Gfx_LoadTex(&menu.tex_buttons, Archive_Find(menu_arc, "buttons.tim"), 0);
	Gfx_LoadTex(&menu.tex_disk, Archive_Find(menu_arc, "disk.tim"), 0);
	Mem_Free(menu_arc);
	
	FontData_Load(&menu.font_bold, Font_Bold, false);
	FontData_Load(&menu.font_arial, Font_Arial, false);
	FontData_Load(&menu.font_cdr, Font_CDR, false);
	FontData_Load(&menu.font_sus, Font_Sus, false);
	
	menu.gf = Char_GF_New(FIXED_DEC(62,1), FIXED_DEC(-12,1));
	stage.camera.x = stage.camera.y = FIXED_DEC(0,1);
	stage.camera.bzoom = FIXED_UNIT;
	stage.gf_speed = 4;
	
	//Initialize menu state
	menu.select = menu.next_select = 0;
	
	switch (menu.page = menu.next_page = page)
	{
		case MenuPage_Opening:
			//Get funny message to use
			//Do this here so timing is less reliant on VSync
			menu.page_state.opening.funny_message = ((*((volatile u32*)0xBF801120)) >> 3) % COUNT_OF(funny_messages); //sysclk seeding
			break;
		default:
			break;
	}
	menu.page_swap = true;
	
	menu.trans_time = 0;
	Trans_Clear();
	
	stage.song_step = 0;

	// to load
	CdlFILE file;
    IO_FindFile(&file, "\\SOUNDS\\SCROLL.VAG;1");
    u32 *data = IO_ReadFile(&file);
    Sounds[0] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	IO_FindFile(&file, "\\SOUNDS\\CONFIRM.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[1] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	IO_FindFile(&file, "\\SOUNDS\\CANCEL.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[2] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
	
	IO_FindFile(&file, "\\SOUNDS\\SCROLLFP.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[3] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
	
	IO_FindFile(&file, "\\SOUNDS\\CONFIRMF.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[4] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
	
	IO_FindFile(&file, "\\SOUNDS\\SWAP0.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[5] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
	
	IO_FindFile(&file, "\\SOUNDS\\SWAP1.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[6] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);
	
	IO_FindFile(&file, "\\SOUNDS\\DEFEAT.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[7] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	//Play menu music
	Audio_PlayXA_Track(XA_GettinFreaky, 0x40, 0, 1);
	Audio_WaitPlayXA();
	
	//Set background colour
	Gfx_SetClear(0, 0, 0);
}

void Menu_Unload(void)
{
	//Free title Girlfriend
	Character_Free(menu.gf);
}

void Menu_ToStage(StageId id, StageDiff diff, boolean story)
{
	menu.next_page = MenuPage_Stage;
	menu.page_param.stage.id = id;
	menu.page_param.stage.story = story;
	menu.page_param.stage.diff = diff;
	Trans_Start();
}

void Menu_Tick(void)
{
	//Clear per-frame flags
	stage.flag &= ~STAGE_FLAG_JUST_STEP;
	
	//Get song position
	u16 next_step = Audio_TellXA_Milli() / 147; //100 BPM
	if (next_step != stage.song_step)
	{
		if (next_step >= stage.song_step)
			stage.flag |= STAGE_FLAG_JUST_STEP;
		stage.song_step = next_step;
	}
	
	//Handle transition out
	if (Trans_Tick())
	{
		//Change to set next page
		menu.page_swap = true;
		menu.page = menu.next_page;
		menu.select = menu.next_select;
	}
	
	//Tick menu page
	MenuPage exec_page;
	switch (exec_page = menu.page)
	{
		case MenuPage_Opening:
		{
			u16 beat = stage.song_step >> 2;
			
			//Start title screen if opening ended
			if (beat >= 16)
			{
				menu.page = menu.next_page = MenuPage_Title;
				menu.page_swap = true;
				//Fallthrough
			}
			else
			{
				//Start title screen if start pressed
				if (pad_state.held & PAD_START)
					menu.page = menu.next_page = MenuPage_Title;
				
				//Draw different text depending on beat
				RECT src_ng = {0, 0, 128, 128};
				const char **funny_message = funny_messages[menu.page_state.opening.funny_message];
				
				switch (beat)
				{
					case 3:
						menu.font_bold.draw(&menu.font_bold, "PRESENTS", screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2, FontAlign_Center);
				//Fallthrough
					case 2:
					case 1:
						menu.font_bold.draw(&menu.font_bold, "IMPOSTORM", screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;
					
					case 7:
						menu.font_bold.draw(&menu.font_bold, "HEY LOOK ITS NEWGROUNDS",    screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 42, FontAlign_Center);
						Gfx_BlitTex(&menu.tex_ng, &src_ng, (screen.SCREEN_WIDTH - 128) >> 1, screen.SCREEN_HEIGHT2 - 26);
						menu.font_bold.draw(&menu.font_bold, "IM TOO LAZY TO REMOVE IT",    screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 + 102, FontAlign_Center);
				//Fallthrough
					case 6:
					case 5:
						menu.font_bold.draw(&menu.font_bold, "THIS IS A MOD TO", screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 74, FontAlign_Center);
						menu.font_bold.draw(&menu.font_bold, "PSXFUNKIN LMAO",           screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 58, FontAlign_Center);
						break;
					
					case 11:
						menu.font_bold.draw(&menu.font_bold, funny_message[1], screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2, FontAlign_Center);
				//Fallthrough
					case 10:
					case 9:
						menu.font_bold.draw(&menu.font_bold, funny_message[0], screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;
					
					case 15:
						menu.font_bold.draw(&menu.font_bold, "FUNKIN", screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 + 8, FontAlign_Center);
				//Fallthrough
					case 14:
						menu.font_bold.draw(&menu.font_bold, "NIGHT", screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 8, FontAlign_Center);
				//Fallthrough
					case 13:
						menu.font_bold.draw(&menu.font_bold, "FRIDAY", screen.SCREEN_WIDTH2, screen.SCREEN_HEIGHT2 - 24, FontAlign_Center);
						break;
				}
				break;
			}
		}
	//Fallthrough
		case MenuPage_Title:
		{
			//Initialize page
			if (menu.page_swap)
			{
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(90,1);
			}
			
			//Draw white fade
			if (stage.prefs.flash != 0)
			{
			if (menu.page_state.title.fade > 0)
			{
				RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
				u8 flash_col = menu.page_state.title.fade >> FIXED_SHIFT;
				Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 1);
				menu.page_state.title.fade -= FIXED_MUL(menu.page_state.title.fadespd, timer_dt);
			}
			}
			
			//Go to main menu when start is pressed
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if ((pad_state.press & PAD_START) && menu.next_page == menu.page && Trans_Idle())
			{
				//play confirm sound
				Audio_PlaySound(Sounds[1], 0x3fff);
				menu.trans_time = FIXED_UNIT;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(300,1);
				menu.next_page = MenuPage_Main;
				menu.next_select = 0;
			}
			
			//Draw "Press Start to Begin"
			RECT press_src = {0, 165, 256, 32};
			Gfx_BlitTex(&menu.tex_title, &press_src, (screen.SCREEN_WIDTH - 256) / 2, screen.SCREEN_HEIGHT - 33);
			
			//Draw Friday Night Funkin' logo
			if ((stage.flag & STAGE_FLAG_JUST_STEP) && (stage.song_step & 0x3) == 0 && menu.page_state.title.logo_bump == 0)
				menu.page_state.title.logo_bump = (FIXED_DEC(6,1) / 24) - 1;
			
			static const fixed_t logo_scales[] = {
				FIXED_DEC(13,10),
				FIXED_DEC(131,100),
				FIXED_DEC(132,100),
				FIXED_DEC(133,100),
				FIXED_DEC(135,100),
				FIXED_DEC(136,100),
			};
			fixed_t logo_scale = logo_scales[(menu.page_state.title.logo_bump * 24) >> FIXED_SHIFT];
			u32 x_rad = (logo_scale * (191 >> 1)) >> FIXED_SHIFT;
			u32 y_rad = (logo_scale * (164 >> 1)) >> FIXED_SHIFT;
			
			RECT logo_src = {  0,  0,191,164};
			RECT logo_dst = {
				160 - x_rad + (screen.SCREEN_WIDEADD2 >> 1),
				110 - y_rad,
				x_rad << 1,
				y_rad << 1
			};
			Gfx_DrawTex(&menu.tex_title, &logo_src, &logo_dst);
			
			if (menu.page_state.title.logo_bump > 0)
				if ((menu.page_state.title.logo_bump -= timer_dt) < 0)
					menu.page_state.title.logo_bump = 0;
				
			//Draw background
			Menu_DrawBack();
			break;
		}
		case MenuPage_Main:
		{
			static const char *menu_options[] = {
				"STORY MODE",
				"FREEPLAY",
				"OPTIONS",
				"CREDITS",
			};
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.last_select = 0;
				menu.freeplaypage = 0;
				storymove = 0;
				storyx = 152;
				storyy = 132;
				menu.scroll = menu.select * FIXED_DEC(12,1);
				menu.page_state.title.fade = FIXED_DEC(0,1);
				menu.page_state.title.fadespd = FIXED_DEC(0,1);
			}
			
			//Draw white fade
			if (stage.prefs.flash != 0)
			{
			if (menu.page_state.title.fade > 0)
			{
				RECT flash2 = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
				u8 flash_col = menu.page_state.title.fade >> FIXED_SHIFT;
				Gfx_BlendRect(&flash2, flash_col, flash_col, flash_col, 1);
				menu.page_state.title.fade -= FIXED_MUL(menu.page_state.title.fadespd, timer_dt);
			}
			}
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select == 0)
						menu.select = 2;
					else if (menu.select == 1)
						menu.select = 3;
					else if (menu.select == 2)
						menu.select = 0;
					else if (menu.select == 3)
						menu.select = 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select == 0)
						menu.select = 2;
					else if (menu.select == 1)
						menu.select = 3;
					else if (menu.select == 2)
						menu.select = 0;
					else if (menu.select == 3)
						menu.select = 1;
				}
				if (pad_state.press & PAD_LEFT)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select == 0)
						menu.select = 1;
					else if (menu.select == 1)
						menu.select = 0;
					else if (menu.select == 2)
						menu.select = 3;
					else if (menu.select == 3)
						menu.select = 2;
				}
				if (pad_state.press & PAD_RIGHT)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select == 0)
						menu.select = 1;
					else if (menu.select == 1)
						menu.select = 0;
					else if (menu.select == 2)
						menu.select = 3;
					else if (menu.select == 3)
						menu.select = 2;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//play confirm sound
					Audio_PlaySound(Sounds[1], 0x3fff);
					switch (menu.select)
					{
						case 0: //Story Mode
							menu.next_page = MenuPage_Story;
							break;
						case 1: //Freeplay
							menu.next_page = MenuPage_Freeplay;
							break;
						case 2: //Options
							menu.next_page = MenuPage_Options;
							break;
						case 3: //Credits
							menu.next_page = MenuPage_Credits;
							break;
					}
					menu.next_select = 0;
					menu.trans_time = FIXED_UNIT;
					menu.page_state.title.fade = FIXED_DEC(255,1);
					menu.page_state.title.fadespd = FIXED_DEC(510,1);
				}
				
				//Return to title screen if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Title;
					Trans_Start();
				}
			}
			
			menu.font_arial.draw(&menu.font_arial,
				"2",
				285,
				204,
				FontAlign_Left
			);
			
			//Draw disk thing
			RECT diskn_src = {  0, 73, 36, 36};
			RECT diskn_dst = {284,204, 72, 72};
			Gfx_DrawTex(&menu.tex_disk, &diskn_src, &diskn_dst);
			
			//Draw logo
			RECT logo_src = {  0,  0,191,164};
			RECT logo_dst = {
				74,
				2,
				172,
				148
			};
			Gfx_DrawTex(&menu.tex_title, &logo_src, &logo_dst);
			
			//Draw menu options
			RECT story_src = {main1, 85, 88, 36};
			RECT story_dst = {71,155, 88, 36};
			RECT freeplay_src = {main2, 24, 88, 36};
			RECT freeplay_dst = {71+91,155, 88, 36};
			RECT options_src = {main3, 61, 88, 23};
			RECT options_dst = {71,155+40, 88, 23};
			RECT credits_src = {main4,  0, 88, 23};
			RECT credits_dst = {71+91,155+40, 88, 23};
			if (menu.select == 0)
				main1 = 89;
			else
				main1 = 0;
			if (menu.select == 1)
				main2 = 89;
			else
				main2 = 0;
			if (menu.select == 2)
				main3 = 89;
			else
				main3 = 0;
			if (menu.select == 3)
				main4 = 89;
			else
				main4 = 0;
			Gfx_DrawTex(&menu.tex_buttons, &story_src, &story_dst);
			Gfx_DrawTex(&menu.tex_buttons, &freeplay_src, &freeplay_dst);
			Gfx_DrawTex(&menu.tex_buttons, &options_src, &options_dst);
			Gfx_DrawTex(&menu.tex_buttons, &credits_src, &credits_dst);
			
			//Draw background
			Menu_DrawBack();
			break;
		}
		case MenuPage_Story:
		{
			static const struct
			{
				const char *week;
				StageId stage;
				const char *name;
				const char *tracks[5];
				int length;
			} menu_options[] = {
				{NULL, StageId_Temp, NULL, { NULL, NULL, NULL, NULL, NULL}, 3},
				{"1", StageId_SussusMoogus,">POLUS PROBLEMS", {"SUSSUS MOOGUS","SABOTAGE","MELTDOWN", NULL, NULL}, 3},
				{"2", StageId_SussusToogus,"MIRA MANIA<", {"SUSSUS TOOGUS","LIGHTS DOWN","REACTOR","EJECTED", NULL}, 4},
				{"3", StageId_Mando,">AIRSHIP ATROCITIES", {"MANDO","DLOW","OVERSIGHT","DANGER","DOUBLE KILL"}, 5},
				{"10", StageId_Defeat,">DEFEAT", {NULL, NULL, NULL, NULL, NULL}, 1},
				{"5", StageId_Ashes,"MAGMATIC MONSTROSITY<<<<", {"ASHES","MAGMATIC","BOILING POINT", NULL, NULL}, 3},
				{"6", StageId_Delusion,"DEADLY DELUSION", {"DELUSION","BLACKOUT","NEUROTIC", NULL, NULL}, 3},
				{"7", StageId_Heartbeat,"HUMANE HEARTBEAT<", {"HEARTBEAT","PINKWAVE","PRETENDER", NULL, NULL}, 3},
				{"4", StageId_O2,"JORSAWSEES JAMS<", {"O2","VOTING TIME","TURBULENCE","VICTORY", NULL}, 4},
				{"11", StageId_SussyBussy,"ROUSEY RIVAL", {"SUSSY BUSSY","RIVALS","CHEWMATE", NULL, NULL}, 3},
				{"12", StageId_Christmas,"LOGGOS HALLOWEEN<", {"CHRISTMAS","SPOOKPOSTOR", NULL, NULL, NULL}, 2},
				{"13", StageId_Titular,"BATTLING THE BOYFRIEND", {"TITULAR","GREATEST PLAN","REINFORCEMENTS","ARMED", NULL}, 4},
			};
			
			diskchange();
			
			//Draw score
			menu.font_arial.draw(&menu.font_arial,
				scoredisp,
				26,
				22,
				FontAlign_Left
			);
			
			if (menu.select != 0)
				sprintf(scoredisp, "HIGH SCORE: %d", increase_Story(menu_options[menu.select].length, menu_options[menu.select].stage));
			else
				sprintf(scoredisp, "", increase_Story(menu_options[menu.select].length, menu_options[menu.select].stage));
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.select = menu.last_select;
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.title.fade = FIXED_DEC(0,1);
				menu.page_state.title.fadespd = FIXED_DEC(0,1);
				menu.page_param.stage.last = true;
			}
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if ((menu.select == 0) || (menu.select == 2) || (menu.select == 5) || (menu.select == 9) || (menu.select == 11))
					{
						Audio_PlaySound(Sounds[0], 0x3fff);
						storymove1 = 1;
						ship = 2;
						storymove1l += 1;
					}
					disk = false;
					if (menu.select == 0)
						menu.select = 8;
					else if (menu.select == 2)
						menu.select = 10;
					else if (menu.select == 5)
						menu.select = 0;
					else if (menu.select == 9)
						menu.select = 2;
					else if (menu.select == 11)
						menu.select = 3;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if ((menu.select == 0) || (menu.select == 2) || (menu.select == 3) || (menu.select == 8) || (menu.select == 10))
					{
						Audio_PlaySound(Sounds[0], 0x3fff);
						storymove2 = 1;
						ship = 3;
						storymove2l += 1;
					}
					disk = false;
					if (menu.select == 0)
						menu.select = 5;
					else if (menu.select == 2)
						menu.select = 9;
					else if (menu.select == 3)
						menu.select = 11;
					else if (menu.select == 8)
						menu.select = 0;
					else if (menu.select == 10)
						menu.select = 2;
				}
				if (pad_state.press & PAD_LEFT)
				{
					if ((menu.select == 1) || (menu.select == 2) || (menu.select == 3) || (menu.select == 4) || (menu.select == 5) || (menu.select == 6))
					{
						Audio_PlaySound(Sounds[0], 0x3fff);
						storymove3 = 1;
						ship = 1;
						storymove3l += 1;
					}
					disk = false;
					if (menu.select == 1)
						menu.select = 0;
					else if (menu.select == 2)
						menu.select = 1;
					else if (menu.select == 3)
						menu.select = 2;
					else if (menu.select == 4)
						menu.select = 3;
					else if (menu.select == 5)
						menu.select = 6;
					else if (menu.select == 6)
						menu.select = 7;
				}
				if (pad_state.press & PAD_RIGHT)
				{
					if ((menu.select == 0) || (menu.select == 1) || (menu.select == 2) || (menu.select == 3) || (menu.select == 6) || (menu.select == 7))
					{
						Audio_PlaySound(Sounds[0], 0x3fff);
						storymove4 = 1;
						ship = 0;
						storymove4l += 1;
					}
					disk = false;
					if (menu.select == 0)
						menu.select = 1;
					else if (menu.select == 1)
						menu.select = 2;
					else if (menu.select == 2)
						menu.select = 3;
					else if (menu.select == 3)
						menu.select = 4;
					else if (menu.select == 6)
						menu.select = 5;
					else if (menu.select == 7)
						menu.select = 6;
				}
				
				if (storymove1 == 1)
				{
					if (storymove <= ((98 * storymove1l)-1))
					{
						storymove += 7;
						storyy += 7;
					}
				}
				else if (storymove2 == 1)
				{
					if (storymove <= ((98 * storymove2l)-1))
					{
						storymove += 7;
						storyy -= 7;
					}
				}
				else if (storymove3 == 1)
				{
					if (storymove <= ((98 * storymove3l)-1))
					{
						storymove += 7;
						storyx += 7;
					}
				}
				else if (storymove4 == 1)
				{
					if (storymove <= ((98 * storymove4l)-1))
					{
						storymove += 7;
						storyx -= 7;
					}
				}
				if ((storymove1 == 1) && (storymove >= (98 * storymove1l)))
				{
					storymove1 = 0;
					if ((storymove1 == 0) && (storymove2 == 0) && (storymove3 == 0) && (storymove4 == 0))
					{
						idk();
					}
					storymove = 0;
					storymove1l = 0;
				}
				else if ((storymove2 == 1) && (storymove >= (98 * storymove2l)))
				{
					storymove2 = 0;
					if ((storymove1 == 0) && (storymove2 == 0) && (storymove3 == 0) && (storymove4 == 0))
					{
						idk();
					}
					storymove = 0;
					storymove2l = 0;
				}
				else if ((storymove3 == 1) && (storymove >= (98 * storymove3l)))
				{
					storymove3 = 0;
					if ((storymove1 == 0) && (storymove2 == 0) && (storymove3 == 0) && (storymove4 == 0))
					{
						idk();
					}
					storymove = 0;
					storymove3l = 0;
				}
				else if ((storymove4 == 1) && (storymove >= (98 * storymove4l)))
				{
					storymove4 = 0;
					if ((storymove1 == 0) && (storymove2 == 0) && (storymove3 == 0) && (storymove4 == 0))
					{
						idk();
					}
					storymove = 0;
					storymove4l = 0;
				}
				
				//Select option if cross is pressed
				if ((pad_state.press & (PAD_START | PAD_CROSS)) && (storymove == 0))
				{
					if ((menu.select == 11) || (menu.select == 0))
					{
					if (((menu.select != 4) || ((menu.select == 4) && (stage.prefs.defeat != 1))) && (menu.select != 0))
					{
						//play confirm sound
						Audio_PlaySound(Sounds[1], 0x3fff);
						menu.last_select = menu.select;
						menu.next_page = MenuPage_Stage;
						menu.page_param.stage.id = menu_options[menu.select].stage;
						menu.page_param.stage.story = true;
						menu.trans_time = FIXED_UNIT;
					}
					else if ((menu.select == 4) && (stage.prefs.defeat == 1))
					{
						//play confirm sound
						Audio_PlaySound(Sounds[1], 0x3fff);
						menu.last_select = menu.select;
						menu.next_page = MenuPage_Defeat;
						menu.page_param.stage.id = menu_options[menu.select].stage;
						menu.page_param.stage.story = true;
						menu.trans_time = FIXED_UNIT;
					}
					}
					else if (disk == true)
					{
						disk = false;
						Audio_PlaySound(Sounds[2], 0x3fff);
					}
					else
						disk = true;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					if (disk == false)
					{
					menu.next_page = MenuPage_Main;
					menu.next_select = 0; //Story Mode
					Trans_Start();
					}
					else
						disk = false;
				}
			}
			
			//Draw week name and tracks
			menu.font_sus.draw(&menu.font_sus,
				menu_options[menu.select].name,
				160,
				61,
				FontAlign_Center
			);
			
			const char * const *trackp = menu_options[menu.select].tracks;
			for (size_t i = 0; i < COUNT_OF(menu_options[menu.select].tracks); i++, trackp++)
			{
				if (*trackp != NULL)
					menu.font_arial.draw(&menu.font_arial,
						*trackp,
						295,
						22 + (i * 11),
						FontAlign_Right
					);
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(48,1);
			menu.scroll += (next_scroll - menu.scroll) >> 3;
			
			//Draw selected option
			Menu_DrawWeek(menu_options[menu.select].week, 31);
			
			//Draw border
			RECT border_src = {  0,  0,255,191};
			RECT border_dst = {  0,  0,320,240};
			Gfx_DrawTex(&menu.tex_border, &border_src, &border_dst);
			
			//Draw map
			Menu_DrawShip();
			Menu_DrawStory();
			
			//Draw background
			Menu_DrawBack();
			
			break;
		}
		case MenuPage_Freeplay:
		{
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options0[] = {
				{StageId_SussusMoogus, 0xFF9271FD, "Sussus Moogus"},
				{StageId_Sabotage, 0xFF9271FD, "Sabotage"},
				{StageId_Meltdown, 0xFF9271FD, "Meltdown"},
				{StageId_SussusToogus, 0xFF9271FD, "Sussus Toogus"},
				{StageId_LightsDown, 0xFF9271FD, "Lights Down"},
				{StageId_Reactor, 0xFF9271FD, "Reactor"},
				{StageId_Ejected, 0xFF9271FD, "Ejected"},
				{StageId_Mando, 0xFF9271FD, "Mando"},
				{StageId_Dlow, 0xFF9271FD, "Dlow"},
				{StageId_Oversight, 0xFF9271FD, "Oversight"},
				{StageId_Danger, 0xFF9271FD, "Danger"},
				{StageId_DoubleKill, 0xFF9271FD, "Double Kill"},
				{StageId_Defeat, 0xFF9271FD, "Defeat"},
				{StageId_Finale, 0xFF9271FD, "Finale"},
				{StageId_IdentityCrisis, 0xFF9271FD, "Identity Crisis"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options1[] = {
				{StageId_Ashes, 0xFF9271FD, "Ashes"},
				{StageId_Magmatic, 0xFF9271FD, "Magmatic"},
				{StageId_BoilingPoint, 0xFF9271FD, "Boiling Point"},
				{StageId_Delusion, 0xFF9271FD, "Delusion"},
				{StageId_Blackout, 0xFF9271FD, "Blackout"},
				{StageId_Neurotic, 0xFF9271FD, "Neurotic"},
				{StageId_Heartbeat, 0xFF9271FD, "Heartbeat"},
				{StageId_Pinkwave, 0xFF9271FD, "Pinkwave"},
				{StageId_Pretender, 0xFF9271FD, "Pretender"},
				{StageId_SaucesMoogus, 0xFF9271FD, "Sauces Moogus"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options2[] = {
				{StageId_O2, 0xFF9271FD, "O2"},
				{StageId_VotingTime, 0xFF9271FD, "Voting Time"},
				{StageId_Turbulence, 0xFF9271FD, "Turbulence"},
				{StageId_Victory, 0xFF9271FD, "Victory"},
				{StageId_Roomcode, 0xFF9271FD, "ROOMCODE"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options3[] = {
				{StageId_SussyBussy, 0xFF9271FD, "Sussy Bussy"},
				{StageId_Rivals, 0xFF9271FD, "Rivals"},
				{StageId_Chewmate, 0xFF9271FD, "Chewmate"},
				{StageId_TomongusTuesday, 0xFF9271FD, "Tomongus Tuesday"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options4[] = {
				{StageId_Christmas, 0xFF9271FD, "Christmas"},
				{StageId_Spookpostor, 0xFF9271FD, "Spookpostor"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options5[] = {
				{StageId_Titular, 0xFF9271FD, "Titular"},
				{StageId_GreatestPlan, 0xFF9271FD, "Greatest Plan"},
				{StageId_Reinforcements, 0xFF9271FD, "Reinforcements"},
				{StageId_Armed, 0xFF9271FD, "Armed"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options6[] = {
				{StageId_AlphaMoogus, 0xFF9271FD, "Alpha Moogus"},
				{StageId_ActinSus, 0xFF9271FD, "Actin Sus"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options7[] = {
				{StageId_Ow, 0xFF9271FD, "Ow"},
				{StageId_Who, 0xFF9271FD, "Who"},
				{StageId_InsaneStreamer, 0xFF9271FD, "Insane Streamer"},
				{StageId_SussusNuzzus, 0xFF9271FD, "Sussus Nuzzus"},
				{StageId_Idk, 0xFF9271FD, "Idk"},
				{StageId_Esculent, 0xFF9271FD, "Esculent"},
				{StageId_Drippypop, 0xFF9271FD, "Drippypop"},
				{StageId_Crewicide, 0xFF9271FD, "Crewicide"},
				{StageId_MonotoneAttack, 0xFF9271FD, "Monotone Attack"},
				{StageId_Top10, 0xFF9271FD, "Top 10"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options8[] = {
				{StageId_Chippin, 0xFF9271FD, "Chippin"},
				{StageId_Chipping, 0xFF9271FD, "Chipping"},
				{StageId_Torture, 0xFF9271FD, "Torture"},
			};
			
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
			} menu_options9[] = {
				{StageId_LemonLime, 0xFF9271FD, "Lemon Lime"},
				{StageId_Chlorophyll, 0xFF9271FD, "Chlorophyll"},
				{StageId_Inflorescence, 0xFF9271FD, "Inflorescence"},
				{StageId_Stargazer, 0xFF9271FD, "Stargazer"},
			};
			
			if (menu.freeplaypage == 0)
				menu.freeplayoptions = COUNT_OF(menu_options0);
			else if (menu.freeplaypage == 1)
				menu.freeplayoptions = COUNT_OF(menu_options1);
			else if (menu.freeplaypage == 2)
				menu.freeplayoptions = COUNT_OF(menu_options2);
			else if (menu.freeplaypage == 3)
				menu.freeplayoptions = COUNT_OF(menu_options3);
			else if (menu.freeplaypage == 4)
				menu.freeplayoptions = COUNT_OF(menu_options4);
			else if (menu.freeplaypage == 5)
				menu.freeplayoptions = COUNT_OF(menu_options5);
			else if (menu.freeplaypage == 6)
				menu.freeplayoptions = COUNT_OF(menu_options6);
			else if (menu.freeplaypage == 7)
				menu.freeplayoptions = COUNT_OF(menu_options7);
			else if (menu.freeplaypage == 8)
				menu.freeplayoptions = COUNT_OF(menu_options8);
			else if (menu.freeplaypage == 9)
				menu.freeplayoptions = COUNT_OF(menu_options9);
			
			diskchange();
			
			menu.font_arial.draw(&menu.font_arial,
				scoredisp,
				230,
				23,
				FontAlign_Left
			);
			
			if (menu.freeplaypage == 0)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options0[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options0[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 1)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options1[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options1[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 2)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options2[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options2[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 3)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options3[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options3[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 4)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options4[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options4[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 5)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options5[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options5[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 6)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options6[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options6[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 7)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options7[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options7[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 8)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options8[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options8[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			else if (menu.freeplaypage == 9)
				sprintf(scoredisp, "SCORE: %d", (stage.prefs.savescore[menu_options9[menu.select].stage][menu.page_param.stage.diff] > 0) ? stage.prefs.savescore[menu_options9[menu.select].stage][menu.page_param.stage.diff] * 10 : 0);
			
			//Initialize page
			if (menu.page_swap)
			{
				menu.select = menu.last_select;
				menu.page_param.stage.last = false;
				menu.scroll = menu.freeplayoptions * FIXED_DEC(24 + screen.SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.freeplay.back_r = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_g = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_b = FIXED_DEC(255,1);
			}
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[3], 0x3fff);
					disk = false;
					if (menu.select > 0)
						menu.select--;
					else
					{
						if (menu.freeplaypage >= 1)
							menu.freeplaypage -= 1;
						else
							menu.freeplaypage = 9;
						if (menu.freeplaypage == 0)
							menu.select = 14;
						else if (menu.freeplaypage == 1)
							menu.select = 9;
						else if (menu.freeplaypage == 2)
							menu.select = 4;
						else if (menu.freeplaypage == 3)
							menu.select = 3;
						else if (menu.freeplaypage == 4)
							menu.select = 1;
						else if (menu.freeplaypage == 5)
							menu.select = 3;
						else if (menu.freeplaypage == 6)
							menu.select = 1;
						else if (menu.freeplaypage == 7)
							menu.select = 9;
						else if (menu.freeplaypage == 8)
							menu.select = 2;
						else if (menu.freeplaypage == 9)
							menu.select = 3;
					}
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[3], 0x3fff);
					disk = false;
					if (menu.select < menu.freeplayoptions - 1)
						menu.select++;
					else
					{
						if (menu.freeplaypage <= 8)
							menu.freeplaypage += 1;
						else
							menu.freeplaypage = 0;
						menu.select = 0;
						menu.scroll = 0;
					}
				}
				if (pad_state.press & PAD_RIGHT)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[5], 0x3fff);
					disk = false;
					if (menu.freeplaypage <= 8)
						menu.freeplaypage += 1;
					else
						menu.freeplaypage = 0;
					menu.select = 0;
					menu.scroll = 0;
				}
				if (pad_state.press & PAD_LEFT)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[6], 0x3fff);
					disk = false;
					if (menu.freeplaypage >= 1)
						menu.freeplaypage -= 1;
					else
						menu.freeplaypage = 9;
					menu.select = 0;
					menu.scroll = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					if ((menu.freeplaypage == 5) || (menu.freeplaypage == 6) || (menu.freeplaypage == 7) || (menu.freeplaypage == 8) || (menu.freeplaypage == 9))
					{
					if ((menu.select != 12) || ((menu.select == 12) && (stage.prefs.defeat != 1)))
					{
						//play confirm sound
						Audio_PlaySound(Sounds[4], 0x3fff);
						menu.last_select = menu.select;
						menu.next_page = MenuPage_Stage;
						if (menu.freeplaypage == 0)
							menu.page_param.stage.id = menu_options0[menu.select].stage;
						else if (menu.freeplaypage == 1)
							menu.page_param.stage.id = menu_options1[menu.select].stage;
						else if (menu.freeplaypage == 2)
							menu.page_param.stage.id = menu_options2[menu.select].stage;
						else if (menu.freeplaypage == 3)
							menu.page_param.stage.id = menu_options3[menu.select].stage;
						else if (menu.freeplaypage == 4)
							menu.page_param.stage.id = menu_options4[menu.select].stage;
						else if (menu.freeplaypage == 5)
							menu.page_param.stage.id = menu_options5[menu.select].stage;
						else if (menu.freeplaypage == 6)
							menu.page_param.stage.id = menu_options6[menu.select].stage;
						else if (menu.freeplaypage == 7)
							menu.page_param.stage.id = menu_options7[menu.select].stage;
						else if (menu.freeplaypage == 8)
							menu.page_param.stage.id = menu_options8[menu.select].stage;
						else if (menu.freeplaypage == 9)
							menu.page_param.stage.id = menu_options9[menu.select].stage;
						menu.page_param.stage.story = false;
						Trans_Start();
					}
					else if ((menu.select == 12) && (stage.prefs.defeat == 1))
					{
						//play confirm sound
						Audio_PlaySound(Sounds[4], 0x3fff);
						menu.last_select = menu.select;
						menu.next_page = MenuPage_Defeat;
						if (menu.freeplaypage == 0)
							menu.page_param.stage.id = menu_options0[menu.select].stage;
						else if (menu.freeplaypage == 1)
							menu.page_param.stage.id = menu_options1[menu.select].stage;
						else if (menu.freeplaypage == 2)
							menu.page_param.stage.id = menu_options2[menu.select].stage;
						else if (menu.freeplaypage == 3)
							menu.page_param.stage.id = menu_options3[menu.select].stage;
						else if (menu.freeplaypage == 4)
							menu.page_param.stage.id = menu_options4[menu.select].stage;
						else if (menu.freeplaypage == 5)
							menu.page_param.stage.id = menu_options5[menu.select].stage;
						else if (menu.freeplaypage == 6)
							menu.page_param.stage.id = menu_options6[menu.select].stage;
						else if (menu.freeplaypage == 7)
							menu.page_param.stage.id = menu_options7[menu.select].stage;
						else if (menu.freeplaypage == 8)
							menu.page_param.stage.id = menu_options8[menu.select].stage;
						else if (menu.freeplaypage == 9)
							menu.page_param.stage.id = menu_options9[menu.select].stage;
						menu.page_param.stage.story = false;
						Trans_Start();
					}
					}
					else if (disk == true)
					{
						disk = false;
						Audio_PlaySound(Sounds[2], 0x3fff);
					}
					else
						disk = true;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					if (disk == false)
					{
					menu.next_page = MenuPage_Main;
					menu.next_select = 1; //Freeplay
					Trans_Start();
					}
					else
						disk = false;
				}
			}
			
			//Draw upperbar
			RECT upperbar_src = {  0, 93,255, 17};
			RECT upperbar_dst = {
				0,
				0,
				320,
				21
			};
			Gfx_DrawTex(&menu.tex_story, &upperbar_src, &upperbar_dst);
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(32,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < menu.freeplayoptions; i++)
            {
                //Get position on screen
                s32 y = (i * 32) - 8 - (menu.scroll >> FIXED_SHIFT);
                if (y <= -screen.SCREEN_HEIGHT2 - 8)
                    continue;
                if (y >= screen.SCREEN_HEIGHT2 + 8)
                    break;
				
				if (menu.freeplaypage == 0)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options0[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 1)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options1[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 2)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options2[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 3)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options3[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 4)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options4[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 5)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options5[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 6)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options6[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 7)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options7[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 8)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options8[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}
				else if (menu.freeplaypage == 9)
				{
					//Draw text
					menu.font_sus.draw(&menu.font_sus,
						Menu_LowerIf2(menu_options9[i].text, menu.select != i),
						36 + (y >> 2),
						screen.SCREEN_HEIGHT2 + y - 8,
						FontAlign_Left
					);
				}

                //Draw thing
                RECT thing_src = {  0, 67,130, 25};
                RECT thing_dst = {
                    29 + (y >> 2),
                    screen.SCREEN_HEIGHT2 + y - 13,
                    130,
                    25
                };
				if (i == menu.select)
					Gfx_DrawTex(&menu.tex_story, &thing_src, &thing_dst);
				else
					Gfx_BlendTex(&menu.tex_story, &thing_src, &thing_dst, 1);
            }
			
			//Draw background
			Menu_DrawBack();
			break;
		}
		case MenuPage_Credits:
		{
			static const struct
			{
				StageId stage;
				const char *text;
				boolean difficulty;
			} menu_options[] = {
				{StageId_Temp, "PORT DEV", false},
				{StageId_Temp, "    PICHUMANTEN", false},
				{StageId_Temp, "ALSO HELPED PORT", false},
				{StageId_Temp, "    IGORSOU", false},
                {StageId_Temp, "    LUCKY", false},
                {StageId_Temp, "    UNSTOPABLE", false},
                {StageId_Temp, "    GEYT", false},
				{StageId_Temp, "PSXFUNKIN CREATOR", false},
				{StageId_Temp, "    CUCKYDEV", false},
                {StageId_Temp, "SPECIAL THANKS", false},
				{StageId_Temp, "    LORD SCOUT", false},
				{StageId_Temp, "    SPICYJPEG", false},
                {StageId_Temp, "    MAFURICK", false},
                {StageId_Temp, "    NINTENDOBRO", false},
                {StageId_Temp, "    DREAMCASTNICK", false},
				{StageId_Temp, "IMPOSTORM", false},
				{StageId_Temp, "    CLOWFOE", false},
                {StageId_Temp, "    EMI", false},
                {StageId_Temp, "    RAREBLIN", false},
                {StageId_Temp, "    LOGGO", false},
                {StageId_Temp, "    ETHANTHEDOODLER", false},
                {StageId_Temp, "    GONK", false},
                {StageId_Temp, "    SQUIDBOY", false},
                {StageId_Temp, "    FABS", false},
                {StageId_Temp, "    DOZENRAHYZ", false},
                {StageId_Temp, "    MAYHEW", false},
                {StageId_Temp, "    THALES", false},
                {StageId_Temp, "    NEATO", false},
                {StageId_Temp, "    MONOTONEDOC", false},
                {StageId_Temp, "    PUNKETT", false},
                {StageId_Temp, "    ORBYYORBINAUT", false},
                {StageId_Temp, "    CROCIDY", false},
                {StageId_Temp, "    AQUA", false},
                {StageId_Temp, "    OFFBI", false},
                {StageId_Temp, "    ELLISBRO", false},
                {StageId_Temp, "    MAYOKIDDO", false},
                {StageId_Temp, "    MSG", false},
                {StageId_Temp, "    AXOR THE AXOLOTL", false},
                {StageId_Temp, "    VRUZZEN", false},
                {StageId_Temp, "    NIISAN", false},
                {StageId_Temp, "    BIDDLE", false},
                {StageId_Temp, "    EMIHEAD", false},
                {StageId_Temp, "    KIWIQUEST", false},
                {StageId_Temp, "    SPAGGY", false},
                {StageId_Temp, "    PIP", false},
                {StageId_Temp, "    JULIEN", false},
                {StageId_Temp, "    AMONGUSFAN", false},
                {StageId_Temp, "    KEONI", false},
                {StageId_Temp, "    KEEGAN", false},
                {StageId_Temp, "    GALLY CID", false},
                {StageId_Temp, "    GIBZ", false},
                {StageId_Temp, "    FARFOXX", false},
                {StageId_Temp, "    KLUTCHDJ", false},
                {StageId_Temp, "    ZIFFY", false},
                {StageId_Temp, "    SASTER", false},
                {StageId_Temp, "    CVAL", false},
                {StageId_Temp, "    ROZEBUD", false},
                {StageId_Temp, "    JADS", false},
                {StageId_Temp, "    MASHPROTATO", false},
                {StageId_Temp, "    FLUFFYHAIRS", false},
                {StageId_Temp, "REST IN PEACE", false},
				{StageId_Temp, "    AMONGUSFAN", false},
			};
			    
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + screen.SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"CREDITS",
				16,
				screen.SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 3; //Credits
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -screen.SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= screen.SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					48 + (y >> 2),
					screen.SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack();
			break;
		}
		case MenuPage_Options:
		{
			static const char *gamemode_strs[] = {"NORMAL", "SWAP", "TWO PLAYER"};
			static const struct
			{
				enum
				{
					OptType_Boolean,
					OptType_Enum,
				} type;
				const char *text;
				void *value;
				union
				{
					struct
					{
						int a;
					} spec_boolean;
					struct
					{
						s32 max;
						const char **strs;
					} spec_enum;
				} spec;
			} menu_options[] = {
				{OptType_Enum,    "GAMEMODE", &stage.mode, {.spec_enum = {COUNT_OF(gamemode_strs), gamemode_strs}}},
				{OptType_Boolean, "GHOST TAP", &stage.prefs.ghost, {.spec_boolean = {0}}},
				{OptType_Boolean, "DOWNSCROLL", &stage.prefs.downscroll, {.spec_boolean = {0}}},
				{OptType_Boolean, "MIDDLESCROLL", &stage.prefs.middlescroll, {.spec_boolean = {0}}},
				{OptType_Boolean, "FLASHING LIGHTS", &stage.prefs.flash, {.spec_boolean = {0}}},
				{OptType_Boolean, "SHOW SONG TIME", &stage.prefs.songtimer, {.spec_boolean = {0}}},
				{OptType_Boolean, "MISS SOUNDS", &stage.prefs.sfxmiss, {.spec_boolean = {0}}},
				{OptType_Boolean, "DEFEAT MISSES", &stage.prefs.defeat, {.spec_boolean = {0}}},
				{OptType_Boolean, "PAL REFRESH RATE", &stage.prefs.palmode, {.spec_boolean = {0}}},
				{OptType_Boolean, "BOTPLAY", &stage.prefs.botplay, {.spec_boolean = {0}}},
				{OptType_Boolean, "PRACTICE MODE", &stage.prefs.practice, {.spec_boolean = {0}}},
				{OptType_Boolean, "DEBUG MODE", &stage.prefs.debug, {.spec_boolean = {0}}},
			};
			if (menu.select == 8 && pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
				stage.pal_i = 1;

			if (stage.mode == StageMode_2P)
				stage.prefs.middlescroll = false;
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + screen.SCREEN_HEIGHT2,1);
			
			RECT save_src = {0, 120, 55, 7};
			RECT save_dst = {screen.SCREEN_WIDTH / 2 + 30 - (121 / 2), screen.SCREEN_HEIGHT - 30, 53 * 2, 7 * 2};
			Gfx_DrawTex(&menu.tex_story, &save_src, &save_dst);

			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"OPTIONS",
				16,
				screen.SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Handle option changing
				switch (menu_options[menu.select].type)
				{
					case OptType_Boolean:
						if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
							*((boolean*)menu_options[menu.select].value) ^= 1;
						break;
					case OptType_Enum:
						if (pad_state.press & PAD_LEFT)
							if (--*((s32*)menu_options[menu.select].value) < 0)
								*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
						if (pad_state.press & PAD_RIGHT)
							if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
								*((s32*)menu_options[menu.select].value) = 0;
						break;
				}
				
				if (pad_state.press & PAD_SELECT)
					writeSaveFile();

				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 2; //Options
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -screen.SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= screen.SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				char text[0x80];
				switch (menu_options[i].type)
				{
					case OptType_Boolean:
						sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
						break;
					case OptType_Enum:
						sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
						break;
				}
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(text, menu.select != i),
					48 + (y >> 2),
					screen.SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack();
			break;
		}
		case MenuPage_Stage:
		{
			//Unload menu state
			Menu_Unload();
			
			//Load new stage
			LoadScr_Start();
			Stage_Load(menu.page_param.stage.id, menu.page_param.stage.diff, menu.page_param.stage.story);
			gameloop = GameLoop_Stage;
			LoadScr_End();
			break;
		}
		case MenuPage_Defeat:
		{
			//Initialize page
			if (menu.page_swap)
			{
				Audio_StopXA();
				menu.select = 0;
				menu.idk = false;
				menu.page_state.title.fade = FIXED_DEC(0,1);
				menu.page_state.title.fadespd = FIXED_DEC(0,1);
			}
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0)
				Trans_Start();
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_LEFT)
				{
					//play scroll sound
					Audio_PlaySound(Sounds[6], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = 6 - 1;
				}
				if (pad_state.press & PAD_RIGHT)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[5], 0x3fff);
					if (menu.select < 6 - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//play confirm sound
					Audio_PlaySound(Sounds[7], 0x3fff);
					menu.idk = true;
					menu.page_param.stage.id = StageId_Defeat;
					menu.next_page = MenuPage_Stage;
					menu.trans_time = FIXED_UNIT;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					if (menu.page_param.stage.last == true)
					{
						menu.next_page = MenuPage_Story;
						menu.next_select = 4; //Story Mode
					}
					else
					{
						menu.next_page = MenuPage_Freeplay;
						menu.next_select = 12; //Story Mode
					}
					Trans_Start();
					Audio_PlayXA_Track(XA_GettinFreaky, 0x40, 0, 1);
					Audio_WaitPlayXA();
				}
			}
			
			//Draw options
			if (menu.idk != true)
				Menu_DrawDefeat();
			if (menu.select == 0)
				stage.defeatmiss = 5;
			else if (menu.select == 1)
				stage.defeatmiss = 4;
			else if (menu.select == 2)
				stage.defeatmiss = 3;
			else if (menu.select == 3)
				stage.defeatmiss = 2;
			else if (menu.select == 4)
				stage.defeatmiss = 1;
			else if (menu.select == 5)
				stage.defeatmiss = 0;
			
			//Draw black
			RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
			Gfx_DrawRect(&screen_src, 0, 0, 0);
			break;
		}
		default:
			break;
	}
	
	//Clear page swap flag
	menu.page_swap = menu.page != exec_page;
}