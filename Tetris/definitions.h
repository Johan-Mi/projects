#pragma once

#include <stdint.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 24

#define TILE_SIZE 16

#define SCREEN_WIDTH (BOARD_WIDTH * TILE_SIZE)
#define SCREEN_HEIGHT (BOARD_HEIGHT * TILE_SIZE)

#define MAX_DENSITY 20 /* Used for randomizeBoard() */

typedef uint8_t u8;
typedef int8_t i8;
