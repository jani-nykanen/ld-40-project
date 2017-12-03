/// Coin (header)
/// (c) 2017 Jani Nykänen

#ifndef __COIN__
#define __COIN__

#include "player.h"

#include "stdbool.h"

/// Coin structure
typedef struct
{
    VEC2 pos;
    float starty;
    float waveTimer;
    SPRITE spr;
    bool exist;

}COIN;

/// Create a coin (that does not exist)
/// > Coin
COIN create_coin();

/// Update coin
/// < o Coin to update
/// < pl Player object
/// < tm Time mul.
void coin_update(COIN* o, PLAYER* pl, float tm);

/// Draw a coin
/// < o Coin to draw
void coin_draw(COIN* o);

/// Push an coin to the game world
/// < o Coin to push
/// < x X coordinate
/// < y Y coordinate
void push_coin(COIN* o, float x, float y);

#endif // __COIN__