#include <cmath>
#include <iostream>
#include <iomanip>
#include <assert.h>

#include "vec4.h"

/**
 * Constructors
 */

vec4::vec4() {
    this->data[0] = 0;
    this->data[1] = 0;
    this->data[2] = 0;
    this->data[3] = 0;
}

vec4::vec4(float x, float y, float z, float w) {
    this->data[0] = x;
    this->data[1] = y;
    this->data[2] = z;
    this->data[3] = w;
}

vec4::vec4(const vec4 &v2) {
    this->data[0] = v2[0];
    this->data[1] = v2[1];
    this->data[2] = v2[2];
    this->data[3] = v2[3];
}

/**
 * Getters/Setters
 */

float vec4::operator[](unsigned int index) const {
    return data[index];
}

float &vec4::operator[](unsigned int index) {
    return data[index];
}

float vec4::operator()(unsigned int index) const {
    assert(index < 4);
    return data[index];
}

float &vec4::operator()(unsigned int index) {
    assert(index < 4);
    return data[index];
}

/**
 * Operator Methods
 */

vec4 &vec4::operator=(const vec4 &v2) {
    this->data[0] = v2[0];
    this->data[1] = v2[1];
    this->data[2] = v2[2];
    this->data[3] = v2[3];
    return *this;
}

bool vec4::operator==(const vec4 &v2) const {
    return (data[0] == v2[0] && data[1] == v2[1] &&
            data[2] == v2[2] && data[3] == v2[3]);
}

bool vec4::operator!=(const vec4 &v2) const {
    return (data[0] != v2[0] || data[1] != v2[1] ||
            data[2] != v2[2] || data[3] != v2[3]);
}

vec4 &vec4::operator+=(const vec4 &v2) {
    this->data[0] += v2[0];
    this->data[1] += v2[1];
    this->data[2] += v2[2];
    this->data[3] += v2[3];
    return *this;
}

vec4 &vec4::operator-=(const vec4 &v2) {
    this->data[0] -= v2[0];
    this->data[1] -= v2[1];
    this->data[2] -= v2[2];
    this->data[3] -= v2[3];
    return *this;
}

vec4 &vec4::operator*=(float c) {
    this->data[0] *= c;
    this->data[1] *= c;
    this->data[2] *= c;
    this->data[3] *= c;
    return *this;
}

vec4 &vec4::operator/=(float c) {
    this->data[0] /= c;
    this->data[1] /= c;
    this->data[2] /= c;
    this->data[3] /= c;
    return *this;
}

vec4 vec4::operator+(const vec4 &v2) const {
    return vec4(this->data[0] + v2[0], this->data[1] + v2[1],
                this->data[2] + v2[2], this->data[3] + v2[3]);
}

vec4 vec4::operator-(const vec4 &v2) const {
    return vec4(this->data[0] - v2[0], this->data[1] - v2[1],
                this->data[2] - v2[2], this->data[3] - v2[3]);
}

vec4 vec4::operator*(float c) const {
    return vec4(this->data[0] * c, this->data[1] * c,
                this->data[2] * c, this->data[3] * c);
}

vec4 vec4::operator/(float c) const {
    return vec4(this->data[0] / c, this->data[1] / c,
                this->data[2] / c, this->data[3] / c);
}

/**
 * Other Methods
 */

float vec4::length() const {
    return (sqrt(pow(this->data[0], 2) + pow(this->data[1], 2) +
                pow(this->data[2], 2) + pow(this->data[3], 2)));
}

vec4 vec4::normalize() const {
    float len = this->length();
    if (len == 0) { return vec4(0, 0, 0, 0); }
    return vec4(this->data[0] / len, this->data[1] / len,
                this->data[2] / len, this->data[3] / len);
}

void vec4::norm() {
    float len = this->length();
    if (len == 0) { return; }
    this->data[0] /= len;
    this->data[1] /= len;
    this->data[2] /= len;
    this->data[3] /= len;
}

/**
 * Other Functions
 */

float dot(const vec4 &v1, const vec4 &v2) {
    return (v1[0] * v2[0] + v1[1] * v2[1] +
            v1[2] * v2[2] + v1[3] * v2[3]);
}

vec4 cross(const vec4 &v1, const vec4 &v2) {
    vec4 ret;
    ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
    ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
    ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
    ret[3] = 0;
    return ret;
}

vec4 operator*(float c, const vec4 &v) {
    return vec4(v[0] * c, v[1] * c, v[2] * c, v[3] * c);
}

std::ostream &operator<<(std::ostream &o, const vec4 &v) {
    o << std::right << std::setw(6) << std::setprecision(3) <<
        v[0] << '\n' << std::setw(6) << v[1] << '\n' << std::setw(6) <<
        v[2] << '\n' << std::setw(6) << v[3] << '\n';
    return o;
}
