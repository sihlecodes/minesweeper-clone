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
	unsigned int *cells;
	int cols;
	int rows;
	int bomb_count;
	float cell_size;
	Rectangle bounds;
} Board;

Board board_create(int cols, int rows, float cell_size);

void board_resize(Board* board, int cols, int rows, float cell_size);

void board_center(Board* board, int width, int height);

void board_clear(Board* board);

void board_destroy(Board* board);

bool board_within_bounds(Board* board, int x, int y);

Vector2 board_map_from_global(Board* board, float global_x, float global_y);

Vector2 board_map_to_global(Board* board, int board_x, int board_y);

int board_map_to_index(Board* board, int board_x, int board_y);

void board_populate(Board* board, int bomb_count);

void board_hide(Board* board);

void board_reveal_at_collapse(Board* board, int x, int y, List *visited);

void board_reveal_at(Board* board, int board_x, int board_y);

void board_toggle_flag_at(Board* board, int board_x, int board_y);

bool board_has_type_at(Board* board, CellType type, int board_x, int board_y);

bool board_has_flag_at(Board* board, int board_x, int board_y);

bool board_has_bomb_at(Board* board, int board_x, int board_y);

bool board_is_hidden_at(Board* board, int board_x, int board_y);

unsigned int board_get_value_at(Board* board, int board_x, int board_y);