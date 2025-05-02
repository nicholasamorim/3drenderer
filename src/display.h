#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "triangle.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)  // 33.3ms

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* color_buffer;
extern float* z_buffer;

enum render_modes {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_WIRE_SOLID,
    RENDER_SOLID,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
};

enum cull_modes {
    CULL_NONE, 
    CULL_BACKFACE
};


extern SDL_Texture* color_buffer_texture; // used to display color buffer
extern uint32_t grid_color;

int get_window_width(void);
int get_window_height(void);
bool initialize_window(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_grid_as_dots(int grid_size);
void draw_grid_as_lines(int grid_size);
void render_color_buffer(void);
void destroy_window(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer(void);
int place_in_buffer(int x, int y);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_wireframe(triangle_t triangle, uint32_t color);
void draw_vertex_points(triangle_t triangle, uint32_t color);
float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float value);
#endif