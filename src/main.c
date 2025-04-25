#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"

bool is_running;


void setup(void) {
    // Allocate required bytes for the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    // Create a SDL Texture for the color display
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    // if (!color_buffer) {
    // not successful
    // }
}


void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT: 
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

void update(void) {

}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 1, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_grid_as_lines(30);
    draw_rect(300, 200, 300, 150, 0xFFFF00FF);

    render_color_buffer();
    // clear_color_buffer(0xFFFFFF00);

    SDL_RenderPresent(renderer);
}

int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}