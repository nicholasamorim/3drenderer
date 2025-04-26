#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;


bool setup(void) {
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

    if (!color_buffer) {
        fprintf(stderr, "Cannow create color buffer");
        return false;
    }

    return true;
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

vec2_t project(vec3_t point) {
    vec2_t projected_point = { 
        (fov_factor * point.x) / point.z, 
        (fov_factor * point.y) / point.z 
    };    
    return projected_point;
}


void update(void) {
    /*
      We still have a small issue.
      Right now, if we increase our FPS the game objects
      will move faster, and if we decrease the FPS our 
      game objects will run slower. 
      
      This is fine for now, but we will come back to 
      this discussion of controlling our framerate very soon. 
      We'll learn about something called "variable delta-time" 
      which will help us get framerate-independent movement.
    */

    // Wait until FRAME_TARGET_TIME passes ensuring consistent FPS
    // int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    
    // // Only delay if we're running fast and coming in hot!
    // if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    //     SDL_Delay(time_to_wait);
    // }
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
    
    // Time since SDL_Init wa called in ms
    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    // Loop over triangle faces
    for (int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];
        
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;
    
        for (int j = 0; j < 3; j++) {
                vec3_t transformed_vertex = face_vertices[j];

                transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
                transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
                transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

                transformed_vertex.z -= camera_position.z;
                
                vec2_t projected_point = project(transformed_vertex);

                // Scale and translarte the proj point to the middle of the screen
                projected_point.x += (window_width / 2);
                projected_point.y += (window_height / 2);

                projected_triangle.points[j] = projected_point;
        }
    
        // Save projected triangle in the array of triangles to render
    triangles_to_render[i] = projected_triangle;
    }

}


void render(void) {
    draw_grid_as_lines(10);
    
    // looop projected points and render
    // for (int i = 0; i < N_MESH_FACES; i++) {
    //     triangle_t triangle = triangles_to_render[i];
    //     draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
    //     draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
    //     draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
    // }

    draw_line(100, 200, 300, 50, 0xFF00FF00);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}


int main(void) {
    is_running = initialize_window();

    if  (!setup()) {
        return 1;
    }

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}