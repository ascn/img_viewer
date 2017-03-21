#include <QImage>
#include <QString>
#include <QDebug>
#include <fstream>

#include "rasterize.h"
#include "tiny_obj_loader.h"
#include "vec4.h"
#include "mat4.h"

bool within(float y, float c1, float c2) {
    return (c1 <= y && y <= c2) || (c2 <= y && y <= c1);
}

float lerp(float a, float b, float alpha) {
    return a + alpha * (b - a);
}

float dist2(vec4 p1, vec4 p2) {
    return std::sqrt(std::pow((p1[0] - p2[0]), 2) + std::pow((p1[1] - p2[1]), 2));
}

camera_mat_t load_camera(char *file) {
    std::ifstream camera_file(file);
    if (!camera_file.is_open()) {
        fprintf(stderr, "error: cannot open file %s\n", file);
        exit(1);
    }
    float left, right, top, bottom, near, far;
    camera_file >> left;
    camera_file >> right;
    camera_file >> bottom;
    camera_file >> top;
    camera_file >> near;
    camera_file >> far;

    camera_mat_t ret;
    ret.proj = mat4::proj(left, right, top, bottom, near, far);
    mat4 view_t;
    float eye_x, eye_y, eye_z, c_x, c_y, c_z, u_x, u_y, u_z;
    camera_file >> eye_x;
    camera_file >> eye_y;
    camera_file >> eye_z;
    view_t[3][0] = -eye_x;
    view_t[3][1] = -eye_y;
    view_t[3][2] = -eye_z;

    mat4 view_o;
    camera_file >> c_x;
    camera_file >> c_y;
    camera_file >> c_z;
    float f_x = c_x - eye_x;
    float f_y = c_y - eye_y;
    float f_z = c_z - eye_z;
    camera_file >> u_x;
    camera_file >> u_y;
    camera_file >> u_z;
    vec4 forward = vec4(f_x, f_y, f_z, 0);
    forward.norm();
    vec4 up = vec4(u_x, u_y, u_z, 0);
    up.norm();
    vec4 x_axis = cross(forward, up);

    view_o[0][0] = x_axis[0];
    view_o[1][0] = x_axis[1];
    view_o[2][0] = x_axis[2];
    view_o[0][1] = up[0];
    view_o[1][1] = up[1];
    view_o[2][1] = up[2];
    view_o[0][2] = forward[0];
    view_o[1][2] = forward[1];
    view_o[2][2] = forward[2];

    ret.view = view_o * view_t;

    return ret;
}

QImage rasterize(char *obj, camera_mat_t cam, int w, int h, e_shader option) {

    // Initialize output image
    QImage out(w, h, QImage::Format_RGB888);

    // Initialize z-buffer
    std::vector<double> z_buf;
    for (int i = 0; i < w * h; ++i) {
        z_buf.push_back(2);
    }

    std::string mtl_path = "../obj/";

    // LOAD OBJ
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err = tinyobj::LoadObj(shapes, materials, obj, mtl_path.c_str());
    if ("" != err) {
        qDebug() << QString::fromStdString(err);
        exit(1);
    }
}
