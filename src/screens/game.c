#include "game.h"

void render_screen_game(Board* board, RenderData* data, double elapsed) {
	DrawTextEx(data->fonts[0], TextFormat("Bombs: %d", board->bomb_count), (Vector2) { 20, 20 }, 24, 0, WHITE);
	DrawTextEx(data->fonts[0], TextFormat("Time: %.0lf", elapsed), (Vector2) { 20, 50 }, 24, 0, WHITE);

	for (size_t x = 0; x <= board->cols; x++) {
		float pos_x = board->bounds.x + x * board->cell_size;
		DrawLine(pos_x, board->bounds.y, pos_x, board->bounds.y + board->bounds.height, BLUE);
	}

	for (size_t y = 0; y <= board->rows; y++) {
		float pos_y = board->bounds.y + y * board->cell_size;
		DrawLine(board->bounds.x, pos_y, board->bounds.x + board->bounds.width, pos_y, BLUE);
	}

	for (size_t i = 0; i < board->rows * board->cols; i++) {
		int x = (i % board->cols);
		int y = (i / board->cols);

		Vector2 cell_position = board_map_to_global(board, x, y);
		unsigned int cell_value = board_get_value_at(board, x, y);

		if (board_is_hidden_at(board, x, y)) {
			DrawRectangle(cell_position.x, cell_position.y, board->cell_size, board->cell_size, PURPLE);

			if (board_has_flag_at(board, x, y))
				DrawRectangle(cell_position.x, cell_position.y, board->cell_size, board->cell_size, YELLOW);

			continue;
		}

		int centered_x = cell_position.x + (board->cell_size) / 2;
		int centered_y = cell_position.y + (board->cell_size) / 2;

		if (cell_value == TYPE_BOMB)
			DrawText("X", centered_x, centered_y, 20, DARKPURPLE);

		else if (cell_value > 0)
			DrawText(TextFormat("%d", cell_value), centered_x, centered_y, 20, RED);
	}
}

void update_screen_game(Board *board, GameScreen* screen) {
	Vector2 board_position = board_map_from_global(board, GetMouseX(), GetMouseY());

	if (!board_within_bounds(board, board_position.x, board_position.y))
		return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (board_has_flag_at(board, board_position.x, board_position.y))
			return;

		if (board_has_bomb_at(board, board_position.x, board_position.y)) {
			printf("Game over!\n");

			*screen = SCREEN_LEVEL_SELECT;
		}
		else
			board_reveal_at(board, board_position.x, board_position.y);
	}

	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && board_is_hidden_at(board, board_position.x, board_position.y))
		board_toggle_flag_at(board, board_position.x, board_position.y);
}