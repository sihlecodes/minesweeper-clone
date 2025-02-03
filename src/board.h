#pragma once

#include "raylib.h"
#include "helpers.h"

typedef enum CellType {
	TYPE_NONE = 0,
	TYPE_FLAGGED = 32,
	TYPE_BOMB = 64,
	TYPE_HIDDEN = 128,
} CellType;

typedef struct Board {
	int cols;
	int rows;
	float cell_size;
	int *cells;
	Rectangle bounds;
} Board;

bool board_within_bounds(Board* board, int x, int y);

Vector2 board_map_from_global(Board *board, float global_x, float global_y);

Vector2 board_map_to_global(Board *board, int board_x, int board_y);

int board_map_to_index(Board *board, int board_x, int board_y);

void board_populate(Board *board, int bomb_count);

void board_hide(Board *board);

void board_reveal_at_collapse(Board* board, int x, int y, List *visited);

void board_reveal_at(Board *board, float global_x, float global_y);

void board_toggle_flag_at(Board* board, float global_x, float global_y);