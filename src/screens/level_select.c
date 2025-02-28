#include "level_select.h"

Clay_TextElementConfig config = {.fontSize = 24, .fontId = 0, .textColor = { 0, 0, 0, 255} };

void render_button(Clay_ElementId id, Clay_String difficulty) {
	Clay_Color hovered = {240,240,245,255};
	Clay_Color background = {245, 245, 250, 255};

	CLAY({
		.id = id,
		.layout = {
			.sizing = {CLAY_SIZING_PERCENT(0.2), CLAY_SIZING_PERCENT(0.5)},
			.childAlignment = { .x = CLAY_ALIGN_X_CENTER },
			.padding = CLAY_PADDING_ALL(20),
		},
		.cornerRadius = CLAY_CORNER_RADIUS(8),
		.backgroundColor = Clay_Hovered() ? hovered : background,
		}) {
		CLAY_TEXT(difficulty, &config);
	}
}

void render_screen_level_select(RenderData* data) {
	Clay_BeginLayout();

	CLAY({
		.layout = {
			.sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
			.childGap = 20,
			.childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
		},
		.backgroundColor = {250, 250, 255, 255},
		})
	{
		render_button(CLAY_ID("Beginner"), CLAY_STRING("Beginner"));
		render_button(CLAY_ID("Intermediate"), CLAY_STRING("Intermediate"));
		render_button(CLAY_ID("Expert"), CLAY_STRING("Expert"));
	}

	Clay_RenderCommandArray renderCommands = Clay_EndLayout();
	Clay_Raylib_Render(renderCommands, data->fonts);
}

void prepare_game(Board* board, int cols, int rows, int bomb_count, int cell_size, double* start, GameScreen* screen) {
	board_resize(board, cols, rows, cell_size);
	board_populate(board, bomb_count);
	board_hide(board);

	*start = GetTime();
	*screen = SCREEN_GAME;
}

void update_screen_level_select(Board* board, double* start, GameScreen* screen) {
	Clay_SetPointerState((Clay_Vector2) { GetMouseX(), GetMouseY() }, false);

	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		return;

	if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("Beginner"))))
		prepare_game(board, 8, 8, 10, 64, start, screen);

	else if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("Intermediate"))))
		prepare_game(board, 16, 16, 40, 48, start, screen);

	else if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("Expert"))))
		prepare_game(board, 30, 16, 99, 40, start, screen);
}