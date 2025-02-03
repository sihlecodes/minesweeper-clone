#include "board.h"
#include <stdlib.h>

bool board_within_bounds(Board* board, int x, int y) {
	return x >= 0 && x < board->cols && y >= 0 && y < board->rows;
}

Vector2 board_map_from_global(Board *board, float global_x, float global_y) {
	return (struct Vector2) {
		(global_x - board->bounds.x) / board->cell_size,
			(global_y - board->bounds.y) / board->cell_size,
	};
}

Vector2 board_map_to_global(Board *board, int board_x, int board_y) {
	return (struct Vector2) {
		board->bounds.x + board_x * board->cell_size,
			board->bounds.y + board_y * board->cell_size,
	};
}

int board_map_to_index(Board *board, int board_x, int board_y) {
	return board_x + board->cols * board_y;
}

void board_populate(Board *board, int bomb_count) {
	if (board->cells != NULL)
		free(board->cells);

	size_t capacity = (sizeof *board->cells) * board->rows * board->cols;

	board->cells = malloc(capacity);
	memset(board->cells, 0, capacity);

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

void board_hide(Board *board) {
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

void board_reveal_at(Board *board, float global_x, float global_y) {
	List visited;
	list_init(&visited, 5);

	Vector2 board_position = board_map_from_global(board, global_x, global_y);
	board_reveal_at_collapse(board, board_position.x, board_position.y, &visited);

	list_deinit(&visited);
}

void board_toggle_flag_at(Board* board, float global_x, float global_y) {
	Vector2 board_position = board_map_from_global(board, global_x, global_y);

	int cell_index = board_map_to_index(board, board_position.x, board_position.y);
	board->cells[cell_index] ^= TYPE_FLAGGED;
}