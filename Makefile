TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
	src/intro/intro.c \
	src/mutil.c \
	src/random.c \
	src/archive.c \
	src/font.c \
	src/trans.c \
	src/loadscr.c \
	src/menu.c \
	src/events.c \
	src/stage.c \
	src/debug.c \
	src/save.c \
	src/psx/psx.c \
	src/psx/io.c \
	src/psx/gfx.c \
	src/psx/audio.c \
	src/psx/pad.c \
	src/psx/timer.c \
	src/psx/strplay.c \
	src/animation.c \
	src/character.c \
	src/character/bf.c \
	src/character/bfpixel.c \
	src/character/bfchristmas.c \
	src/character/loggop.c \
	src/character/henryphone.c \
	src/character/bfv1.c \
	src/character/blueow.c \
	src/character/going2killevery1.c \
	src/character/kid.c \
	src/character/bfdrip.c \
	src/character/picodrip.c \
	src/character/pip.c \
	src/character/ziffy.c \
	src/character/tomongus.c \
	src/character/loggo.c \
	src/character/spooker.c \
	src/character/grinch.c \
	src/character/clowfoeg.c \
	src/character/henry.c \
	src/character/charles.c \
	src/character/ellie.c \
	src/character/righthandman.c \
	src/character/reginald.c \
	src/character/redv1.c \
	src/character/redow.c \
	src/character/yfronts.c \
	src/character/jerma.c \
	src/character/nuzzus.c \
	src/character/tobyfox.c \
	src/character/brown.c \
	src/character/drippostor.c \
	src/character/dave.c \
	src/character/amogus.c \
	src/character/cval.c \
	src/character/jads.c \
	src/character/dad.c \
	src/character/gf.c \
	src/character/speaker.c \
	src/character/gfpixel.c \
	src/character/gfv1.c \
	src/stage/polus.c \
	src/stage/cafeteria.c \
	src/stage/christmas.c \
	src/stage/gronch.c \
	src/stage/henrybg.c \
	src/stage/v1.c \
	src/stage/shields.c \
	src/stage/jermaroom.c \
	src/stage/earthbound.c \
	src/stage/idk.c \
	src/stage/electrical.c \
	src/stage/drip.c \
	src/stage/daveoffice.c \
	src/stage/towers.c \
	src/stage/o2.c \
	src/stage/week1.c \
	src/stage/dummy.c \
	src/object.c \
	src/object/combo.c \
	src/object/splash.c \
	src/pause.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
