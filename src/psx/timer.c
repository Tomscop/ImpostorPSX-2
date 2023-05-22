/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "../timer.h"
#include "../stage.h"
#include "../audio.h"

#define TIMER_BITS (3)

int blockx = -318;
int blockx2 = -320;

//Timer state
Timer timer;
u32 frame_count, animf_count;
u32 timer_count, timer_lcount, timer_countbase;
u32 timer_persec;

fixed_t timer_sec, timer_dt, timer_secbase;

u8 timer_brokeconf;

//Timer interface
extern void InterruptCallback(int index, void *cb);
extern void ChangeClearRCnt(int t, int m);

void Timer_Callback(void) {
	timer_count++;
}

void Timer_Init(boolean pal_console, boolean pal_video)
{
	//Initialize counters
	frame_count = animf_count = timer_count = timer_lcount = timer_countbase = 0;
	timer_sec = timer_dt = timer_secbase = 0;
	
	//Setup counter IRQ
	static const u32 hblanks_per_sec[4] = {
		15734 >> TIMER_BITS, //!console && !video => 262.5 * 59.940
		15591 >> TIMER_BITS, //!console &&  video => 262.5 * 59.393
		15769 >> TIMER_BITS, // console && !video => 312.5 * 50.460
		15625 >> TIMER_BITS, // console &&  video => 312.5 * 50.000
	};
	timer_persec = hblanks_per_sec[(pal_console << 1) | pal_video];
	
	EnterCriticalSection();
	
	SetRCnt(RCntCNT1, 1 << TIMER_BITS, RCntMdINTR);
	InterruptCallback(5, (void*)Timer_Callback); //IRQ5 is RCNT1
	StartRCnt(RCntCNT1);
	ChangeClearRCnt(1, 0);
	
	ExitCriticalSection();
	
	timer_brokeconf = 0;
}

void Timer_Tick(void)
{
	u32 status = *((volatile const u32*)0x1f801814);
	
	//Increment frame count
	frame_count++;
	
	//Update counter time
	if (timer_count == timer_lcount)
	{
		if (timer_brokeconf != 0xFF)
			timer_brokeconf++;
		if (timer_brokeconf >= 10)
		{
			if ((status & 0x00100000) != 0)
				timer_count += timer_persec / 50;
			else
				timer_count += timer_persec / 60;
		}
	}
	else
	{
		if (timer_brokeconf != 0)
			timer_brokeconf--;
	}
	
	if (timer_count < timer_lcount)
	{
		timer_secbase = timer_sec;
		timer_countbase = timer_lcount;
	}
	fixed_t next_sec = timer_secbase + FIXED_DIV(timer_count - timer_countbase, timer_persec);
	
	timer_dt = next_sec - timer_sec;
	timer_sec = next_sec;
	
	if (!stage.paused)
		animf_count = (timer_sec * 24) >> FIXED_SHIFT;
	
	timer_lcount = timer_count;
}

void Timer_Reset(void)
{
	Timer_Tick();
	timer_dt = 0;
}

void StageTimer_Calculate()
{
	//Calculate the minutes and seconds
	if (timer.timer >= 60) {
		timer.timermin ++;
		timer.timer -= 60;
	}	
}

void StageTimer_Tick()
{
	//im deeply sorry for anyone reading this code
	//has the song started?
	if (stage.song_step > 0) //if song starts decrease the timer
		timer.timer = (Audio_GetLength(stage.stage_def->music_track)+1) - (stage.song_time / 1000); //seconds (ticks down)
	else //if not keep the timer at the song starting length    
		timer.timer = (Audio_GetLength(stage.stage_def->music_track)+1); //seconds (ticks down)
	timer.timermin = timer.timer / 60; //minutes left till song ends
	timer.timersec = timer.timer % 60; //seconds left till song ends
}

void StageTimer_Draw()
{
	//Draw song name
    stage.font_cdr.draw(&stage.font_cdr,
        stage.songname,
        -141, 
        (stage.prefs.downscroll) ? 104 : -113,
        FontAlign_Left
    );
	
	//draw square length
	RECT square_black = { 0, 249, 125, 6};
	RECT square_fill = { 0, 249, (124 * (stage.song_time) / (Audio_GetLength(stage.stage_def->music_track) * 1024)), 6};

	RECT_FIXED square_dst = {
	FIXED_DEC(-144,1),
	FIXED_DEC(-113,1),
	0,
	FIXED_DEC(10,1)
	};
	
	if (stage.prefs.downscroll)
		square_dst.y = -square_dst.y - square_dst.h + FIXED_DEC(1,1);

	square_dst.w = square_fill.w << FIXED_SHIFT;
	if (stage.song_step >= 0)
		Stage_DrawTexCol(&stage.tex_hud1, &square_fill, &square_dst, stage.bump, 71, 213, 70);
	
	square_dst.w = square_black.w << FIXED_SHIFT;
	Stage_DrawTexCol(&stage.tex_hud1, &square_black, &square_dst, stage.bump, 54, 64, 54);
}

void StageInfo_Draw()
{
	//Draw song name
    stage.font_cdr.draw(&stage.font_cdr,
        stage.songname,
        blockx, 
        -58,
        FontAlign_Left
    );
	
	//Draw composor name
    stage.font_cdr.draw(&stage.font_cdr,
        stage.composer,
        blockx, 
        -48,
        FontAlign_Left
    );
	stage.font_cdr.draw(&stage.font_cdr,
        stage.composer2,
        blockx, 
        -38,
        FontAlign_Left
    );
	
	//draw block
	RECT block_src = { 2, 250, 2, 2};
	RECT_FIXED block_dst = {
	FIXED_DEC(blockx2,1),
	FIXED_DEC(-60,1),
	FIXED_DEC(120,1),
	FIXED_DEC(32,1)
	};
	
	if ((stage.stage_id == StageId_SussusToogus) || (stage.stage_id == StageId_Reactor) || (stage.stage_id == StageId_IdentityCrisis) || (stage.stage_id == StageId_Ashes) || (stage.stage_id == StageId_BoilingPoint) || (stage.stage_id == StageId_Reinforcements) || (stage.stage_id == StageId_Armed) || (stage.stage_id == StageId_ActinSus) || (stage.stage_id == StageId_InsaneStreamer) || (stage.stage_id == StageId_SussusNuzzus) || (stage.stage_id == StageId_Drippypop))
		block_dst.h += FIXED_DEC(6,1);
	if ((stage.stage_id == StageId_SussusMoogus) || (stage.stage_id == StageId_Sabotage) || (stage.stage_id == StageId_SussusToogus) || (stage.stage_id == StageId_Reactor) || (stage.stage_id == StageId_Ashes) || (stage.stage_id == StageId_BoilingPoint) || (stage.stage_id == StageId_Pretender) || (stage.stage_id == StageId_Grinch) || (stage.stage_id == StageId_Titular) || (stage.stage_id == StageId_GreatestPlan) || (stage.stage_id == StageId_Reinforcements) || (stage.stage_id == StageId_Armed) || (stage.stage_id == StageId_Who) || (stage.stage_id == StageId_InsaneStreamer) || (stage.stage_id == StageId_Drippypop) || (stage.stage_id == StageId_SussusNuzzus) || (stage.stage_id == StageId_Finale) || (stage.stage_id == StageId_IdentityCrisis) || (stage.stage_id == StageId_Dlow) || (stage.stage_id == StageId_ActinSus) || (stage.stage_id == StageId_Top10))
		block_dst.w += FIXED_DEC(26,1);
	if ((stage.stage_id == StageId_Magmatic) || (stage.stage_id == StageId_VotingTime) || (stage.stage_id == StageId_ActinSus) || (stage.stage_id == StageId_SussusNuzzus))
		block_dst.w += FIXED_DEC(9,1);
	
	if ((stage.song_step == -29) || ((stage.song_step == 0) && ((stage.stage_id == StageId_AlphaMoogus) || (stage.stage_id == StageId_ActinSus) || (stage.stage_id == StageId_Torture))))
	{
		blockx = -318;
		blockx2 = -320;
	}
	if ((stage.song_step >= -15) && (stage.song_step <= 15) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)) && (stage.stage_id != StageId_AlphaMoogus) && (stage.stage_id != StageId_ActinSus) && (stage.stage_id != StageId_Torture))
	{
		blockx += 5;
		blockx2 += 5;
	}
	if ((stage.song_step >= 24) && (stage.song_step <= 39) && (blockx != -318) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-320,1)))
	{
		blockx -= 5;
		blockx2 -= 5;
	}
	
	if (stage.stage_id == StageId_Torture)
	{
		if ((stage.song_step >= (32 * 4)) && (stage.song_step <= (32 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Composer: JADS");
			strcpy(stage.composer2, "");
		}
		if ((stage.song_step >= (128 * 4)) && (stage.song_step <= (128 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Composer: Cval");
			strcpy(stage.composer2, "");
		}
		if ((stage.song_step >= (160 * 4)) && (stage.song_step <= (160 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Composer: Ziffy");
			strcpy(stage.composer2, "");
		}
		if ((stage.song_step >= (224 * 4)) && (stage.song_step <= (224 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Composer: Fluffyhairs");
			strcpy(stage.composer2, "");
		}
		if ((stage.song_step >= (256 * 4)) && (stage.song_step <= (256 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Featuring: Rozebud");
			strcpy(stage.composer2, "");
		}
		if ((stage.song_step >= (272 * 4)) && (stage.song_step <= (272 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Nevermind here's Fluffyhairs");
			strcpy(stage.composer2, "again.");
		}
		if ((stage.song_step >= (336 * 4)) && (stage.song_step <= (336 * 4)+30) && (blockx != -158) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-160,1)))
		{
			blockx += 5;
			blockx2 += 5;
			strcpy(stage.composer, "Composer: Ziffy (again)");
			strcpy(stage.composer2, "");
		}
		if ((((stage.song_step >= (32 * 4)+39) && (stage.song_step <= (32 * 4)+54)) || ((stage.song_step >= (128 * 4)+39) && (stage.song_step <= (128 * 4)+54)) || ((stage.song_step >= (160 * 4)+39) && (stage.song_step <= (160 * 4)+54)) || ((stage.song_step >= (224 * 4)+39) && (stage.song_step <= (224 * 4)+54)) || ((stage.song_step >= (256 * 4)+39) && (stage.song_step <= (256 * 4)+54)) || ((stage.song_step >= (272 * 4)+39) && (stage.song_step <= (272 * 4)+54)) || ((stage.song_step >= (336 * 4)+39) && (stage.song_step <= (336 * 4)+54))) && ((blockx != -318) && (FIXED_DEC(blockx2,1) != FIXED_DEC(-320,1))))
		{
			blockx -= 5;
			blockx2 -= 5;
		}
		if ((stage.song_step >= 1087) && (stage.song_step <= 1143))
		{
			block_dst.w += FIXED_DEC(35,1);
			block_dst.h += FIXED_DEC(6,1);
		}
	}
	
	Stage_BlendTex(&stage.tex_hud1, &block_src, &block_dst, stage.bump, 1);
}
