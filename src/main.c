#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "color.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"

// Array of triangles to render frame by frame
// pointer in memory to the first position of array
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { 0, 0, 0 };
mat4_t projection_matrix;

bool is_running = false;
int previous_frame_time = 0;

enum render_modes {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_WIRE_SOLID,
    RENDER_SOLID,
};

enum cull_modes {
    CULL_NONE, 
    CULL_BACKFACE
};

enum render_modes render_mode = RENDER_WIRE;
enum cull_modes cull_mode = CULL_BACKFACE;


bool backface_culling =  true;

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

    float fov = M_PI / 3.0; // same as 180/3 or 60deg
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    projection_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    load_obj_file_data("./assets/cube.obj");

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
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }
            else if (event.key.keysym.sym == SDLK_1) 
                render_mode = RENDER_WIRE_VERTEX;
                else if (event.key.keysym.sym == SDLK_2) 
                render_mode = RENDER_WIRE;
                else if (event.key.keysym.sym == SDLK_3) 
                render_mode = RENDER_SOLID;
                else if (event.key.keysym.sym == SDLK_4) 
                render_mode = RENDER_WIRE_SOLID;
            else if (event.key.keysym.sym == SDLK_c) {
                cull_mode = CULL_BACKFACE;
            }
            else if (event.key.keysym.sym == SDLK_d) {
                cull_mode = CULL_NONE;
            };
            break;
    }
}


void sort_by_depth(triangle_t* array) {
    int num_triangles = array_length(array);

    // Bubble sort by avg depth
    for (int i = 0; i < num_triangles; ++i) {
        for (int j = i; j < num_triangles; j++) {
            if (array[i].avg_depth < array[j].avg_depth) {
                triangle_t temp = triangles_to_render[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
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
    
    // Time since SDL_Init was called in ms
    previous_frame_time = SDL_GetTicks();

    // Init array of triangles for the frame
    triangles_to_render = NULL;

    // Test transformations
    mesh.rotation.x += 0.01;
    // mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.01;
    // mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;
    // mesh.scale.x += 0.0002;
    // mesh.scale.y += 0.0001;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_x(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_x(mesh.rotation.z);

    // Loop over triangle faces
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];
    
        // Apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
            
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
            transformed_vertices[j] = transformed_vertex;
        }

        if (cull_mode == CULL_BACKFACE) {
            vec3_t a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t c = vec3_from_vec4(transformed_vertices[2]);

            vec3_t b_a = vec3_sub(b, a);
            vec3_t c_a = vec3_sub(c, a);
            vec3_normalize(&b_a);
            vec3_normalize(&c_a);

            // handiness has to do with the order, the order matters
            // this case is left handiness. Z is positive as it goes
            // further and further away from the camera
            vec3_t normal = vec3_cross(b_a, c_a);
            vec3_normalize(&normal);

            vec3_t camera_ray = vec3_sub(camera_position, a);
            // if the normal is pointing away from the camera, this will
            // return a negative value
            float dot_normal_camera = vec3_dot(normal, camera_ray);

            if (dot_normal_camera < 0 ) {
                continue;  // do not render if projected away from camera
            }
        }

        vec4_t projected_points[3];
        for (int j = 0; j < 3; j++) {
            projected_points[j] = mat4_mul_vec4_project(projection_matrix, transformed_vertices[j]);

            // Scale into view
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);

            // Translate the proj point to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        // Calculate average depth for each face based on their Z values
        float avg_depth = (
            transformed_vertices[0].z + 
            transformed_vertices[1].z + 
            transformed_vertices[2].z) / 3;

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y },
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth
        };
    
        // Save projected triangle in the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }

    // Sort the triangles to render by their avg_depth
    sort_by_depth(triangles_to_render);

}

void draw_wireframe(triangle_t triangle, uint32_t color) {
    draw_triangle(
        triangle.points[0].x, triangle.points[0].y, 
        triangle.points[1].x, triangle.points[1].y,
        triangle.points[2].x, triangle.points[2].y,
        color
    );
}

void draw_vertex_points(triangle_t triangle, uint32_t color) {
    draw_rect(triangle.points[0].x - 3 , triangle.points[0].y - 3, 6, 6, color);
    draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, color);
    draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, color);
}

void render(void) {
    draw_grid_as_lines(50);
    
    // loop projected points and render
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        enum Render_Modes {
            WIREFRAME_VERTEX,
            WIREFRAME_ONLY,
            SOLID,
            WIREFRAME_SOLID
        };

        int rm = (int)render_mode;

        if (rm == RENDER_SOLID || rm == RENDER_WIRE_SOLID) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, 
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                GRAY
            );
        }

        if (rm == RENDER_WIRE || rm == RENDER_WIRE_VERTEX || rm == RENDER_WIRE_SOLID) {
            draw_wireframe(triangle, WHITE);
        }

        if (rm == RENDER_WIRE_VERTEX) {
            draw_vertex_points(triangle, RED);
        }
    }

    // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);

    // Clear array of triangles to render every loop
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(BLACK);

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    return 0;
}