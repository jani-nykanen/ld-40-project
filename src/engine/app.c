/// Main file
/// (c) 2017 Jani Nykänen

#include "app.h"

#include <stdio.h>
#include <stdbool.h>

#include "controls.h"
#include "graphics.h"
#include "assets.h"
#include "audio.h"

#include "stdlib.h"
#include "math.h"
#include "stdio.h"

/// Is application app_running
static bool isRunning;
/// Is full screen
static bool isFullscreen;

/// Window
static SDL_Window* window;
/// Renderer
static SDL_Renderer* rend;
/// Canvas
static FRAME* canvas;

/// (Timer) old ticks
static int oldTicks;
/// (Timer) new ticks
static int newTicks;
/// (Timer) delta time
static int deltaTime;

/// Canvas pos
static SDL_Point canvasPos;
/// Canvas size
static SDL_Point canvasSize;

/// Current scene
static SCENE currentScene;
/// Global scene
static SCENE globalScene;
/// Scenes
static SCENE scenes[16];
/// Scene count
static Uint8 sceneCount;

/// Wait time
static int frame_wait;

/// Configuration
static CONFIG config;

/// Joystick
static SDL_Joystick* joy;

/// Calculate canvas size and position on screen
/// < winWidth Window width
/// < winHeight Window height
static void app_calc_canvas_prop(int winWidth, int winHeight)
{
    // If aspect ratio is bigger or equal to the ratio of the canvas
    if((float)winWidth/(float)winHeight >= (float)config.canvasWidth/ (float)config.canvasHeight )
    {
        canvasSize.y = winHeight;
        canvasSize.x = (int) ( (float)winHeight / (float) config.canvasHeight  * config.canvasWidth);

        canvasPos.x = winWidth/2 - canvasSize.x/2;
        canvasPos.y = 0;
    }
    else
    {
        canvasSize.x = winWidth;
        canvasSize.y =  (int) ( (float)canvasSize.x / (float) config.canvasWidth  * config.canvasHeight );

        canvasPos.y = winHeight/2 - canvasSize.y/2;
        canvasPos.x = 0;
    }
}

/// Initialize SDL
/// > An error code, 0 on success, 1 on error
static int app_init_SDL()
{   
    // Init
    if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        printf("Failed to init SDL!\n");
        return 1;
    }

    // Create window

    int windowWidth = config.winWidth;
    int windowHeight = config.winHeight;

    set_dimensions(windowWidth,windowHeight);

    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				              windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
	if(window == NULL)
	{
		printf("Failed to create an SDL window!\n");
        return 1;
	}

    isFullscreen = config.fullscreen == 1;
    // app_toggle_fullscreen();

    // Create renderer
    rend = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(rend == NULL)
    {
        printf("Failed to create an SDL renderer!\n");
        return 1;
    }

    // Hide mouse cursor
    SDL_ShowCursor(0);

    // Open joystick
    joy = SDL_JoystickOpen(0);

    // Init audio
    if(audio_init() == 1)
    {
        return 1;
    }

    return 0;
}

/// Toggle fullscreen mode
void app_toggle_fullscreen()
{
    SDL_SetWindowFullscreen(window,!isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	isFullscreen = !isFullscreen;
}

/// Initialize application
/// > An error code, 0 on success, 1 on error
static int app_init(SCENE* arrScenes, int count, const char* assPath)
{
    // Init SDL
    if(app_init_SDL() != 0)
    {
        return 1;
    }

    // Load assets
    if(load_assets(config.assPath) != 0)
    {
        return 1;
    }

    // Set global renderer & init graphics
    init_graphics();
    set_global_renderer(rend);

    // Gen palette
    fr_gen_palette();

    // Create frame
    canvas = frame_create(config.canvasWidth,config.canvasHeight);
    if(canvas == NULL)
    {
        return 1;
    }
    bind_frame(canvas);

    // Calculate canvas pos & size
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    app_calc_canvas_prop(w,h);

    // Copy scenes to a scene array
    // and initialize them
    sceneCount = count;
    int i =0;
    for(; i < count; i++)
    {
        scenes[i] = arrScenes[i];
        if(scenes[i].on_init != NULL)
        {
            if(scenes[i].on_init() == 1)
            {
                return 1;
            }
        }

        if(strcmp(scenes[i].name,"global") == 0)
        {
            globalScene = scenes[i];
        }
    }

    // Make the first scene the current scene
    if(sceneCount > 0)
        currentScene = scenes[0];

    // Calculate frame wait value
    frame_wait = (int) floor (1000.0f / (float) config.fps);

    isRunning = true;

    return 0;
}

/// Go through events
static void app_events()
{
    SDL_Event event;

    // Go through every event
    while (SDL_PollEvent(&event) != 0)
    {
        switch(event.type)
        {
        // Application quit
        case SDL_QUIT:
            isRunning = false;
            return;

        // Window event (resize etc)
        case SDL_WINDOWEVENT:
            // Resize
            if(event.window.windowID == SDL_GetWindowID(window) && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                app_calc_canvas_prop(event.window.data1,event.window.data2);
            }
            break;
        
        // Key down event
        case SDL_KEYDOWN:
            ctr_on_key_down(event.key.keysym.scancode);
            break;
        // Key up event
        case SDL_KEYUP:
            ctr_on_key_up(event.key.keysym.scancode);
            break;

        // Mouse down event
        case SDL_MOUSEBUTTONDOWN:
            ctr_on_mouse_down((int)event.button.button);
            break;
        // Key up event
        case SDL_MOUSEBUTTONUP:
            ctr_on_mouse_up((int)event.button.button);
            break;
        // Mouse move enent
        case SDL_MOUSEMOTION:
            ctr_on_mouse_move(event.motion.x,event.motion.y);
            break;
        // Mouse wheel event
        case SDL_MOUSEWHEEL:
            ctr_on_mouse_wheel(event.wheel.y);
            break;

        // Joy button down
        case SDL_JOYBUTTONDOWN:
            ctr_on_joy_down(event.jbutton.button);
            break;

        // Joy button up
        case SDL_JOYBUTTONUP:
            ctr_on_joy_up(event.jbutton.button);
            break;

        // Joy axis
        case SDL_JOYAXISMOTION:
        {
            int axis = 0;
            if(event.jaxis.axis == 0)
                axis = 0;
            else if(event.jaxis.axis == 1)
                axis = 1;
            else 
                break;

            float value = (float)event.jaxis.value / 32767.0f;

            ctr_on_joy_axis(axis,value);
            
            break;
        }

        // Joy hat
        case SDL_JOYHATMOTION:
        {
            int v = event.jhat.value;
            VEC2 stick = vec2(0.0f,0.0f);
            if(v == SDL_HAT_LEFTUP || v == SDL_HAT_LEFT || v == SDL_HAT_LEFTDOWN)
            {
                stick.x = -1.0f;
            }

            if(v == SDL_HAT_RIGHTUP || v == SDL_HAT_RIGHT || v == SDL_HAT_RIGHTDOWN)
            {
                stick.x = 1.0f;
            }

            if(v == SDL_HAT_LEFTUP || v == SDL_HAT_UP || v == SDL_HAT_RIGHTUP)
            {
                stick.y = -1.0f;
            }

            if(v == SDL_HAT_LEFTDOWN || v == SDL_HAT_DOWN || v == SDL_HAT_RIGHTDOWN)
            {
                stick.y = 1.0f;
            }

            ctr_on_joy_axis(0,stick.x);
            ctr_on_joy_axis(1,stick.y);

            break;
        }

        default:
            break;
        }
    }

}   

/// Update application
/// < delta Delta time in milliseconds
static void app_update(Uint32 delta)
{
    float tm = (float)((float)delta/1000.0f) / (1.0f/60.0f);
    /// Limit tm (in other words, limit minimum fps)
    if(tm > 5.0) tm = 5.0;

    // Quit
    if(get_key_state(SDL_SCANCODE_LCTRL) == DOWN &&
       get_key_state(SDL_SCANCODE_Q) == PRESSED)
    {
        isRunning = false;
        return;
    }

    // Full screen
    if( (get_key_state(SDL_SCANCODE_LALT) == DOWN &&
       get_key_state(SDL_SCANCODE_RETURN) == PRESSED) ||
       get_key_state(SDL_SCANCODE_F4) == PRESSED )
    {
        app_toggle_fullscreen();
    }

    // Update current & global scenes
    if(currentScene.on_update != NULL)
    {
        currentScene.on_update(tm);
    }
    if(globalScene.on_update != NULL)
    {
        globalScene.on_update(tm);
    }

    ctr_update();

}

/// Draw application
static void app_draw()
{
    // Clear to black
    clear(0,0,0);

    // Draw global & current scenes
    if(currentScene.on_draw != NULL)
    {
        currentScene.on_draw();
    }
    if(globalScene.on_draw != NULL)
    {
        globalScene.on_draw();
    }

    // Update frame texture
    frame_update_tex(canvas);

    // Draw frame
    SDL_Rect dest = (SDL_Rect){canvasPos.x,canvasPos.y,canvasSize.x,canvasSize.y};
    SDL_RenderCopy(rend,canvas->tex,NULL,&dest);

    SDL_RenderPresent(rend);
}

/// Destroy application
static void app_destroy()
{
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);

    SDL_JoystickClose(joy);
}

/// Swap scene
void app_swap_scene(const char* name)
{
    int i = 0;
    for(; i < sceneCount; i++)
    {
        if(strcmp(scenes[i].name,name) == 0)
        {
            currentScene = scenes[i];
            return;
        }
    }

}

/// Terminate application
void app_terminate()
{
    isRunning = false;
}

/// Get main frame
FRAME* app_get_canvas()
{
    return canvas;
}

/// Run application
/// > An error code, 0 on success, 1 on error
int app_run(SCENE* arrScenes, int count, CONFIG c)
{
    config = c;

    if(app_init(arrScenes,count,NULL) != 0) return 1;

    while(isRunning)
    {
        oldTicks = SDL_GetTicks();
        app_events();
        app_update(deltaTime);
        app_draw();
        newTicks = SDL_GetTicks();

        // Wait
        int deltaMilliseconds = (newTicks - oldTicks);
        int restTime = (int) (frame_wait-1) - (int)deltaMilliseconds;
        if (restTime > 0) 
            SDL_Delay((unsigned int) restTime);

        // Set delta time
        deltaTime = SDL_GetTicks() - oldTicks;;

    }
    app_destroy();

    return 0;
}
