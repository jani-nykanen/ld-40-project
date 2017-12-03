/// Stage (source)
/// (c) 2017 Jani Nykänen

#include "stage.h"

#include "../engine/assets.h"
#include "../engine/graphics.h"

#include "math.h"

/// Floor pos
static float fpos;
/// Global speed
static float gspeed;
/// Sky phase
static int skyPhase;
/// Old sky phase
static int oldSky;
/// Sky change timer
static float skyChangeTimer;

/// Bitmaps
static BITMAP* bmpSky;
static BITMAP* bmpBush;
static BITMAP* bmpHills;
static BITMAP* bmpTiles;

/// Initialize stage
void init_stage()
{
    fpos = 0.0f;
    gspeed = 1.0f;
    skyChangeTimer = 0.0f;
    skyPhase = 0;
    oldSky = 0;

    bmpSky = get_bitmap("sky");
    bmpBush  = get_bitmap("bush");
    bmpHills  = get_bitmap("hills");
    bmpTiles =  get_bitmap("tiles");
}

/// Update stage
void update_stage(PLAYER* pl, float tm)
{
    gspeed = 1.0f + (int)floor(pl->money/5) * 0.1f;
    skyPhase = (int)floor(pl->money/10);
    if(skyPhase > 3) skyPhase = 3;

    if(skyPhase != oldSky)
    {
        skyChangeTimer = 60.0f;
    }
    if(skyChangeTimer > 0.0f)
    {
        skyChangeTimer -= 2.0f * tm;
    }

    fpos += gspeed * tm;
    if(fpos >= 512.0f)
        fpos -= 512.0f;

    oldSky = skyPhase;
}

/// Draw stage
void draw_stage()
{
    int i = 0;

    // Sky
    if(skyChangeTimer <= 0.0f)
        draw_bitmap_region(bmpSky,0,(bmpSky->h/4.0f)*skyPhase,bmpSky->w,bmpSky->h/4,0,0,0);
    else
    {
        draw_bitmap_region(bmpSky,0,(bmpSky->h/4.0f)*(skyPhase-1),bmpSky->w,bmpSky->h/4,0,0,0);
        int skip = 6 - (int)floor(skyChangeTimer/10.0f);
        draw_skipped_bitmap_region(bmpSky,0,(bmpSky->h/4.0f)*(skyPhase),bmpSky->w,bmpSky->h/4,0,0,skip,skip,0);
    }

    // Hills
    int p = -((int) round(fpos/4.0f)) % 128;
    for(i=0; i <2; i++)
    {
        draw_bitmap(bmpHills,p+i*128,24,0);
    }

    // Bush
    p = -((int) round(fpos/2.0f)) % 64;
    for(i=0; i <3; i++)
    {
        draw_bitmap(bmpBush,p+i*64,96-12 - 24,0);
    }

    // Floor
    p = -((int)round(fpos)) % 16;
    for(i=0; i < 9; i++)
    {
        draw_bitmap(bmpTiles,p + i*16,96-12,0);
    }
}

/// Get the global speed
float get_global_speed()
{
    return gspeed;
}