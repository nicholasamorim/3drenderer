#include <stdio.h>
#include <stdint.h>
#include "upng.h"

int texture_width = 64;
int texture_height = 64;

upng_t* png = NULL;
uint32_t* mesh_texture = NULL;

void load_png_texture_data(char* filename) {
    png = upng_new_from_file(filename);

    if (png != NULL) {
        upng_decode(png);
        
        if (upng_get_error(png) == UPNG_EOK) {
            mesh_texture = (uint32_t*)upng_get_buffer(png);
            texture_width = upng_get_width(png);
            texture_height = upng_get_height(png);
        }
    }
}
