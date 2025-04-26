#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)  // 33.3ms

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture; // used to display color buffer

extern int window_width;
extern int window_height;
extern uint32_t grid_color;


bool initialize_window(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_grid_as_dots(int grid_size);
void draw_grid_as_lines(int grid_size);
void render_color_buffer(void);
void destroy_window(void);
void clear_color_buffer(uint32_t color);
int place_in_buffer(int x, int y);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif