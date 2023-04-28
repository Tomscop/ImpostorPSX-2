/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "events.h"
#include "stage.h"
#include "timer.h"
#include "random.h"
#include "mutil.h"

Events event_speed;

static void Events_Check(Event* event)
{
	//Events
	switch(event->event & EVENTS_FLAG_VARIANT)
	{
		case EVENTS_FLAG_SPEED: //Scroll Speed!!
		{
			event_speed.value1 = event->value1;
			event_speed.value2 = event->value2;
			break;
		}
		case EVENTS_FLAG_GF: //Set GF Speed!!
		{
			stage.gf_speed = (event->value1 >> FIXED_SHIFT) * 4;
			break;
		}
		case EVENTS_FLAG_CAMZOOM: //Add Camera Zoom!!
		{
			stage.charbump += event->value1;
			stage.bump += event->value2;
			break;
		}
		case EVENTS_FLAG_FLASH: //Flash!!
		{
			if((event->value1 == 0) || ((event->value1 >> FIXED_SHIFT) == 1))
			{
				stage.flash = FIXED_DEC(255,1);
				stage.flashspd = FIXED_DEC(1024,1);
			}
			break;
		}
		case EVENTS_FLAG_BEEP: //Reactor Beep!!
		{
			stage.bump += FIXED_DEC(3,100); //0.03
			stage.charbump += FIXED_DEC(15,1000); //0.015
			stage.reactor = FIXED_DEC(255,1);
			if (event->value1 == 409)
				stage.reactorspd = FIXED_DEC(875,1);
			if ((event->value1 >> FIXED_SHIFT) == 1)
				stage.reactorspd = FIXED_DEC(1024,1);
			break;
		}
		case EVENTS_FLAG_LIGHTS1: //Lights out!!
		{
			stage.lights = 1;
			stage.flash = FIXED_DEC(255,1);
			stage.flashspd = FIXED_DEC(1024,1);
			stage.player->health_bar = 0xFFFFFFFF;
			stage.opponent->health_bar = 0xFF000000;
			stage.player->health_i = 7;
			stage.opponent->health_i = 8;
			break;
		}
		case EVENTS_FLAG_LIGHTS2: //Lights on!!
		{
			stage.lights = 2;
			stage.player->health_bar = 0xFF29B5D6;
			stage.opponent->health_bar = 0xFF1F6B2B;
			stage.player->health_i = 20;
			stage.opponent->health_i = 4;
			break;
		}
		case EVENTS_FLAG_PINK: //pink toggle!!
		{
			if (stage.pinkstuff == false)
			{
				stage.pink = 0;
				stage.pinkstuff = true;
			}
			else
			{
				stage.pink = 2;
				stage.pinkstuff = false;
			}
			break;
		}
		case EVENTS_FLAG_HUDFADE: //HUD Fade!!
		{
			if ((event->value1 >> FIXED_SHIFT) == 0)
			{
				stage.hudfade = 0;
			}
			if ((event->value1 >> FIXED_SHIFT) == 1)
			{
				stage.hudfade = 1;
			}
			break;
		}
		case EVENTS_FLAG_WHOCAM: //Cam lock in Who!!
		{
			if (stage.song_step >= 450)
			{
			if ((event->value1 >> FIXED_SHIFT) == 0)
			{
				stage.camswitch = 0;
			}
			if ((event->value1 >> FIXED_SHIFT) == 1)
			{
				if ((event->value2 >> FIXED_SHIFT) == 0)
					stage.camswitch = 1;
				if ((event->value2 >> FIXED_SHIFT) == 1)
					stage.camswitch = 2;
			}
			}
			break;
		}
		case EVENTS_FLAG_VTCAM: //Cam lock in Who!!
		{
			if ((event->value1 >> FIXED_SHIFT) == 0)
			{
				stage.camswitch = 0;
			}
			if ((event->value1 >> FIXED_SHIFT) == 1)
			{
				if ((event->value2 >> FIXED_SHIFT) == 0)
					stage.camswitch = 1;
				if ((event->value2 >> FIXED_SHIFT) == 1)
					stage.camswitch = 2;
			}
			break;
		}
		case EVENTS_FLAG_BOP: //Alter Camera Bop!!
		{
			//Value 1
			if ((event->value1 >> FIXED_SHIFT) == 1)
			{
				stage.bopintense1 = FIXED_DEC(30,1000);
				stage.bopintense2 = FIXED_DEC(15,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 11)
			{
				stage.bopintense1 = FIXED_DEC(33,1000);
				stage.bopintense2 = FIXED_DEC(17,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 115)
			{
				stage.bopintense1 = FIXED_DEC(35,1000);
				stage.bopintense2 = FIXED_DEC(17,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 12)
			{
				stage.bopintense1 = FIXED_DEC(36,1000);
				stage.bopintense2 = FIXED_DEC(18,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 124)
			{
				stage.bopintense1 = FIXED_DEC(37,1000);
				stage.bopintense2 = FIXED_DEC(19,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 128)
			{
				stage.bopintense1 = FIXED_DEC(38,1000);
				stage.bopintense2 = FIXED_DEC(19,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 13)
			{
				stage.bopintense1 = FIXED_DEC(39,1000);
				stage.bopintense2 = FIXED_DEC(20,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 132)
			{
				stage.bopintense1 = FIXED_DEC(39,1000);
				stage.bopintense2 = FIXED_DEC(20,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 136)
			{
				stage.bopintense1 = FIXED_DEC(40,1000);
				stage.bopintense2 = FIXED_DEC(20,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 14)
			{
				stage.bopintense1 = FIXED_DEC(42,1000);
				stage.bopintense2 = FIXED_DEC(21,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 142)
			{
				stage.bopintense1 = FIXED_DEC(42,1000);
				stage.bopintense2 = FIXED_DEC(21,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 3)
			{
				stage.bopintense1 = FIXED_DEC(9,1000);
				stage.bopintense2 = FIXED_DEC(5,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 4)
			{
				stage.bopintense1 = FIXED_DEC(12,1000);
				stage.bopintense2 = FIXED_DEC(6,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 5)
			{
				stage.bopintense1 = FIXED_DEC(15,1000);
				stage.bopintense2 = FIXED_DEC(8,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 6)
			{
				stage.bopintense1 = FIXED_DEC(18,1000);
				stage.bopintense2 = FIXED_DEC(9,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 8)
			{
				stage.bopintense1 = FIXED_DEC(24,1000);
				stage.bopintense2 = FIXED_DEC(12,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 0)
			{
				stage.bopintense1 = FIXED_DEC(0,1000);
				stage.bopintense2 = FIXED_DEC(0,1000);
			}
			if ((event->value1 >> FIXED_SHIFT) == 100)
			{
				stage.bopintense1 = FIXED_DEC(0,1000);
				stage.bopintense2 = FIXED_DEC(0,1000);
			}
			//Value 2
			if ((event->value2 >> FIXED_SHIFT) == 1)
			{
				stage.bop1 = 0x3;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) == 2)
			{
				stage.bop1 = 0x7;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) == 4)
			{
				stage.bop1 = 0xF;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) == 8)
			{
				stage.bop1 = 0x1F;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) >= 40)
			{
				stage.bop2 = 222;
			}
			break;
		}
		default: //nothing lol
		break;
	}
}

void Events_Tick(void)
{
	//Scroll Speed!
	stage.speed += (FIXED_MUL(stage.ogspeed, event_speed.value1) - stage.speed) / (((event_speed.value2 / 60) + 1));
}

void Events_StartEvents(void)
{
	for (Event *event = stage.cur_event; event->pos != 0xFFFF; event++)
	{
		//Update event pointer
		if (event->pos > (stage.note_scroll >> FIXED_SHIFT))
			break;

		else
			stage.cur_event++;

		if (event->event & EVENTS_FLAG_PLAYED)
			continue;

		Events_Check(event);
		event->event |= EVENTS_FLAG_PLAYED;
	}

	//Same thing but for event.json
	if (stage.event_chart_data != NULL)
	{
		for (Event *event = stage.event_cur_event; event->pos != 0xFFFF; event++)
		{
			//Update event pointer
			if (event->pos > (stage.event_note_scroll >> FIXED_SHIFT))
				break;

			else
				stage.event_cur_event++;

			if (event->event & EVENTS_FLAG_PLAYED)
				continue;

			Events_Check(event);
			event->event |= EVENTS_FLAG_PLAYED;
		}
	}

	Events_Tick();
}

//Initialize some stuffs
void Events_Load(void)
{
	//Scroll Speed
	event_speed.value1 = FIXED_UNIT;
	event_speed.value2 = 0;
}