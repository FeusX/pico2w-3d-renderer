#ifndef RENDERER_H
#define RENDERER_H

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "display.h"
#include "../utils/color.h"
#include "../utils/type.h"

vec3_t rotate_x(vec3_t p, float angle);
vec3_t rotate_y(vec3_t p, float angle);
vec3_t rotate_z(vec3_t p, float angle);

void swap_int(int *a, int *b);
void draw_pixel(int x, int y, uint16_t color);
void fill_screen(uint16_t color);
void draw_line(int x0, int y0, int x1, int y1, uint16_t color);
void project_draw(vec3_t vec, uint16_t color);
void project_vertices(const object_t *ent, const camera_t *cam, vec2_t *projected, bool *visible);
void draw_hline(int x0, int x1, int y, uint16_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);

void draw_cube(float x, float y, float size, float angle, float focal, vec3_t *cube_vertices);

void render_wireframe(const object_t *ent, const camera_t *cam, vec2_t *projected, bool *visible);
void render_faces(const object_t *ent, const camera_t *cam, vec2_t *projected, bool *visible);

#endif
