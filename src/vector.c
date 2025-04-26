#include <math.h>
#include "vector.h"

vec3_t vec3_rotate(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}