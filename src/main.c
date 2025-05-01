#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "array.h"
#include "color.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include "upng.h"

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
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
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
    z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);
    
    // Create a SDL Texture for the color display
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
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

    // load_obj_file_data("./assets/cube.obj");
    // load_png_texture_data("./assets/cube.png");
    // load_obj_file_data("./assets/f22.obj");
    // load_png_texture_data("./assets/f22.png");
    
    load_obj_file_data("./assets/drone.obj");
    load_png_texture_data("./assets/drone.png");
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
                else if (event.key.keysym.sym == SDLK_5) 
                render_mode = RENDER_TEXTURED;
                else if (event.key.keysym.sym == SDLK_6) 
                render_mode = RENDER_TEXTURED_WIRE;
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
    // Wait until FRAME_TARGET_TIME passes ensuring consistent FPS
    // int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    // Init array of triangles for the frame
    triangles_to_render = NULL;

    // Test transformations
    mesh.rotation.x += 0.005;
    // mesh.rotation.y += 0.1;
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
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

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
    
        if (cull_mode == CULL_BACKFACE) {
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

            // Invert y to account for flipped screen y coordinates
            projected_points[j].y *= -1;

            // Translate the proj point to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        // Calculate average depth for each face based on their Z values
        float avg_depth = (
            transformed_vertices[0].z + 
            transformed_vertices[1].z + 
            transformed_vertices[2].z) / 3.0;

        // Calculate color based on light angle
        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
            
        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
            },
            .texcoords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            },
            .color = triangle_color,
            .avg_depth = avg_depth
        };
    
        // Save projected triangle in the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }

    // Sort the triangles to render by their avg_depth
    sort_by_depth(triangles_to_render);

}

void render(void) {
    draw_grid_as_lines(50);
    
    // loop projected points and render
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        int rm = (int)render_mode;

        if (rm == RENDER_SOLID || rm == RENDER_WIRE_SOLID) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, 
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color
            );
        }

        if (rm == RENDER_TEXTURED || rm == RENDER_TEXTURED_WIRE) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture
            );
        }

        if (rm == RENDER_WIRE || rm == RENDER_WIRE_VERTEX || rm == RENDER_WIRE_SOLID || rm == RENDER_TEXTURED_WIRE) {
            draw_wireframe(triangle, WHITE);
        }

        if (rm == RENDER_WIRE_VERTEX) {
            draw_vertex_points(triangle, RED);
        }
    }

    // Clear array of triangles to render every loop
    array_free(triangles_to_render);

    render_color_buffer();

    clear_color_buffer(BLACK);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    free(z_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
    upng_free(png);
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