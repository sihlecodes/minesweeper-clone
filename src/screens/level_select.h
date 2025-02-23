#pragma once

#include "raylib.h"
#include "vendor/clay.h"
#include "vendor/clay_renderer_raylib.h"

#include "board.h"

void render_screen_level_select(RenderData* data);

void update_screen_level_select(Board* board, double* start, GameScreen* screen);