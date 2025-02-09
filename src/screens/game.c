#include "game.h"

#define FONT_SPACING 0
#define UI_FONT_SIZE 24

void render_screen_game(Board* board, RenderData* data, double elapsed) {
	DrawTextEx(data->fonts[1], TextFormat("Bombs: %d", board->bomb_count), (Vector2) { 20, 20 }, UI_FONT_SIZE, FONT_SPACING, WHITE);
	DrawTextEx(data->fonts[1], TextFormat("Time: %.0lf", elapsed), (Vector2) { 20, 50 }, UI_FONT_SIZE, FONT_SPACING, WHITE);

	const int VALUE_FONT_SIZE = board->cell_size * .55;

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

		char* value = NULL;
		Color color = DARKPURPLE;

		if (cell_value == TYPE_BOMB)
			value = "X";

		else if (cell_value > 0) {
			value = TextFormat("%d", cell_value);
			color = RED;
		}

		if (value != NULL) {
			Vector2 size = MeasureTextEx(data->fonts[1], value, VALUE_FONT_SIZE, FONT_SPACING);

			size.x = cell_position.x + (board->cell_size - size.x) /2;
			size.y = cell_position.y + (board->cell_size - size.y) /2;

			DrawTextEx(data->fonts[1], value, size, VALUE_FONT_SIZE, FONT_SPACING, color);
		}
	}
}

bool input_disabled = false;

void update_screen_game(Board *board, GameScreen* screen) {
	if (IsKeyPressed(KEY_SPACE)) {
		*screen = SCREEN_LEVEL_SELECT;
		input_disabled = false;
	}

	if (input_disabled)
		return;

	Vector2 board_position = board_map_from_global(board, GetMouseX(), GetMouseY());

	if (!board_within_bounds(board, board_position.x, board_position.y))
		return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (board_has_flag_at(board, board_position.x, board_position.y))
			return;

		if (board_has_bomb_at(board, board_position.x, board_position.y)) {
			printf("Game over!\n");

			input_disabled = true;
			//*screen = SCREEN_LEVEL_SELECT;

			board_reveal_bombs(board);
		}

		board_reveal_collapse_at(board, board_position.x, board_position.y);
	}

	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && board_is_hidden_at(board, board_position.x, board_position.y))
		board_toggle_flag_at(board, board_position.x, board_position.y);
}