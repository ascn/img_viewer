#include <QImage>

#include "vec4.h"
#include "mat4.h"

bool within(float x, float c1, float c2);
float lerp(float a, float b, float alpha);
float dist2(vec4 p1, vec4 p2);

typedef struct pixel_t {
    unsigned char r, g, b;
} pixel_t;

typedef struct {
    vec4 vert[3];
    vec4 pixel_coord[3];
    vec4 bounding_box[2];
    bool is_renderable = true;
    pixel_t color;
    vec4 normals[3];
} face_t;

typedef struct {
    mat4 proj;
    mat4 view;
} camera_mat_t;

camera_mat_t load_camera(const char *file);

typedef enum { NONE, WHITE, NORM_FLAT, NORM_GOURAUD, NORM_BARY,
               NORM_GOURAUD_Z, NORM_BARY_Z, RANDOM, TEXTURE } e_shader;

QImage rasterize(const char *obj, camera_mat_t& camera, int w, int h, e_shader shading);