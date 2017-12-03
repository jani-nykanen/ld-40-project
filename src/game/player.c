/// Player (header)
/// (c) 2017 Jani Nykänen

#include "player.h"

#include "../engine/assets.h"
#include "../engine/graphics.h"
#include "../engine/controls.h"

#include "stage.h"

/// Player bitmap
static BITMAP* bmpPlayer = NULL;

/// Controls
static void pl_control(PLAYER*pl)
{
    if(any_pressed())
    {
        if(pl->canJump)
        {
            pl->gravity = -2.25f;
            pl->doubleJump = true;
            pl->canJump = false;
        }
        else if(pl->doubleJump)
        {
            pl->gravity = -1.75f;
            pl->doubleJump = false;
        }
    }

    pl->spinning = (any_down() && !pl->doubleJump && pl->gravity > 0.0f && !pl->canJump);
    
    if(!pl->canJump && any_released() && pl->gravity < 0.0f)
    {
        pl->gravity /= 1.75f;
    }
}

/// Animate
static void pl_animate(PLAYER*pl, float tm)
{
    if(pl->canJump)
    {
        float speed = get_global_speed();
        spr_animate(&pl->spr,0,0,5,5.0f-speed,tm);
    }
    else
    {
        if(pl->spinning)
        {
            spr_animate(&pl->spr,2,0,3,3,tm);
        }
        else
        {

            if(pl->doubleJump || pl->gravity > 0.0f)
            {
                int frame = pl->gravity < 0.0f ? 0 : 1;
                spr_animate(&pl->spr,1,frame,frame,0,tm);
            }
            else 
            {
                spr_animate(&pl->spr,1,2,5,3,tm);
            }

        }
    }
}

/// Move
static void pl_move(PLAYER* pl, float tm)
{
    pl->gravity += 0.075f * tm;
    if(pl->gravity > 2.0f)
        pl->gravity = 2.0f;

    if(pl->spinning)
    {
        pl->gravity = 0.25f;
    }

    pl->pos.y += pl->gravity * tm;

    pl->canJump = false;
    if(pl->pos.y > 96-12)
    {
        pl->pos.y = 96-12;
        pl->canJump = true;
        pl->gravity = 0.0f;
        pl->doubleJump = false;
        pl->spinning = false;
    }

    if(pl->hurtTimer > 0.0f)
        pl->hurtTimer -= 1.0f * tm;

    if(pl->pos.x < 24.0f)
        pl->pos.x += 0.5f * tm;
}

/// Create a player object
PLAYER create_player()
{
    if(bmpPlayer == NULL)
        bmpPlayer = get_bitmap("figure");

    PLAYER pl;
    pl.pos = vec2(-8.0f,96-12);
    pl.gravity = 0.0f;
    pl.canJump = true;
    pl.doubleJump = false;
    pl.spr = create_sprite(12,16);
    pl.health = 3;
    pl.hurtTimer = 0.0f;
    pl.money = 0;

    return pl;
}

/// Update player
/// < pl Player to update
/// < tm Time mul.
void pl_update(PLAYER* pl, float tm)
{
    pl_control(pl);
    pl_move(pl,tm);
    pl_animate(pl,tm);
}

/// Draw player
/// < pl Player
void pl_draw(PLAYER* pl)
{
    if(pl->hurtTimer <= 0.0f || (int)floor(pl->hurtTimer / 4.0f) % 2 == 0)
    {
        spr_draw(&pl->spr,bmpPlayer,pl->pos.x-6,pl->pos.y-16,0);
    }
}

/// Make him/her suffer
void pl_hurt(PLAYER* pl, VEC2 p, VEC2 d)
{
    if(pl->hurtTimer > 0.0f) return;

    if(pl->pos.x+2 > p.x && pl->pos.x-2 < p.x+d.x
        && pl->pos.y-2 > p.y && pl->pos.y-10 < p.y+d.y)
    {
        pl->hurtTimer = 60.0f;
        pl->health --;
    }
}