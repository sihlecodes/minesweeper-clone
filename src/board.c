#include "board.h"
#include <stdlib.h>
#include <assert.h>

bool board_within_bounds(Board* board, int x, int y) {
	return x >= 0 && x < board->cols && y >= 0 && y < board->rows;
}

Vector2 board_map_from_global(Board* board, float global_x, float global_y) {
	return (Vector2) {
		(global_x - board->bounds.x) / board->cell_size,
		(global_y - board->bounds.y) / board->cell_size,
	};
}

Vector2 board_map_to_global(Board* board, int board_x, int board_y) {
	return (Vector2) {
		board->bounds.x + board_x * board->cell_size,
		board->bounds.y + board_y * board->cell_size,
	};
}

int board_map_to_index(Board* board, int board_x, int board_y) {
	return board_x + board->cols * board_y;
}

Board board_create(int cols, int rows, float cell_size) {
	Board board = {0};
	board_resize(&board, cols, rows, cell_size);

	return board;
}

void board_resize(Board* board, int cols, int rows, float cell_size) {
	board_destroy(board);

	board->cols = cols;
	board->rows = rows;
	board->cell_size = cell_size;
	board->bounds.width = board->cols * board->cell_size;
	board->bounds.height = board->rows * board->cell_size;

	board->cells = malloc((sizeof *board->cells) * board->cols * board->rows);
	assert(board->cells != NULL);

	board_clear(board);
}

void board_center(Board* board, float width, float height) {
	board->bounds.x = (width - board->bounds.width) / 2;
	board->bounds.y = (height - board->bounds.height) / 2;
}

void board_clear(Board* board) {
	memset(board->cells, 0, (sizeof *board->cells) * board->cols * board->rows);
	board->hidden_count = 0;
}

void board_destroy(Board* board) {
	if (board->cells != NULL)
		free(board->cells);
}

void board_populate(Board* board, int bomb_count) {
	Vector2 neighbours[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
	board->bomb_count = board->flag_count = bomb_count;

	while (bomb_count > 0) {
		int cell = GetRandomValue(0, board->rows * board->cols - 1);

		if (board->cells[cell] != TYPE_BOMB) {
			board->cells[cell] = TYPE_BOMB;

			int x = (cell % board->cols);
			int y = (cell / board->cols);

			for (size_t i = 0; i < 8; i++) {
				int neighbour_x = x + neighbours[i].x;
				int neighbour_y = y + neighbours[i].y;

				if (board_within_bounds(board, neighbour_x, neighbour_y)) {
					int neighbour_index = board_map_to_index(board, neighbour_x, neighbour_y);

					if (board->cells[neighbour_index] != TYPE_BOMB)
						board->cells[neighbour_index]++;
				}
			}

			bomb_count--;
		}
	}
}

void board_hide(Board* board) {
	while (board->hidden_count < board->rows * board->cols)
		board->cells[board->hidden_count++] |= TYPE_HIDDEN;
}

void board_reveal_at(Board* board, int board_x, int board_y) {
	if (!board_is_hidden_at(board, board_x, board_y))
		return;

	board->hidden_count--;
	board->cells[board_map_to_index(board, board_x, board_y)] &= ~TYPE_HIDDEN;
}

static void recursive_board_reveal_collapse_at(Board* board, int x, int y, List *visited) {
	if (!board_within_bounds(board, x, y))
		return;

	int cell_index = board_map_to_index(board, x, y);

	if (list_contains(visited, cell_index))
		return;

	list_append(visited, cell_index);

	if (board_has_flag_at(board, x, y))
		board_toggle_flag_at(board, x, y);

	board_reveal_at(board, x, y);

	if (board->cells[cell_index] > 0)
		return;

	recursive_board_reveal_collapse_at(board, x - 1, y - 1, visited);
	recursive_board_reveal_collapse_at(board, x, y - 1, visited);
	recursive_board_reveal_collapse_at(board, x + 1, y - 1, visited);
	recursive_board_reveal_collapse_at(board, x + 1, y, visited);
	recursive_board_reveal_collapse_at(board, x + 1, y + 1, visited);
	recursive_board_reveal_collapse_at(board, x, y + 1, visited);
	recursive_board_reveal_collapse_at(board, x - 1, y + 1, visited);
	recursive_board_reveal_collapse_at(board, x - 1, y, visited);
}

void board_reveal_collapse_at(Board* board, int board_x, int board_y) {
	List visited = list_create(5);
	recursive_board_reveal_collapse_at(board, board_x, board_y, &visited);
	list_destroy(&visited);
}

void board_reveal_bombs(Board* board) {
	for (size_t i = 0; i < board->rows * board->cols; i++) {
		int x = i % board->cols;
		int y = i / board->cols;

		if (board_has_bomb_at(board, x, y))
			board_reveal_at(board, x, y);
	}
}

void board_toggle_flag_at(Board* board, int board_x, int board_y) {
	if (!board_is_hidden_at(board, board_x, board_y))
		return;

	if (board_has_flag_at(board, board_x, board_y))
		board->flag_count++;

	else if (board->flag_count > 0)
		board->flag_count--;

	else
		return;

	board->cells[board_map_to_index(board, board_x, board_y)] ^= TYPE_FLAGGED;

#if DEBUG
	printf("Flags left: %d\n", board->flag_count);
#endif
}

bool board_has_type_at(Board* board, CellType type, int board_x, int board_y) {
	return (board->cells[board_map_to_index(board, board_x, board_y)] & type) == type;
}

bool board_has_flag_at(Board* board, int board_x, int board_y) {
	return board_has_type_at(board, TYPE_FLAGGED, board_x, board_y);
}

bool board_has_bomb_at(Board* board, int board_x, int board_y) {
	return board_has_type_at(board, TYPE_BOMB, board_x, board_y);
}

bool board_is_hidden_at(Board* board, int board_x, int board_y) {
	return board_has_type_at(board, TYPE_HIDDEN, board_x, board_y);
}

unsigned int board_get_value_at(Board* board, int board_x, int board_y) {
	return board->cells[board_map_to_index(board, board_x, board_y)] & ~(TYPE_FLAGGED | TYPE_HIDDEN);
}