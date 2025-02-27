#pragma once

#include "raylib.h"
#include "board.h"

void render_screen_game(Board* board, RenderData* data);

void update_screen_game(Board *board, double* start, GameScreen* screen);