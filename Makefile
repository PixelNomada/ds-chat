# SPDX-License-Identifier: CC0-1.0

export BLOCKSDS ?= /opt/wonderful/thirdparty/blocksds/core
export BLOCKSDSEXT ?= /opt/wonderful/thirdparty/blocksds/external
export WONDERFUL_TOOLCHAIN ?= /opt/wonderful

NAME            := ds-chat
GAME_TITLE      := DS Chat
GAME_SUBTITLE   := WhatsApp DSi
GAME_AUTHOR     := Usuario
GAME_ICON       := 

SOURCEDIRS      := source
INCLUDEDIRS     :=
GFXDIRS         :=
BINDIRS         :=
AUDIODIRS       :=
NITROFSDIR      :=

# Con WiFi
ARM7ELF         := $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

LIBS            := -ldswifi9 -lnds9
LIBDIRS         := $(BLOCKSDS)/libs/dswifi \
                   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
