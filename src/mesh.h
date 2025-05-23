#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

typedef struct {
    vec3_t* vertices;
    face_t* faces;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
} mesh_t;

extern mesh_t mesh;

void load_obj_file_data(char* filename);

#endif
