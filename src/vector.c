#include <math.h>
#include "vector.h"

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}


vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

float vec2_length(vec2_t v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

float vec3_length(vec3_t v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

vec2_t vec2_add(vec2_t v1, vec2_t v2) {
    vec2_t vec = {
        v1.x + v2.x,
        v1.y + v2.y,
    };

    return vec;
}

vec3_t vec3_add(vec3_t v1, vec3_t v2) {
    vec3_t vec = {
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z,
    };

    return vec;
}

vec2_t vec2_sub(vec2_t v1, vec2_t v2) {
    vec2_t vec = {
        v1.x - v2.x,
        v1.y - v2.y,
    };

    return vec;
}

vec3_t vec3_sub(vec3_t v1, vec3_t v2) {
    vec3_t vec = {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
    };

    return vec;
}

vec2_t vec2_mul(vec2_t v, float factor) {
    vec2_t vec = {
        v.x * factor,
        v.y * factor
    };

    return vec;
}

vec2_t vec2_div(vec2_t v, float factor) {
    vec2_t vec = {
        v.x / factor,
        v.y / factor
    };

    return vec;
}

vec3_t vec3_mul(vec3_t v, float factor) {
    vec3_t vec = {
        v.x * factor,
        v.y * factor,
        v.z * factor
    };

    return vec;
}

vec3_t vec3_div(vec3_t v, float factor) {
    vec3_t vec = {
        v.x / factor,
        v.y / factor,
        v.z / factor
    };

    return vec;
}

vec3_t vec3_cross(vec3_t v1, vec3_t v2) {
    vec3_t vector = {
        (v1.y * v2.z) - (v1.z * v2.y),
        (v1.z * v2.x) - (v1.x * v2.z),
        (v1.x * v2.y) - (v1.y * v2.x)
    };

    return vector;
}

float vec2_dot(vec2_t v1, vec2_t v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
}

float vec3_dot(vec3_t v1, vec3_t v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

void vec2_normalize(vec2_t* v) {
    float length = sqrt(v->x * v->x + v->y * v->y);
    v->x /= length;
    v->y /= length;
}

void vec3_normalize(vec3_t* v) {
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;

}

vec4_t vec4_from_vec3(vec3_t v) {
    vec4_t result = { v.x, v.y, v.z, 1.0 };
    return result; 
}

vec3_t vec3_from_vec4(vec4_t v) {
    vec3_t result = { v.x, v.y, v.z };
    return result; 
}

