# SPDX-License-Identifier: CC0-1.0

export BLOCKSDS ?= /opt/blocksds/core
export BLOCKSDSEXT ?= /opt/blocksds/external
export WONDERFUL_TOOLCHAIN ?= /opt/wonderful

NAME            := ds-chat
GAME_TITLE      := DS Chat
GAME_SUBTITLE   := Wha DSi
GAME_AUTHOR     := TuNombre
GAME_ICON       := icon.gif

SOURCEDIRS      := source
INCLUDEDIRS     :=
GFXDIRS         :=
BINDIRS         :=
AUDIODIRS       :=
NITROFSDIR      :=

# ARM7 con soporte WiFi
ARM7ELF         := $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

LIBS            := -ldswifi9 -lnds9
LIBDIRS         := $(BLOCKSDS)/libs/dswifi \
                   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
