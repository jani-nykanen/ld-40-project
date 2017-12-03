/// Game scene (source)
/// (c) 2017 Jani Nykänen

#include "game.h"

#include "../engine/app.h"
#include "../engine/graphics.h"
#include "../engine/bitmap.h"
#include "../engine/controls.h"
#include "../engine/assets.h"
#include "../engine/tilemap.h"
#include "../engine/audio.h"
#include "../engine/transform.h"
#include "../engine/mesh.h"

#include "../vpad.h"

#include "stage.h"
#include "player.h"
#include "obstacle.h"
#include "coin.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

/// Player
static PLAYER pl;
/// Obstacles
#define OBS_COUNT 8
static OBSTACLE obstacles[OBS_COUNT];
/// Coins
#define COIN_COUNT 12
static COIN coins[COIN_COUNT];
/// Obstacle timer
static float interval;
static float obsTimer;
/// Fish timer
static float fishTimer;
/// Phase
static int phase;

/// Bitmap font
static BITMAP* bmpFont;
/// Bitmap font 2
static BITMAP* bmpFont2;
/// Heart bitmap
static BITMAP* bmpHeart;
/// Game over bitmap
static BITMAP* bmpGameOver;

/// Game over frame
static FRAME* goverFrame;
/// Is the game over
static bool gameOver;
/// Game over timer
static float goverTimer;
/// Draw money text
static bool drawMoney;
/// Hint index
static int hintIndex;

/// Push obstacle to the game world
static void push_obs()
{
    int i = 0;
    for(; i < OBS_COUNT; i++)
    {
        if(obstacles[i].exist == false)
        {
            push_obstacle(&obstacles[i],rand() % 3);
            return;
        }
    }
}

/// Push coins
static void push_coins()
{   
    if(rand() % 3 == 0) return;

    float posy = 24 + (float) (rand() % 48) ;
    float posx = 128 + (float) (rand() % 24 + 16);

    int loop = 0;
    int loopMax = rand() % 2 + 1;
    if(rand() % 2 == 0) loopMax ++;
    if(rand() % 2 == 0) loopMax ++;
    int i = 0;

    int dist = loopMax == 2 ? 16 : 12;

    for(; loop < loopMax; loop++ )
    {
        for(i=0; i < COIN_COUNT; i++)
        {
            if(coins[i].exist == false)
            {
                push_coin(&coins[i],posx + (float)loop*dist, posy );
                break;
            }
        }
    }
}

/// Push some nice fish
static void push_fish()
{
    int i = 0;
    for(; i < OBS_COUNT; i++)
    {
        if(obstacles[i].exist == false)
        {
            push_obstacle(&obstacles[i], 3);
            return;
        }
    }
}

/// Reset game
static void game_reset()
{
    init_stage();
    pl = create_player();
    int i = 0;
    for(; i < OBS_COUNT; i++)
        obstacles[i] = create_obstacle();
    for(i=0; i < COIN_COUNT; i++)
        coins[i] = create_coin();

    // Set default values
    phase = 0;
    interval = 90.0f;
    obsTimer = interval;
    fishTimer = interval * (0.5f + (float)(rand() % 3 + 1));
    gameOver = false;
    goverTimer = 0.0f;
}

/// Init game
static int game_init()
{
    /// Initialize components
    init_stage();
    pl = create_player();
    int i = 0;
    for(; i < OBS_COUNT; i++)
        obstacles[i] = create_obstacle();
    for(i=0; i < COIN_COUNT; i++)
        coins[i] = create_coin();

    // Set default values
    phase = 0;
    interval = 90.0f;
    obsTimer = interval;
    fishTimer = interval * (0.5f + (float)(rand() % 3 + 1));
    gameOver = false;
    hintIndex = 0;
    goverTimer = 0.0f;

    // Get bitmaps
    bmpFont = get_bitmap("font");
    bmpHeart = get_bitmap("heart");
    bmpGameOver = get_bitmap("gameover");
    bmpFont2 = get_bitmap("font2");

    // Create frame
    goverFrame = frame_create(128,96);
    if(goverFrame == NULL) return 1;

    srand(time(NULL));

    return 0;
}

/// Forward declaration
static void game_draw();
/// Update game
/// tm Time multiplier
static void game_update(float tm)
{
    // Update game over screne
    if(gameOver)
    {
        if(goverTimer > 0.0f)
            goverTimer -= 1.0f * tm;
        else
        {
            if(any_pressed())
            {
                gameOver = false;
                game_reset();
            }
        }

        return;
    }

    // Update components
    update_stage(&pl,tm);
    pl_update(&pl,tm);
    int i = 0;
    for(; i < 8; i++)
        ob_update(&obstacles[i],&pl,tm);
    for(i=0; i < 8; i++)
        coin_update(&coins[i],&pl,tm);

    // Update obstacle timer
    float speed = get_global_speed();
    obsTimer -= speed * tm;
    // Create a new obstacle
    if(obsTimer <= 0.0f)
    {
        obsTimer += interval;
        push_obs();
        push_coins();
    }
    
    // Update fish timer
    fishTimer -= speed * tm;
    if(fishTimer <= 0.0f)
    {
        int min = (phase < 2) ? 3-phase : 1; 
        int max = (phase < 2) ? 5 : ((phase < 5) ? 5-phase : 0);
        fishTimer += interval * ( (max > 0 ? rand() % max : 0 ) + min);
        push_fish();
    }

    // Set phase & interval
    phase = (int)floor(pl.money / 10);
    interval = 100.0f - phase*7.5f;

    // Game over?
    drawMoney = true;
    if(pl.health <= 0)
    {
        
        drawMoney = false;
        game_draw();
        copy_frame(get_current_frame(),goverFrame);
        drawMoney = true;
        goverTimer = 30.0f;
        gameOver = true;
        hintIndex = rand () % 3;
    }

}

/// Draw game
static void game_draw()
{
    clear_frame(0b00101010);

    if(gameOver)
    {
        draw_inverted_bitmap((BITMAP*)goverFrame,0,0,0);

        if(goverTimer > 0.0f)
        {
            int skip = (int)floor(goverTimer / 5.0f) +1;
            draw_skipped_bitmap_region((BITMAP*)goverFrame,0,0,128,96,0,0,skip,skip,0);
        }
        else
        {
            draw_bitmap(bmpGameOver,0,16,0);

            // The Ludum Dare way to do these things:
            if(hintIndex == 0)
                draw_text(bmpFont2,(Uint8*)"HINT: Collect\nmore money!",64,16,48,-1,12,false);
            else if(hintIndex == 1)
                draw_text(bmpFont2,(Uint8*)"HINT: Want to win?\nCollect money!",64,1,48,-1,12,false);
            else if(hintIndex == 2)
                draw_text(bmpFont2,(Uint8*)"HINT: Money is\nyour goal!",64,12,48,-1,12,false);
        }

        return;
    }

    // Draw components
    draw_stage();
    int i = 0;
    for(; i < COIN_COUNT; i++)
    {
        coin_draw(&coins[i]);
    }
    for(i=0; i < OBS_COUNT; i++)
    {
        if(obstacles[i].type != O_FISH) ob_draw(&obstacles[i]);
    }
    for(i=0; i < OBS_COUNT; i++)
    {
        if(obstacles[i].type == O_FISH) ob_draw(&obstacles[i]);
    }
    pl_draw(&pl);

    // Draw money
    if(drawMoney)
    {
        char moneyStr[32];
        snprintf(moneyStr,32,"MONEY: $%d",pl.money);
        draw_text(bmpFont,(Uint8*)moneyStr,32, 24,2, 0,0, false);

        // Draw hearts
        for(i=0; i < pl.health; i++)
        {
            draw_bitmap(bmpHeart,1,2 + i*13,0);
        }
    }
}

/// Destroy game
static void game_destroy()
{
    destroy_assets();
}

/// Get game scene
SCENE get_game_scene()
{
    // Set scene functions
    SCENE s = (SCENE){game_init,game_update,game_draw,game_destroy};

    // Set scene name
    set_scene_name(&s,"game");

    return s;
}
