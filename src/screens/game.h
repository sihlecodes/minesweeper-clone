#pragma once

#include "raylib.h"
#include "board.h"

void render_screen_game(Board* board, RenderData* data, double elapsed);

void update_screen_game(Board *board, GameScreen* screen);