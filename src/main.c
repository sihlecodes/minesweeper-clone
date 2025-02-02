#include "raylib.h"

#include "resource_dir.h"
#include <stdlib.h>

typedef enum CellType {
	NONE = 0,
	BOMB = 64,
	HIDDEN = 128,
} CellType;

typedef struct Board {
	int cols;
	int rows;
	float cell_size;
	int *cells;
	Rectangle bounds;
} Board;

bool board_within_bounds(Board* board, int x, int y) {
	return x >= 0 && x < board->cols && y >= 0 && y < board->rows;
}

void board_populate(Board *board, int bomb_count) {
	if (board->cells != NULL)
		free(board->cells);

	size_t capacity = (sizeof *board->cells) * board->rows * board->cols;

	board->cells = malloc(capacity);
	memset(board->cells, 0, capacity);

	Vector2 neighbours[8] = { {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0} };

	while (bomb_count > 0) {
		int cell = GetRandomValue(0, board->rows * board->cols - 1);

		if (board->cells[cell] != BOMB) {
			board->cells[cell] = BOMB;

			int x = (cell % board->cols);
			int y = (cell / board->rows);

			for (size_t i = 0; i < 8; i++) {
				int neighbour_x = x + neighbours[i].x;
				int neighbour_y = y + neighbours[i].y;
				int neighbour_cell = neighbour_x + neighbour_y * board->cols;

				if (board_within_bounds(board, neighbour_x, neighbour_y))
					if (board->cells[neighbour_cell] != BOMB)
						board->cells[neighbour_cell]++;
			}

			bomb_count--;
		}
	}
}

void board_hide(Board* board) {
	for (size_t i = 0; i < board->rows * board->cols; i++)
		board->cells[i] |= HIDDEN;
}

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 800;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Raylib");
	SearchAndSetResourceDir("resources");

	Board board = {
		.cols = 8,
		.rows = 8,
		.cell_size = 64,
		.cells = NULL,
		
		.bounds = {
			.width = board.cols * board.cell_size,
			.height = board.rows * board.cell_size,
		}
	};

	board.bounds.x = (WINDOW_WIDTH - board.bounds.width) / 2;
	board.bounds.y = (WINDOW_HEIGHT - board.bounds.height) / 2;

	board_populate(&board, 10);
	board_hide(&board);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(SKYBLUE);

		for (size_t x = 0; x <= board.cols; x++) {
			float pos_x = board.bounds.x + x * board.cell_size;
			DrawLine(pos_x, board.bounds.y, pos_x, board.bounds.y + board.bounds.height, BLUE);
		}

		for (size_t y = 0; y <= board.rows; y++) {
			float pos_y = board.bounds.y + y * board.cell_size;
			DrawLine(board.bounds.x, pos_y, board.bounds.x + board.bounds.width, pos_y, BLUE);
		}

		for (size_t i = 0; i < board.rows * board.cols; i++) {
			int x = (i % board.cols);
			int y = (i / board.rows);

			Vector2 cell_pos = {
				board.bounds.x + x * board.cell_size,
				board.bounds.y + y * board.cell_size};
			
			if ((board.cells[i] & HIDDEN) == HIDDEN) {
				//DrawRectangle(cell_pos.x, cell_pos.y, board.cell_size, board.cell_size, PURPLE);
				//continue;
			}

			int cell_type = board.cells[i] ^ HIDDEN;

			int centered_x = cell_pos.x + (board.cell_size) / 2;
			int centered_y = cell_pos.y + (board.cell_size) / 2;

			if (cell_type == BOMB)
				DrawText("X", centered_x, centered_y, 20, DARKPURPLE);

			else if (cell_type > 0) {
				char type[2];
				sprintf(type, "%d", cell_type);
				DrawText(type, centered_x, centered_y, 20, RED);
			}
		}

		EndDrawing();
	}

	free(board.cells);

	CloseWindow();
	return 0;
}
