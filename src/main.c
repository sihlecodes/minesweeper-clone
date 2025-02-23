#include <stdlib.h>

#include "raylib.h"
#include "resource_dir.h"

#define CLAY_IMPLEMENTATION
#include "vendor/clay.h"

#define CLAY_RENDERER_RAYLIB_IMPLEMENTATION
#include "vendor/clay_renderer_raylib.h"

#include "screens.h"
#include "board.h"

Font fonts[2];

void handle_clay_errors(Clay_ErrorData errorData) {
	printf("%s\n", errorData.errorText.chars);
}

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 800;

	RenderData render_data = {
		.window_size = {
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
		},
		.fonts = fonts,
	};

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper clone");
	SearchAndSetResourceDir("resources");

	const int FONT_ID_BODY = 0;
	fonts[FONT_ID_BODY] = LoadFontEx("Roboto-Regular.ttf", 40, 0, 400);
	SetTextureFilter(fonts[FONT_ID_BODY].texture, TEXTURE_FILTER_BILINEAR);

	const int FONT_ID_HEADER = 1;
	fonts[FONT_ID_HEADER] = LoadFontEx("RobotoMono-Medium.ttf", 40, 0, 400);
	SetTextureFilter(fonts[FONT_ID_HEADER].texture, TEXTURE_FILTER_BILINEAR);

	Board board = {0};

	double start = GetTime();
	double elapsed;

	GameScreen screen = SCREEN_LEVEL_SELECT;

	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
	Clay_Initialize(arena, (Clay_Dimensions) { WINDOW_WIDTH, WINDOW_HEIGHT }, (Clay_ErrorHandler) { handle_clay_errors });
	
	Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
	//Clay_SetDebugModeEnabled(true);

	while (!WindowShouldClose())
	{
		elapsed = GetTime() - start;
		board_center(&board, WINDOW_WIDTH, WINDOW_HEIGHT);

		switch (screen) {
		case SCREEN_LEVEL_SELECT:
			update_screen_level_select(&board, &start, &screen);
			break;

		case SCREEN_GAME:
			update_screen_game(&board, &screen);
			break;
		}

		BeginDrawing();
		ClearBackground(SKYBLUE);

		switch (screen) {
		case SCREEN_LEVEL_SELECT:
			render_screen_level_select(&render_data);
			break;

		case SCREEN_GAME:
			render_screen_game(&board, &render_data, elapsed);
			break;
		}

		EndDrawing();
	}

	board_destroy(&board);

	UnloadFont(fonts[FONT_ID_BODY]);
	UnloadFont(fonts[FONT_ID_HEADER]);

	CloseWindow();
	return 0;
}
