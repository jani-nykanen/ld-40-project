/// Stage (header)
/// (c) 2017 Jani Nykänen

#ifndef __STAGE__
#define __STAGE__

#include "player.h"

/// Initialize stage
void init_stage();

/// Update stage
/// < pl Player
/// < tm Time mul.
void update_stage(PLAYER* pl, float tm);

/// Draw stage
void draw_stage();

/// Get the global speed
/// > Global speed
float get_global_speed();

#endif // __STAGE__