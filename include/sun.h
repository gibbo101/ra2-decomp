// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#ifdef INCLUDE_COM
#include "isun.h"
#endif
#include <cstring>

#include "ability.hh"
#include "do.hh"
#include "facing.hh"

#ifdef _DEBUG
#define _DEBUG_ASSERT
#endif

#ifdef _DEBUG
#define _DEBUG_PRINT
#endif

#define NETWORK
#define TIMING_FIX	1

#ifdef _DEBUG

#endif

#define	INIT_FREE_DISK_SPACE		8192
#define	SAVE_GAME_DISK_SPACE		 (INIT_FREE_DISK_SPACE - (1024*4096))

#define SPEECH_BUFFER_SIZE		126000L

#define FAME_FILE_NAME			"HALLFAME.DAT"
#define NET_SAVE_FILE_NAME		"SAVEGAME.NET"
#define CONFIG_FILE_NAME		"SUN.INI"

#define	MAP_CELL_W				512
#define	MAP_CELL_H				512
#define	MAP_CELL_TOTAL			(MAP_CELL_W*MAP_CELL_H)

#define	REFRESH_EOL				Cell(32767, 32767)

#define CELL_PIXEL_W			24
#define CELL_PIXEL_H			48
#define CELL_LEPTON_W			256
#define CELL_LEPTON_H			256
#define CELL_LEPTON				256

#define LEPTON_TO_PIXEL(lepton) ((lepton) / 7)
#define PIXEL_TO_LEPTON(pixel)  ((pixel) * 7)

#define	PIXEL_LEPTON_W			(CELL_LEPTON_W/CELL_PIXEL_W)
#define	PIXEL_LEPTON_H			(CELL_LEPTON_H/CELL_PIXEL_H)

#define BRIDGE_CELL_HEIGHT		4

#define	REGION_WIDTH		4
#define	REGION_HEIGHT		4
#define	MAP_REGION_WIDTH	(((MAP_CELL_W + (REGION_WIDTH -1)) / REGION_WIDTH)+2)
#define	MAP_REGION_HEIGHT	(((MAP_CELL_H + (REGION_WIDTH -1)) / REGION_HEIGHT)+2)
#define  MAP_TOTAL_REGIONS	(MAP_REGION_WIDTH * MAP_REGION_HEIGHT)

#define	DESCRIP_MAX				44

#define	CONQUER_PATH_MAX		24

#define	MAX_EVENTS			128

#define MAX_LOG_LEVEL		10

#define	MAX_PLAYERS						8

enum {
	MAX_TEAM_CLASSCOUNT=6,
	MAX_TEAM_MISSIONS=50
};

#define NORMAL_LIGHT 1000
