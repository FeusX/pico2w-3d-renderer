#include "renderer.h"

#include "pico/divider.h"

void swap_int(int *a, int *b) { int t = *a; *a = *b; *b = t; }

void draw_pixel(int x, int y, uint16_t color)
{
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
  vram[y * SCREEN_W + x] = color;
}

void fill_screen(uint16_t color)
{
  if(color == 0x0000 || color == 0xFFFF) { memset(vram, color & 0xFF, sizeof(vram)); return; }

  uint32_t color32 = (color << 16) | color;
  uint32_t *vram32 = (uint32_t*)vram;
  for(int i = 0; i < (SCREEN_W * SCREEN_H) / 2; i++) { vram32[i] = color32; }
}

void project_draw(vec3_t vec, uint16_t color)
{
  if(vec.z <= 0) return;

  float focal_length = 200.0f;

  int sx = (int)((vec.x * focal_length) / vec.z) + (SCREEN_W / 2);
  int sy = (int)((vec.y * focal_length) / vec.z) + (SCREEN_H / 2);

  draw_pixel(sx, sy, color);
}

void draw_line(int x0, int y0, int x1, int y1, uint16_t color) 
{
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while(true)
  {
    draw_pixel(x0, y0, color);
    if(x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if(e2 >= dy) { err += dy; x0 += sx; }
    if(e2 <= dx) { err += dx; y0 += sy; }
  }
}

vec3_t rotate_x(vec3_t p, float angle)
{
  vec3_t out;
  float c = cosf(angle), s = sinf(angle);
  out.x = p.x;
  out.y = p.y * c - p.z * s;
  out.z = p.y * s + p.z * c;
  return out;
}

vec3_t rotate_y(vec3_t p, float angle)
{
  vec3_t out;
  float c = cosf(angle), s = sinf(angle);
  out.x = p.x * c + p.z * s;
  out.y = p.y;
  out.z = -p.x * s + p.z * c;
  return out;
}

vec3_t rotate_z(vec3_t p, float angle)
{
  vec3_t out;
  float c = cosf(angle), s = sinf(angle);
  out.x = p.x * c - p.y * s;
  out.y = p.x * s + p.y * c;
  out.z = p.z;
  return out;
}

void draw_cube(float x, float y, float size, float angle, float focal, vec3_t *cube_vertices)
{
  int proj_x[8], proj_y[8];

  for(int i = 0; i < 8; i++)
  {
    vec3_t v = cube_vertices[i];

    v.x *= size;
    v.y *= size;
    v.z *= size;

    v = rotate_x(v, angle);
    v = rotate_y(v, angle * 0.8f);
    v = rotate_z(v, angle * 0.5f);

    v.z += 150.0f;
    proj_x[i] = (int)((v.x * focal) / v.z) + (SCREEN_W / 2) + (int)x;
    proj_y[i] = (int)((v.y * focal) / v.z) + (SCREEN_H / 2) + (int)y;
  }
  
  for(int i = 0; i < 4; i++)
  {
    draw_line(proj_x[i], proj_y[i], proj_x[(i + 1) % 4], proj_y[(i + 1) % 4], CL_RED);
    draw_line(proj_x[i + 4], proj_y[i + 4], proj_x[((i + 1) % 4) + 4], proj_y[((i + 1) % 4) + 4], CL_BLUE);
    draw_line(proj_x[i], proj_y[i], proj_x[i + 4], proj_y[i + 4], CL_GREEN);
  }
}

mat3_t rot_matrix(vec3_t rot, float s)
{
  float cx = cosf(rot.x), sx = sinf(rot.x);
  float cy = cosf(rot.y), sy = sinf(rot.y);
  float cz = cosf(rot.z), sz = sinf(rot.z);

  mat3_t res;

  res.m[0][0] = s * (cy * cz);
  res.m[0][1] = s * (cy * sz);
  res.m[0][2] = s * (-sy);

  res.m[1][0] = s * (sx * sy * cz - cx * sz);
  res.m[1][1] = s * (sx * sy * sz + cx * cz);
  res.m[1][2] = s * (sx * cy);

  res.m[2][0] = s * (cx * sy * cz + sx * sz);
  res.m[2][1] = s * (cx * sy * sz - sx * cz);
  res.m[2][2] = s * (cx * cy);
  return res;
}

void draw_hline(int x0, int x1, int y, uint16_t color)
{
  if(x0 > x1) swap_int(&x0, &x1);
  
  if(y < 0 || y >= SCREEN_H || x0 >= SCREEN_W || x1 < 0) return;
  if(x0 < 0) x0 = 0;
  if(x1 >= SCREEN_W) x1 = SCREEN_W - 1;

  uint16_t *row_ptr = &vram[y * SCREEN_W + x0];
  for(int x = x0; x <= x1; x++) { *row_ptr++ = color; }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color)
{
  if(y0 > y1) { swap_int(&y0, &y1); swap_int(&x0, &x1); }
  if(y0 > y2) { swap_int(&y0, &y2); swap_int(&x0, &x2); }
  if(y1 > y2) { swap_int(&y1, &y2); swap_int(&x1, &x2); }

  int total_height = y2 - y0;
  if(total_height == 0) return;

  for(int i = 0; i < total_height; i++)
  {
    bool second_half = i > y1 - y0 || y1 == y0;
    int segment_height = second_half ? y2 - y1 : y1 - y0;

    float alpha = (float)i / total_height;
    float beta  = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;

    int ax = x0 + (x2 - x0) * alpha;
    int bx = second_half ? x1 + (x2 - x1) * beta : x0 + (x1 - x0) * beta;

    draw_hline(ax, bx, y0 + i, color);
  }
}

void render_wireframe(const object_t *ent, const camera_t *cam, vec2_t *projected, bool *visible)
{
  mat3_t rot_mat = rot_matrix(ent->rotation, ent->scale);

  float rel_x = ent->position.x - cam->position.x;
  float rel_y = ent->position.y - cam->position.y;
  float rel_z = ent->position.z - cam->position.z;

  for(int i = 0; i < ent->mesh->vertex_count; i++)
  {
    vec3_t v = ent->mesh->vertices[i];

    float rx = v.x * rot_mat.m[0][0] + v.y * rot_mat.m[0][1] + v.z * rot_mat.m[0][2] + rel_x;
    float ry = v.x * rot_mat.m[1][0] + v.y * rot_mat.m[1][1] + v.z * rot_mat.m[1][2] + rel_y;
    float rz = v.x * rot_mat.m[2][0] + v.y * rot_mat.m[2][1] + v.z * rot_mat.m[2][2] + rel_z;

    if(rz < 0.1f) { visible[i] = false; continue; }

    int32_t x_num = (int32_t)(rx * cam->focal_length);
    int32_t y_num = (int32_t)(ry * cam->focal_length);
    int32_t den = (int32_t)(rz <= 1.0f ? 1 : rz);

    visible[i] = true;

    hw_divider_divmod_s32_start(x_num, den);
    projected[i].x = hw_divider_s32_quotient_wait() + (SCREEN_W >> 1);

    hw_divider_divmod_s32_start(y_num, den);
    projected[i].y = hw_divider_s32_quotient_wait() + (SCREEN_H >> 1);
  }

  for(int i = 0; i < ent->mesh->edge_count; i++)
  {
    int a = ent->mesh->edges[i * 2];
    int b = ent->mesh->edges[i * 2 + 1];

    if(visible[a] && visible[b])
    { 
      draw_line(projected[a].x, projected[a].y, projected[b].x, projected[b].y, ent->color); 
    }
  }
}

void render_faces(const object_t *ent, const camera_t *cam, vec2_t *projected, bool *visible)
{
  mat3_t rot_mat = rot_matrix(ent->rotation, ent->scale);

  float rel_x = ent->position.x - cam->position.x;
  float rel_y = ent->position.y - cam->position.y;
  float rel_z = ent->position.z - cam->position.z;

  for(int i = 0; i < ent->mesh->vertex_count; i++)
  {
    vec3_t v = ent->mesh->vertices[i];

    float rx = v.x * rot_mat.m[0][0] + v.y * rot_mat.m[0][1] + v.z * rot_mat.m[0][2] + rel_x;
    float ry = v.x * rot_mat.m[1][0] + v.y * rot_mat.m[1][1] + v.z * rot_mat.m[1][2] + rel_y;
    float rz = v.x * rot_mat.m[2][0] + v.y * rot_mat.m[2][1] + v.z * rot_mat.m[2][2] + rel_z;

    if(rz < 0.1f) { visible[i] = false; continue; }

    int32_t x_num = (int32_t)(rx * cam->focal_length);
    int32_t y_num = (int32_t)(ry * cam->focal_length);
    int32_t den = (int32_t)(rz <= 1.0f ? 1 : rz);

    visible[i] = true;

    hw_divider_divmod_s32_start(x_num, den);
    projected[i].x = hw_divider_s32_quotient_wait() + (SCREEN_W >> 1);

    hw_divider_divmod_s32_start(y_num, den);
    projected[i].y = hw_divider_s32_quotient_wait() + (SCREEN_H >> 1);
  }

  for(int i = 0; i < ent->mesh->face_count; i++)
  {
    int a = ent->mesh->faces[i * 3];
    int b = ent->mesh->faces[i * 3 + 1];
    int c = ent->mesh->faces[i * 3 + 2];

    if(visible[a] && visible[b] && visible[c])
    {
      int x0 = projected[a].x, y0 = projected[a].y;
      int x1 = projected[b].x, y1 = projected[b].y;
      int x2 = projected[c].x, y2 = projected[c].y;

      int winding = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
        
      if(winding > 0)
      { draw_triangle(x0, y0, x1, y1, x2, y2, ent->color); }
    }
  }
}