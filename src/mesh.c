#include <stdio.h>
#include <string.h>
#include "array.h"
#include "mesh.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 },
    .scale = { 1.0, 1.0, 1.0 },
    .translation = { 0, 0, 0 }
};


void load_obj_file_data(char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot read asset\n");
        return;
    }
    char buffer[1024];

    tex2_t* texcoords = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {

        if (strncmp(buffer, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }
        
        if (strncmp(buffer, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(buffer, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        }

        if (strncmp(buffer, "f ", 2) == 0) {
            // structure to store the f lines from the obj
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            
            sscanf(
                buffer, 
                "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0], 
                &vertex_indices[1], &texture_indices[1], &normal_indices[1], 
                &vertex_indices[2], &texture_indices[2], &normal_indices[2] 
            );

            face_t face = {
                .a = vertex_indices[0] - 1,
                .b = vertex_indices[1] - 1,
                .c = vertex_indices[2] - 1,
                .a_uv = texcoords[texture_indices[0] - 1],
                .b_uv = texcoords[texture_indices[1] - 1],
                .c_uv = texcoords[texture_indices[2] - 1],
                .color = 0xFFFFFFFF
            };

            array_push(mesh.faces, face);
        }
    }
    array_free(texcoords);
}
