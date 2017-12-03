/// Obstacle (source)
/// (c) 2017 Jani Nykänen

#include "obstacle.h"

#include "../engine/graphics.h"
#include "../engine/assets.h"

#include "player.h"
#include "stage.h"

#include "stdlib.h"
#include "math.h"

/// Obstacle bitmap
static BITMAP* bmpObstacle =NULL;
/// Fish bitmap
static BITMAP* bmpFish =NULL;

/// Create an obstacle (that does not exist)
OBSTACLE create_obstacle()
{
    if(bmpObstacle == NULL)
        bmpObstacle = get_bitmap("obstacles");

    if(bmpFish == NULL)
        bmpFish = get_bitmap("fish");

    OBSTACLE o;
    o.fishSpr = create_sprite(24,12);
    o.exist = false;

    return o;
}

/// Update obstacle
void ob_update(OBSTACLE* o, PLAYER* pl, float tm)
{
    if(o->exist == false) return;

    float speed = get_global_speed();
    if(o->type == O_FISH) speed *= 1.5f;
    o->pos.x -= speed * tm;
    if( (o->type == O_FISH && o->pos.x < -24) || (o->type != O_FISH && o->pos.x < -16))
    {
        o->exist = false;
        return;
    }

    // Type specific behaviour
    switch(o->type)
    {
    case O_PLANT:
    {
        o->anim += 1.0f * tm;
        o->gravity += 0.05f * tm;
        if(o->gravity > 2.0f)
            o->gravity = 2.0f;
        o->pos.y += o->gravity *tm;
        if(o->pos.y > 96-24 && o->gravity >= 0.0f)
        {
            o->gravity = 0.0f;
            o->pos.y = 96-24;
        }

        if(!o->jumped)
        {
            o->jumpTimer -= 1.0f * tm;
            if(o->jumpTimer <= 0.0f)
            {
                o->gravity = -1.00f - (float) (rand() % 150) / 100.0f;
                o->jumped = true;
            }
        }

        pl_hurt(pl,vec2(o->pos.x+2,o->pos.y+2),vec2(8.0f,8.0f));
    }
    break;

    case O_FLYING:
    {
        o->waveTimer += 0.05f * tm;
        o->pos.y = o->startY + sin(o->waveTimer) * o->waveLength;
        pl_hurt(pl,vec2(o->pos.x+2,o->pos.y+2),vec2(8.0f,8.0f + 12.0f* (o->height-1)));
    }
    break;

    case O_POLE:
    {
        pl_hurt(pl,vec2(o->pos.x+2,o->pos.y+2),vec2(8.0f,8.0f + 12.0f* (o->height-1)));
    }
    break;

    case O_FISH:
    {
        spr_animate(&o->fishSpr,0,0,3,6,tm);
        pl_hurt(pl,vec2(o->pos.x+2,o->pos.y+2),vec2(20.0f,8.0f));
    }
    break;

    case O_GOAL:
    {
        if(pl->pos.x >= o->pos.x+12)
        {
            pl->victorous = true;
        }
    }
    break;

    default:
        break;
    }
}

/// Draw obstacle
void ob_draw(OBSTACLE* o)
{
    if(o->exist == false) return;

    switch(o->type)
    {
    case O_PLANT:
    {
        int frame = (int)floor(o->anim/8.0f) % 2;
        draw_bitmap_region(bmpObstacle,12*frame,48,12,12,(int)round(o->pos.x),(int)round(o->pos.y),0);
    }
    break;

    case O_POLE:
    {
        draw_bitmap_region(bmpObstacle,0,0,12,12,(int)round(o->pos.x),(int)round(o->pos.y),0);
        draw_bitmap_region(bmpObstacle,0,36,12,12,(int)round(o->pos.x),96-24,0);
        if(o->height > 2)
        {
            draw_bitmap_region(bmpObstacle,0,12,12,12,(int)round(o->pos.x),(int)round(o->pos.y) + 12,0);
        }

        int i = 0;
        for(; i < o->height-3; i++)
        {
            draw_bitmap_region(bmpObstacle,0,24,12,12,(int)round(o->pos.x),(int)round(o->pos.y) + (i+2)*12,0);
        }
    }
    break;

    case O_FLYING:
    {
        int i = -1;
        for(; i < (int)floor(o->pos.y/12); i++)
        {
            draw_bitmap_region(bmpObstacle,12,0,12,12,(int)round(o->pos.x),i*12 + (int)(o->pos.y-o->startY),0);
        }

        draw_bitmap_region(bmpObstacle,12,12,12,12,(int)round(o->pos.x),(int)round(o->pos.y) -12,0);
        draw_bitmap_region(bmpObstacle,12,24,12,12,(int)round(o->pos.x),(int)round(o->pos.y),0);
        if(o->height > 1)
        {
            draw_bitmap_region(bmpObstacle,12,36,12,12,(int)round(o->pos.x),(int)round(o->pos.y) + 12,0);
        }
    }
    break;

    case O_FISH:
    {
        spr_draw(&o->fishSpr,bmpFish,o->pos.x,o->pos.y,0);
    }
    break;

    case O_GOAL:
    {
        draw_bitmap_region(bmpObstacle,0,60,24,24,o->pos.x,o->pos.y,0);
    }
    break;

    default:
        break;
    }
}

/// Push an obstacle to the game world
void push_obstacle(OBSTACLE* o, int type)
{
    o->type = type;
    o->pos.x = 128;
    
    switch(type)
    {
    case O_PLANT:
    {
        o->anim = 0.0f;
        o->pos.y = 96-24;
        o->jumpTimer = (float)(rand() % 60 + 15);
        o->jumped = false;
    }
    break;

    case O_POLE:
    {
        int height = rand() % 2 + 2;
        o->pos.y = 96-12 - height*12;
        o->height = height;
    }
    break;

    case O_FLYING:
    {
        int height = rand() % 2 + 1;
        o->pos.y = (2 + rand() % 3) * 12;
        o->height = height;
        o->waveLength = (float)(rand() % 16 + 4);
    }
    break;

    case O_FISH:
    {
        o->pos.x += (float) (rand() % 24);
        o->pos.y = 24 + (float) (rand() % 48 );
    }
    break;

    case O_GOAL:
    {
        o->pos.y = 96-36;
    }
    break;

    default:
        break;
    }

    o->startY = o->pos.y;
    o->exist = true;
}
