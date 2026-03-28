#ifndef TYPE_H
#define TYPE_H

typedef struct { 
  float x, y, z; 
} vec3_t;

typedef struct {
  int x, y;
} vec2_t;

typedef struct {
  float m[3][3];
} mat3_t;

typedef struct {
  const vec3_t *vertices;
  const uint16_t *edges;
  const uint16_t *faces;
  uint16_t vertex_count, edge_count, face_count;
} mesh_t;

typedef struct {
  vec3_t position, rotation;
  float focal_length;
} camera_t;

typedef struct {
  vec3_t position, rotation;
  float scale;
  const mesh_t *mesh;
  uint16_t color;
} object_t;

#endif
