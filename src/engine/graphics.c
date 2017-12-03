/// Graphics (source)
/// (c) 2017 Jani Nykänen

#include "graphics.h"

#include "mathext.h"
#include "transform.h"

#include "malloc.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"

/// Triangle type
typedef struct
{
    VEC3 A,B,C; /// Vertices
    Uint8 color; /// Color
    float depth; /// Depth value
    VEC3* normal; /// Normal
}
_TRIANGLE;

/// Min delta
static const float DEPTH_MIN = 0.025f;
/// Size of triangle buffer
#define TBUFFER_SIZE 1024
// Maximum darkness values
static const int MAX_DARKNESS_VALUE = 6;

/// Global renderer
static SDL_Renderer* grend;
/// Global frame
static FRAME* gframe;
/// Window dim
static SDL_Point windowDim;

/// Translate x
static int transX;
/// Translate y
static int transY;

/// Global texture used in drawing filled polygons
static BITMAP* gtex;

/// Is darkness enabled
static bool darkness;
/// Darkness value
static int dvalue;
/// Darkness begin
static float darkBegin;
/// Darkness end
static float darkEnd;
/// Darkness step
static float darkStep;

/// Put pixel function
static void (*ppfunc) (int,int,Uint8);

/// Put pixel to the screen
/// < x X coordinate
/// < y Y coordinate
/// < index Color index
static void put_pixel(int x, int y, Uint8 index)
{
    if(index == 255 || x < 0 || y < 0 || x >= gframe->w || y >= gframe->h) return;
    gframe->colorData[y*gframe->w+x] = index;
}

/// Put pixel to the screen (with darkness enabled)
/// < x X coordinate
/// < y Y coordinate
/// < index Color index
static void put_pixel_dark(int x, int y, Uint8 index)
{
    if(index == 255 || x < 0 || y < 0 || x >= gframe->w || y >= gframe->h) return;

    if(darkness && dvalue > 0)
    {
        Uint8 col;
        if(dvalue % 2 == 0)
        {
            col = index  + (dvalue+1)/2 *64;
        }
        else
        {
            if( (x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1) )
                col = index  + (dvalue)/2 *64;
            else
                col = index  + (dvalue+2) / 2 *64;
        }
        gframe->colorData[y*gframe->w+x] = col;
        return;
    }

    gframe->colorData[y*gframe->w+x] = index;
}

/// Initialize graphics
void init_graphics()
{
    ppfunc = put_pixel;
}

/// Set global renderer
void set_global_renderer(SDL_Renderer* rend)
{
    grend = rend;
}

/// Get grend
SDL_Renderer* get_global_renderer()
{
    return grend;
}

/// Clear screen
void clear(unsigned char r, unsigned char g, unsigned char b)
{
    SDL_SetRenderDrawColor(grend, r,g,b, 255);
    SDL_RenderClear(grend);
}

/// Set window dimensions
void set_dimensions(int w, int h)
{
    windowDim.x = w;
    windowDim.y = h;
}

/// Get window dimensions
SDL_Point get_dimensions()
{
    return windowDim;
}

/// Bind frame
void bind_frame(FRAME* fr)
{
    gframe = fr; 
}

/// Return currently used frame
FRAME* get_current_frame()
{
    return gframe;
}

/// Clear frame
void clear_frame(Uint8 index)
{
    memset(gframe->colorData,index,gframe->size);
}

/// Draw a non-scaled bitmap
void draw_bitmap(BITMAP* b, int dx, int dy, int flip)
{
    int x; // Screen X
    int y = dy; // Screen Y
    int px = 0; // Pixel X
    int py = 0; // Pixel Y

    // Go though every pixel and put them to a frame
    for(; y < dy+b->h; y++)
    {
        for(x = dx; x < dx+b->w; x++)
        {
            ppfunc(x,y, b->data[py*b->w +px]);
            px ++;
        }
        py ++;
        px = 0;
    } 
}

/// Draw a non-scaled bitmap with inverted colors
void draw_inverted_bitmap(BITMAP* b, int dx, int dy, int flip)
{
    int x; // Screen X
    int y = dy; // Screen Y
    int px = 0; // Pixel X
    int py = 0; // Pixel Y

    Uint8 index;

    // Go though every pixel and put them to a frame
    for(; y < dy+b->h; y++)
    {
        for(x = dx; x < dx+b->w; x++)
        {
            index = b->data[py*b->w +px];
            index = ~index;
            index = index & 0b00111111;

            ppfunc(x,y, index);
            px ++;
        }
        py ++;
        px = 0;
    } 
}

/// Draw a rotated bitmap area
void draw_rotated_bitmap_area(BITMAP* b,  float trx, float try, float angle)
{
    // Rotation matrix B
    float b11 = cos(angle), b21 = -sin(angle);
    float b12 = sin(angle), b22 = cos(angle);

    // Inverse of determinant of B
    float detInv = 1.0f / (b11 * b22 - b12 * b21);

    // The inverse of matrix B
    float ib11 = detInv * (b22), ib21 = detInv * -b21;
    float ib12 = detInv * -(b12), ib22 = detInv * b22;

    // Texture coordinates in bitmap
    int tx = 0;
    int ty = 0;

    // "gframe" stands for "global frame",
    // so cx and cy point to the center of
    // the frame where this thing is drawn to
    int cx = gframe->w / 2;
    int cy = gframe->h / 2;

    // Translated coordinates
    int xx, yy;

    // Coordinates
    int x,y;

    // Color, obviously
    Uint8 color;

    // Draw pixels
    for(x = 0; x < gframe->w; x++)
    {
        for(y = 0; y < gframe->h; y++)
        {
            // Translate point
            xx = x - cx;
            yy = y - cy;

            // Get texture coordinates
            tx = (int)(ib11 * xx + ib21 * yy +trx);
            ty = (int)(ib12 * xx + ib22 * yy +try);

            // If texture coords are outside the
            // texture area, force them back!
            while(tx >= b->w) tx -= b->w;
            while(ty >= b->h) ty -= b->h;
            while(tx < 0) tx += b->w;
            while(ty < 0) ty += b->h;

            color = b->data[ty*b->w +tx];
            ppfunc(x,y, color);
        }
    } 
}

/// Draw a bitmap region
void draw_bitmap_region(BITMAP* b, int sx, int sy, int sw, int sh, int dx, int dy, int flip)
{
    dx += transX;
    dy += transY;

    int x; // Screen X
    int y = dy; // Screen Y
    int px = sx; // Pixel X
    int py = sy; // Pixel Y

    int beginx = flip == FLIP_NONE ? dx : dx+sw-1;
    int endx = flip == FLIP_NONE ? dx+sw : dx;
    int stepx = flip == FLIP_NONE ? 1 : -1;

    // Go though every pixel and put them to a frame
    for(; y < dy+sh; y++)
    {
        for(x = beginx; x != endx; x += stepx)
        {
            ppfunc(x,y, b->data[py*b->w +px]);

            px ++;
        }
        py ++;
        px = sx;
    } 
}

/// Draw a skipped bitmap region
void draw_skipped_bitmap_region(BITMAP* b, int sx, int sy, int sw, int sh, int dx, int dy, int skipx, int skipy, int flip)
{

    dx += transX;
    dy += transY;

    int x; // Screen X
    int y = dy; // Screen Y
    int px = sx; // Pixel X
    int py = sy; // Pixel Y

    int beginx = flip == FLIP_NONE ? dx : dx+sw-1;
    int endx = flip == FLIP_NONE ? dx+sw : dx;
    int stepx = flip == FLIP_NONE ? (1) : -(1);

    int skipxCount = 0;
    int skipyCount = 0;

    // Go though every pixel and put them to a frame
    for(; y < dy+sh; y++)
    {
        skipxCount = 0;
        for(x = beginx; x != endx; x += stepx)
        {
            
            if(skipx == 0 || (skipxCount % skipx != 0 && (skipy == 0 || skipyCount % skipy != 0) ))
            {
                ppfunc(x,y, b->data[py*b->w +px]);
            }

            px ++;
            skipxCount ++;
        }
        skipyCount ++;
        py ++;
        px = sx;
    } 
}

/// Draw a scaled bitmap line
void draw_scaled_bitmap_region(BITMAP* b, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
    int x; // Screen X
    int y = dy; // Screen Y
    int px = sx; // Pixel X
    int py = sy; // Pixel Y
    float pxf = (float)sx; // Pixel X (float)
    float pyf = (float)sy; // Pixel Y (float)

    float ssx = (float)dw/(float)sw;
    float ssy = (float)dh/(float)sh;

    // Go though every pixel and put them to a frame
    // if pixel is not out of frame range
    for(; y < dy+ (int)floor(dh ); y++)
    {
        for(x = dx; x < dx+ (int)floor(dw ); x++)
        {
            if(x < -1)
            {
                pxf += 1.0f/ssx * abs(x);
                x = -1;
                continue;
            }
            else if(x >= gframe->w)
                break;

            px = (int)(pxf);
            py = (int)(pyf);

            ppfunc(x,y, b->data[py*b->w +px]);
            pxf += 1.0f/ssx;
        }
        pyf += 1.0f/ssy;
        pxf = (float)sx;
    } 
}

/// Draw text using a bitmap font
void draw_text(BITMAP* b, Uint8* text, int len, int dx, int dy, int xoff, int yoff, bool center)
{
    int x = dx;
    int y = dy;
    int cw = b->w / 16;
    int ch = b->h / 16;
    int i=0;
    Uint8 c;
    int sx;
    int sy;

    if(center)
    {
        x -= (len+1)/2 * (cw+xoff);
    }

    for(; i < len && text[i] != '\0'; i++)
    {
        c = text[i];
        if(c == '\n')
        {
            x = dx;
            y += yoff;
            continue;
        }

        sx = c % 16;
        sy = c / 16;

        draw_bitmap_region(b,sx*cw,sy*ch,cw,ch,x,y,0);

        x += cw + xoff;
    }
}

/// Fill rectangle
void fill_rect(int x, int y, int w, int h, Uint8 index)
{
    x += transX;
    y += transY;

    int dx = x;
    int dy = y;

    for(; dy < y+h; dy++)
    {
        for(dx = x; dx < x+w; dx++)
        {
            ppfunc(dx,dy,index);
        }
    }
}

/// Draw a line
void draw_line(int x1, int y1, int x2, int y2, Uint8 color)
{

    // Bresenham's line algorithm
    int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = abs(y2-y1), sy = y1<y2 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;
     
    for(;;)
    {
        ppfunc(x1,y1, color);
        
        if (x1==x2 && y1==y2) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }
}


/// Draw a textured triangle
static void _draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, Uint8 col, int spc)
{
    // Calculate minimums & maximums
    int maxy = max(y1,max(y2,y3));
    int miny = min(y1,min(y2,y3));
    int maxx = max(x1,max(x2,x3));
    int minx = min(x1,min(x2,x3));

    // Do not draw if not visible
    if(maxx < 0 || minx >= gframe->w || maxy < 0 || miny >= gframe->h 
        || (maxy == miny)) 
        return;

    // Sort points
    int py1 = miny;
    int px1 = y1 == py1 ? x1 : (y2 == py1 ? x2 : x3);
    int px2,py2;
    int px3, py3;
    if(py1 == y1 && px1 == x1)
    {
        px2 = x2; py2 = y2;
        px3 = x3; py3 = y3;
    }
    else if(py1 == y2 && px1 == x2)
    {
        px2 = x1; py2 = y1;
        px3 = x3; py3 = y3;
    }
    else
    {
        px2 = x1; py2 = y1;
        px3 = x2; py3 = y2;
    }

    // Swap p2 & p3 if certain conditions are met
    if( (spc == 0 && px3 < px2) || spc == 2)
    {
        int bx = px3;
        int by = py3;
        px3 = px2;
        py3 = py2;
        px2 = bx;
        py2 = by;
    }

    // Calculate steps
    float step1 = (py2 != py1) ? (float) (px2 - px1) / (float) (py2 - py1) : (px2-px1);
    float step2 = (py3 != py1) ? (float) (px3 - px1) / (float) (py3 - py1) : (px3-px1);
    float step3 = (py3 != py2) ? (float) (px3 - px2) / (float) (py3 - py2) : (px3-px2);

    int x,y;
    float startx = px1;
    float endx = px1;

    // Is the top or bottom flat
    bool flat = py1 == py2 || py2 == py3 || py1 == py3;

    // Draw visible pixels
    for(y = miny; y <= min(maxy,gframe->h); y++)
    {
        if(y >= 0)
        {
            for(x = max(0,(int)startx); x <= min(gframe->w,(int)endx); x++)
            {
                ppfunc(x,y,col);
            }
        }

        if(!flat || y > miny)
        {
            if(y >= py3)
                step2 = step3;
            if(y >= py2)
                step1 = step3;
        }

        startx += step1;
        endx += step2;

        if(spc != 2 && startx-endx > 1)
        {
            _draw_triangle(px1,py1,px2,py2,px3,py3,col,spc+1);
            return;
        }
    }
}
/// Draw triangle (as defined in the header)
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, Uint8 col)
{
    _draw_triangle(x1,y1,x2,y2,x3,y3,col,0);
}

/// Set translation
void set_translation(int x, int y)
{
    transX = x;
    transY = y;
}

/// Set darkness
/// < enable Enable darkness
/// < start Start depth value
/// < end End depth value
void set_darkness(bool enable, float start, float end)
{
    darkness = enable;
    if(enable)
    {
        darkBegin = start;
        darkEnd = end;
        darkStep = (darkEnd-darkBegin) / 3.0f;
        ppfunc = put_pixel_dark;
    }
    else
    {
        ppfunc = put_pixel;
    }
    
    dvalue = 0;
}

/// Bind a texture
void bind_texture(BITMAP* tex)
{
    gtex = tex;
}