#include "game.h"

#define FONT_SPACING 0
#define UI_FONT_SIZE 24

#define DARKRED CLITERAL(Color) { 161, 0, 0, 255 }
#define CYAN	CLITERAL(Color) { 0, 241, 241, 255 }

#define CLITERAL(type)
Color COLOR_VALUES[8] = { BLUE, DARKGREEN, RED, DARKBLUE, DARKRED, CYAN, BLACK, DARKGRAY };

#define CLITERAL(type) (type)

double elapsed;

void render_screen_game(Board* board, RenderData* data) {
	DrawTextEx(data->fonts[1], TextFormat("Flags: %d", board->flag_count), (Vector2) { 20, 20 }, UI_FONT_SIZE, FONT_SPACING, BLACK);
	DrawTextEx(data->fonts[1], TextFormat("Time: %.0lf", elapsed), (Vector2) { 20, 50 }, UI_FONT_SIZE, FONT_SPACING, BLACK);

	const int VALUE_FONT_SIZE = board->cell_size * .6;

	for (size_t x = 0; x <= board->cols; x++) {
		float pos_x = board->bounds.x + x * board->cell_size;
		DrawLine(pos_x, board->bounds.y, pos_x, board->bounds.y + board->bounds.height, GRAY);
	}

	for (size_t y = 0; y <= board->rows; y++) {
		float pos_y = board->bounds.y + y * board->cell_size;
		DrawLine(board->bounds.x, pos_y, board->bounds.x + board->bounds.width, pos_y, GRAY);
	}

	float texture_scale = board->cell_size / 128;

	for (size_t i = 0; i < board->rows * board->cols; i++) {
		int x = (i % board->cols);
		int y = (i / board->cols);

		Vector2 cell_position = board_map_to_global(board, x, y);
		unsigned int cell_value = board_get_value_at(board, x, y);

		if (board_is_hidden_at(board, x, y)) {
			DrawTextureEx(data->block, cell_position, 0, texture_scale, WHITE);

			if (board_has_flag_at(board, x, y))
				DrawTextureEx(data->flag, cell_position, 0, texture_scale, WHITE);

			continue;
		}

		const char* value = NULL;
		Color color;

		if (cell_value == TYPE_BOMB)
			DrawTextureEx(data->bomb, cell_position, 0, texture_scale, WHITE);

		else if (cell_value != 0) {
			color = COLOR_VALUES[cell_value - 1];
			value = TextFormat("%d", cell_value);
		}

		if (value != NULL) {
			Vector2 size = MeasureTextEx(data->fonts[1], value, VALUE_FONT_SIZE, FONT_SPACING);

			size.x = cell_position.x + (board->cell_size - size.x) / 2;
			size.y = cell_position.y + (board->cell_size - size.y) / 2;

			DrawTextEx(data->fonts[1], value, size, VALUE_FONT_SIZE, FONT_SPACING, color);
		}
	}
}

bool is_game_over = false;

void game_over(Board* board) {
	printf("Game over!\n");
	is_game_over = true;

	//*screen = SCREEN_LEVEL_SELECT;
	board_reveal_bombs(board);
}

void update_screen_game(Board *board, double* start, GameScreen* screen) {
	if (IsKeyPressed(KEY_SPACE)) {
		*screen = SCREEN_LEVEL_SELECT;
		is_game_over = false;
	}

	if (is_game_over)
		return;

	elapsed = GetTime() - *start;
	Vector2 board_position = board_map_from_global(board, GetMouseX(), GetMouseY());

	if (!board_within_bounds(board, board_position.x, board_position.y))
		return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (board_has_flag_at(board, board_position.x, board_position.y))
			return;

		if (board_has_bomb_at(board, board_position.x, board_position.y))
			return game_over(board);

		if (board_is_hidden_at(board, board_position.x, board_position.y))
			board_reveal_collapse_at(board, board_position.x, board_position.y);

		else if (board_box_reveal_at(board, board_position.x, board_position.y))
			return game_over(board);

		if (board->hidden_count == board->bomb_count) {
			printf("You win!");
			is_game_over = true;
		}
	}

	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && board_is_hidden_at(board, board_position.x, board_position.y))
		board_toggle_flag_at(board, board_position.x, board_position.y);
}