#include "board.h"
#include <stdlib.h>

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
	board_clear(&board);

	return board;
}

void board_resize(Board* board, int cols, int rows, float cell_size) {
	board->cols = cols;
	board->rows = rows;
	board->cell_size = cell_size;
	board->bounds.width = board->cols * board->cell_size;
	board->bounds.height = board->rows * board->cell_size;

	board->cells = malloc((sizeof *board->cells) * board->cols * board->rows);
}

void board_clear(Board* board) {
	memset(board->cells, 0, (sizeof *board->cells) * board->cols * board->rows);
}

void board_destroy(Board* board) {
	free(board->cells);
}

void board_populate(Board* board, int bomb_count) {
	Vector2 neighbours[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

	while (bomb_count > 0) {
		int cell = GetRandomValue(0, board->rows * board->cols - 1);

		if (board->cells[cell] != TYPE_BOMB) {
			board->cells[cell] = TYPE_BOMB;

			int x = (cell % board->cols);
			int y = (cell / board->rows);

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
	for (size_t i = 0; i < board->rows * board->cols; i++)
		board->cells[i] |= TYPE_HIDDEN;
}

void board_reveal_at_collapse(Board* board, int x, int y, List *visited) {
	if (!board_within_bounds(board, x, y))
		return;

	int cell_index = board_map_to_index(board, x, y);

	if (list_contains(visited, cell_index))
		return;

	list_append(visited, cell_index);
	board->cells[cell_index] &= board->cells[cell_index] ^ TYPE_HIDDEN;

	if (board->cells[cell_index] > 0)
		return;

	board_reveal_at_collapse(board, x - 1, y - 1, visited);
	board_reveal_at_collapse(board, x, y - 1, visited);
	board_reveal_at_collapse(board, x + 1, y - 1, visited);
	board_reveal_at_collapse(board, x + 1, y, visited);
	board_reveal_at_collapse(board, x + 1, y + 1, visited);
	board_reveal_at_collapse(board, x, y + 1, visited);
	board_reveal_at_collapse(board, x - 1, y + 1, visited);
	board_reveal_at_collapse(board, x - 1, y, visited);
}

void board_reveal_at(Board* board, int board_x, int board_y) {
	List visited;
	list_init(&visited, 5);
	board_reveal_at_collapse(board, board_x, board_y, &visited);
	list_deinit(&visited);
}

void board_toggle_flag_at(Board* board, int board_x, int board_y) {
	if (board_has_type_at(board, TYPE_HIDDEN, board_x, board_y))
		board->cells[board_map_to_index(board, board_x, board_y)] ^= TYPE_FLAGGED;
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