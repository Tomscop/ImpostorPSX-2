/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_EVENTS_H
#define PSXF_GUARD_EVENTS_H

#include "psx.h"
#include "fixed.h"

//EVENTS
#define EVENTS_FLAG_VARIANT 0xFFFC

#define EVENTS_FLAG_SPEED     (1 << 2) //Change Scroll Speed
#define EVENTS_FLAG_GF        (1 << 3) //Set GF Speed
#define EVENTS_FLAG_CAMZOOM   (1 << 4) //Add Camera Zoom
#define EVENTS_FLAG_FLASH     (1 << 5) //Flash
#define EVENTS_FLAG_BEEP      (1 << 6) //Reactor Beep
#define EVENTS_FLAG_BOP       (1 << 7) //Alter Camera Bop
#define EVENTS_FLAG_LIGHTS1   (1 << 8) //Lights off
#define EVENTS_FLAG_LIGHTS2   (1 << 9) //Lights on
#define EVENTS_FLAG_HUDFADE   (1 << 10) //HUD Fade
#define EVENTS_FLAG_PINK      (1 << 11) //pink toggle
#define EVENTS_FLAG_WHOCAM    (1 << 12) //Cam lock in Who
#define EVENTS_FLAG_VTCAM     (1 << 13) //Cam lock in Voting Time

#define EVENTS_FLAG_PLAYED    (1 << 15) //Event has been already played

//Psych Engine Events Reader By IgorSou3000

typedef struct
{
  //psych engine shit
  u16 pos; //1/12 steps
  u16 event;
  s16 value1;
  s16 value2;
}Event;

typedef struct
{
  fixed_t value1, value2;
}Events;

void Events_ScrollSpeed(void);

void Events_Tick(void);
void Events_StartEvents(void);
void Events_Load(void);

extern Events event_speed;

#endif