/// Title scene (source)
/// (c) 2017 Jani Nykänen

#include "title.h"

#include "../engine/app.h"
#include "../engine/graphics.h"
#include "../engine/bitmap.h"
#include "../engine/controls.h"
#include "../engine/assets.h"
#include "../engine/transform.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

#include "../game/game.h"

/// Background frame
static FRAME* frameBg;
/// Frame drawn
static bool frameDrawn;
/// Logo bitmap
static BITMAP* bmpLogo;
/// Dollars bitmap
static BITMAP* bmpDollars;
/// Font bitmap
static BITMAP* bmpFont;
/// Angle
static float angle;
/// Title timer
static float titleTimer;
/// Title phase
static int titlePhase;

/// Initialize title scene
/// > 0 on success
int title_init()
{
    frameBg = frame_create(128,96);
    if(frameBg == NULL)
    {
        return 1;
    }
    frameDrawn = false;

    bmpLogo = get_bitmap("logo");
    bmpDollars = get_bitmap("dollars");
    bmpFont = get_bitmap("font2");

    angle = 0.0f;

    titlePhase = 0;
    titleTimer = 0.0f;

    return 0;
} 

/// Update title scene
/// < tm Time mul.
void title_update(float tm)
{
    if(titlePhase == 0)
    {
        titleTimer += 2.0f * tm;
        if(titleTimer >= 60.0f)
        {
            titleTimer = 0.0f;
            titlePhase = 1;
        }
    }
    else if(titlePhase == 1)
    {
        if(any_pressed())
        {
            titlePhase = 2;
        }

        angle += 0.01f * tm;
    }
    else if(titlePhase == 2)
    {
        titleTimer += 2.0f * tm;
        if(titleTimer >= 60.0f)
        {
            app_swap_scene("game");
        }
    }
}

/// Draw title screen
void title_draw()
{
    int skip = 0;

    if(!frameDrawn)
    {
        bind_frame(frameBg);
        game_draw();
        bind_frame(app_get_canvas());
        frameDrawn = true;
    }

    clear_frame(0b00101010);

    draw_inverted_bitmap((BITMAP*)frameBg,0,0,0);
    if(titlePhase == 2)
    {
        skip = (int)(floor(titleTimer / 10.0f)) +1;
        draw_skipped_bitmap_region((BITMAP*)frameBg,0,0,128,96, 0,0,skip,skip,0);
    }
    else if(titlePhase == 0)
    {
        skip = (int)(floor( (60.0f-titleTimer) / 10.0f));
    }

    draw_rotated_bitmap_area(bmpDollars,(int)(64 + sin(angle)*64) % 128,(int)(64 + cos(angle)*64) % 128,skip,angle);

    float scale = 1.0f + sin(angle*5)*0.05f;
    int y = 0;
    if(titlePhase == 0)
    {
        y = (int) ( -64 + 64.0f/60.0f * titleTimer);
    }
    else if(titlePhase == 2)
    {
        y = (int) ( -64 + 64.0f/60.0f * (60.0f-titleTimer));
    }
    draw_scaled_bitmap_region(bmpLogo,0,0,128,96,-128*(scale-1.0f)/2,y -96*(scale-1.0f)/2,128*scale,96*scale);

    if(titlePhase == 1 && (int)floor((angle*100)/30.0f) %2 == 0)
    {
        draw_text(bmpFont,(Uint8*)"Press Any Key",13,64,72,-1,0,true);
    }
}

/// Get title scene
SCENE get_title_scene()
{
    SCENE s = (SCENE){title_init,title_update,title_draw,NULL};

    return s;
}