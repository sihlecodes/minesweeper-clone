#include <stdlib.h>

#include "raylib.h"
#include "resource_dir.h"

#include "helpers.h"
#include "board.h"

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
			int cell_type = board.cells[i];
			int x = (i % board.cols);
			int y = (i / board.rows);

			Vector2 cell_position = board_map_to_global(&board, x, y);
			
			if ((cell_type & TYPE_HIDDEN) == TYPE_HIDDEN) {
				DrawRectangle(cell_position.x, cell_position.y, board.cell_size, board.cell_size, PURPLE);

				if ((cell_type & TYPE_FLAGGED) == TYPE_FLAGGED)
					DrawRectangle(cell_position.x, cell_position.y, board.cell_size, board.cell_size, YELLOW);

				continue;
			}

			int centered_x = cell_position.x + (board.cell_size) / 2;
			int centered_y = cell_position.y + (board.cell_size) / 2;

			if (cell_type == TYPE_BOMB)
				DrawText("X", centered_x, centered_y, 20, DARKPURPLE);

			else if (cell_type > 0) {
				char type[2];
				sprintf(type, "%d", cell_type);
				DrawText(type, centered_x, centered_y, 20, RED);
			}
		}

		EndDrawing();

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			board_reveal_at(&board, GetMouseX(), GetMouseY());
		}
		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			board_toggle_flag_at(&board, GetMouseX(), GetMouseY());
	}

	free(board.cells);

	CloseWindow();
	return 0;
}
