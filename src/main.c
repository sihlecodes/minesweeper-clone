#include <stdlib.h>

#include "raylib.h"
#include "resource_dir.h"

#include "board.h"

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 800;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper clone");
	SearchAndSetResourceDir("resources");

	Board board = board_create(8, 8, 64);

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

		Vector2 board_position = board_map_from_global(&board, GetMouseX(), GetMouseY());

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (board_has_flag_at(&board, board_position.x, board_position.y))
				continue;

			if (board_has_bomb_at(&board, board_position.x, board_position.y)) {
				printf("Game over!\n");

				board_clear(&board);
				board_populate(&board, 10);
				board_hide(&board);
			}
			else 
				board_reveal_at(&board, board_position.x, board_position.y);
		}

		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			if (board_is_hidden_at(&board, board_position.x, board_position.y))
				board_toggle_flag_at(&board, board_position.x, board_position.y);
	}

	board_destroy(&board);

	CloseWindow();
	return 0;
}
