/// Coin (source)
/// (c) 2017 Jani Nykänen

#include "coin.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "player.h"
#include "stage.h"

#include "stdlib.h"
#include "math.h"

/// Coin bitmap
static BITMAP* bmpCoin = NULL;

/// Create a coin (that does not exist)
COIN create_coin()
{
    if(bmpCoin == NULL) bmpCoin = get_bitmap("coin");

    COIN c;
    c.spr = create_sprite(10,10);
    c.exist = false;
    return c;
}

/// Update coin
void coin_update(COIN* o, PLAYER* pl, float tm)
{
    if(!o->exist) return;

    spr_animate(&o->spr,0,0,3,4,tm);

    float speed = get_global_speed();
    o->pos.x -= speed * tm;
    if(o->pos.x < -10)
        o->exist = false;

    if(pl->pos.x+2 > o->pos.x && pl->pos.x-2 < o->pos.x+10
        && pl->pos.y-2 > o->pos.y && pl->pos.y-12 < o->pos.y+10)
    {
        o->exist = false;
        pl->money ++;
    }

    o->waveTimer += 0.05f * tm;
    o->pos.y = o->starty + sin(o->waveTimer) * 2;
}

/// Draw a coin
void coin_draw(COIN* c)
{
    if(!c->exist) return;

    spr_draw(&c->spr,bmpCoin,(int)round(c->pos.x),(int)(c->pos.y),0);

    
}

/// Push an coin to the game world
void push_coin(COIN* c, float x, float y)
{
    c->exist = true;
    c->pos = vec2(x,y);
    c->starty = y;
    c->waveTimer = (float)(rand() % 1000)/(M_PI*2.0f);
}
