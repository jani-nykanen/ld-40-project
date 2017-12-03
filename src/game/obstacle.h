/// Obstacle (header)
/// (c) 2017 Jani Nykänen

#ifndef __OBSTACLE__
#define __OBSTACLE__

#include "player.h"

#include "stdbool.h"

/// Obstacle types
enum
{
    O_POLE = 0,
    O_PLANT = 1,
    O_FLYING = 2,
    O_FISH = 3,
    O_GOAL = 4,
};

/// Obstacle structure
typedef struct
{
    VEC2 pos;
    float startY;
    float gravity;
    int type;
    int height;
    float anim;
    float jumpTimer;
    float waveLength;
    float waveTimer;
    bool exist;
    SPRITE fishSpr;
    bool jumped;

}OBSTACLE;

/// Create an obstacle (that does not exist)
/// > Obstacle
OBSTACLE create_obstacle();

/// Update obstacle
/// < o Obstacle to update
/// < pl Player object
/// < tm Time mul.
void ob_update(OBSTACLE* o, PLAYER* pl, float tm);

/// Draw obstacle
/// < o Obstacle to draw
void ob_draw(OBSTACLE* o);

/// Push an obstacle to the game world
/// < o Obstacle to push
/// < type Type
void push_obstacle(OBSTACLE* o, int type);

#endif // __OBSTACLE__