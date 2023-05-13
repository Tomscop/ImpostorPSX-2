/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"

//XA enumerations
typedef enum
{
	XA_Menu,   //MENU.XA
	XA_GameOvr,   //GAMEOVR.XA
	XA_17, //17
	XA_18, //18
	XA_19, //19
	XA_20, //20
	XA_21, //21
	XA_22, //22
	XA_23, //23
	XA_24, //24
	XA_25, //25
	XA_26, //26
	XA_27, //27
	XA_28, //28
	XA_29, //29
	XA_30, //30
	XA_31, //31
	
	XA_Max,
} XA_File;

typedef enum
{
	//MENU.XA
	XA_GettinFreaky, //Gettin' Freaky
	XA_GameOver,     //Game Over
	//GAMEOVR.XA
	XA_GameOverJ, //Game Over Jorsawsee
	XA_GameOverP, //Game Over Pico
	XA_GameOverH, //Game Over Henry
	//17.XA
	XA_SussyBussy, //Sussy Bussy
	XA_Rivals, //Rivals
	//18.XA
	XA_Chewmate, //Chewmate
	XA_TomongusTuesday, //Tomongus Tuesday
	//19.XA
	XA_Christmas, //Christmas
	XA_Spookpostor, //Spookpostor
	//20.XA
	XA_Titular, //Titular
	XA_GreatestPlan, //Greatest Plan
	//21.XA
	XA_Reinforcements, //Reinforcements
	XA_Armed, //Armed
	//22.XA
	XA_AlphaMoogus, //Alpha Moogus
	XA_ActinSus, //Actin Sus
	//23.XA
	XA_Ow, //Ow
	XA_Who, //Who
	//24.XA
	XA_InsaneStreamer, //Insane Streamer
	XA_SussusNuzzus, //Sussus Nuzzus
	//25.XA
	XA_Idk, //Idk
	XA_Esculent, //Esculent
	//26.XA
	XA_Drippypop, //Drippypop
	XA_Crewicide, //Crewicide
	//27.XA
	XA_MonotoneAttack, //Monotone Attack
	XA_Top10, //Top 10
	//28.XA
	XA_Chippin, //Chippin
	XA_Chipping, //Chipping
	//29.XA
	XA_Torture, //Torture
	XA_Grinch, //Grinch
	//30.XA
	XA_LemonLime, //Lemon Lime
	XA_Chlorophyll, //Chlorophyll
	//31.XA
	XA_Inflorescence, //Inflorescence
	XA_Stargazer, //Stargazer
	
	XA_TrackMax,
} XA_Track;

//Audio functions
u32 Audio_GetLength(XA_Track lengthtrack);
void Audio_Init(void);
void Audio_Quit(void);
void Audio_Reset(void);
void Audio_PlayXA_Track(XA_Track track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_Track(XA_Track track);
void Audio_PauseXA(void);
void Audio_ResumeXA(void);
void Audio_StopXA(void);
void Audio_ChannelXA(u8 channel);
s32 Audio_TellXA_Sector(void);
s32 Audio_TellXA_Milli(void);
boolean Audio_PlayingXA(void);
void Audio_WaitPlayXA(void);
void Audio_ProcessXA(void);
void findFreeChannel(void);
u32 Audio_LoadVAGData(u32 *sound, u32 sound_size);
void AudioPlayVAG(int channel, u32 addr);
void Audio_PlaySoundOnChannel(u32 addr, u32 channel, int volume);
void Audio_PlaySound(u32 addr, int volume);
u32 VAG_IsPlaying(u32 channel);
void Audio_ClearAlloc(void);

#endif
