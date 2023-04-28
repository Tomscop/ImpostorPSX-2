#ifndef PSXF_GUARD_INTRO_H
#define PSXF_GUARD_INTRO_H

#include "../gfx.h"
#include "../io.h"
#include "../fixed.h"

typedef struct
{
	u16 timer;
	u32 sounds[6];
	struct
	{
		fixed_t x, y, w, h;
		s16 tx, ty;
	} logo1;
	struct
	{
		fixed_t x, y, w, h;
		s16 tx, ty;
		fixed_t shake;
	} logo2;
} Intro;

extern Intro intro;

//Menu functions
void Intro_Load();
void Intro_Unload();
void Intro_Tick();

#endif
