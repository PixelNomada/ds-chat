# SPDX-License-Identifier: CC0-1.0

export BLOCKSDS ?= /opt/blocksds/core
export BLOCKSDSEXT ?= /opt/blocksds/external
export WONDERFUL_TOOLCHAIN ?= /opt/wonderful

NAME            := ds-chat
GAME_TITLE      := DS Chat
GAME_SUBTITLE   := WhatsApp para DSi
GAME_AUTHOR     := TuNombre
GAME_ICON       := icon.gif

SOURCEDIRS      := source
INCLUDEDIRS     :=
GFXDIRS         :=
BINDIRS         :=
AUDIODIRS       :=
NITROFSDIR      :=

# Por ahora usamos el ARM7 básico. Más adelante cambiaremos a dswifi
ARM7ELF         := $(BLOCKSDS)/sys/arm7/main_core/arm7_maxmod.elf

LIBS            := -lnds9
LIBDIRS         := $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
