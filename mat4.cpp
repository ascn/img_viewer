#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif // _USE_MATH_DEFINES

#include <ostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <math.h>

#include "mat4.h"

mat4::mat4() {
	this->data[0] = vec4(1, 0, 0, 0);
	this->data[1] = vec4(0, 1, 0, 0);
	this->data[2] = vec4(0, 0, 1, 0);
	this->data[3] = vec4(0, 0, 0, 1);
}

mat4::mat4(float diag) {
	this->data[0] = vec4(diag, 0, 0, 0);
	this->data[1] = vec4(0, diag, 0, 0);
	this->data[2] = vec4(0, 0, diag, 0);
	this->data[3] = vec4(0, 0, 0, diag);
}

mat4::mat4(const vec4 &col0, const vec4 &col1,
				const vec4 &col2, const vec4 &col3) {
	this->data[0] = col0;
	this->data[1] = col1;
	this->data[2] = col2;
	this->data[3] = col3;
}

mat4::mat4(const mat4 &m2) {
	this->data[0] = m2[0];
	this->data[1] = m2[1];
	this->data[2] = m2[2];
	this->data[3] = m2[3];
}

const vec4 &mat4::operator[](unsigned int index) const {
	return this->data[index];
}

vec4 &mat4::operator[](unsigned int index) {
	return this->data[index];
}

const vec4 &mat4::operator()(unsigned int index) const {
    assert(index < 4);
	return this->data[index];
}

vec4 &mat4::operator()(unsigned int index) {
    assert(index < 4);
	return this->data[index];
}

/**
 * Static Initializers
 */

mat4 mat4::rot(float angle, float x, float y, float z) {
	mat4 ret;
	float theta = angle * (M_PI / 180);
	float t = 1 - cos(theta);
	ret[0][0] = cos(theta) + x * x * t;
	ret[0][1] = z * sin(theta) + x * y * t;
	ret[0][2] = -y * sin(theta) + x * z * t;
	ret[0][3] = 0;
	ret[1][0] = -z * sin(theta) + x * y * t;
	ret[1][1] = cos(theta) + y * y * t;
	ret[1][2] = x * sin(theta) + y * z * t;
	ret[1][3] = 0;
	ret[2][0] = y * sin(theta) + x * z * t;
	ret[2][1] = -x * sin(theta) + y * z * t;
	ret[2][2] = cos(theta) + z * z * t;
	return ret;
}

mat4 mat4::trans(float x, float y, float z) {
	mat4 ret;
	ret[3][0] = x;
	ret[3][1] = y;
	ret[3][2] = z;
	ret[3][3] = 1;
    return ret;
}

mat4 mat4::scale(float x, float y, float z) {
	mat4 ret;
	ret[0][0] = x;
	ret[1][1] = y;
	ret[2][2] = z;
	ret[3][3] = 1;
	return ret;
}

mat4 mat4::proj(float l, float r, float t, float b, float n, float f) {
	mat4 ret;
	ret[0][0] = (2 * n) / (r - l);
	ret[1][1] = (2 * n) / (t - b);
	ret[2][0] = (r + l) / (r - l);
	ret[2][1] = (t + b) / (t - b);
	ret[2][2] = f / (f - n);
	ret[2][3] = 1;
	ret[3][2] = (-f * n) / (f - n);
    ret[3][3] = 0;
	return ret;
}

/**
 * Operator Functions
 */

mat4 &mat4::operator=(const mat4 &m2) {
	this->data[0] = m2[0];
	this->data[1] = m2[1];
	this->data[2] = m2[2];
	this->data[3] = m2[3];
	return *this;
}

bool mat4::operator==(const mat4 & m2) const {
	return (this->data[0] == m2[0] && this->data[1] == m2[1] &&
				this->data[2] == m2[2] && this->data[3] == m2[3]);
}

bool mat4::operator!=(const mat4 &m2) const {
	return (this->data[0] != m2[0] || this->data[1] != m2[1] ||
				this->data[2] != m2[2] || this->data[3] != m2[3]);
}

mat4 &mat4::operator+=(const mat4 &m2) {
	this->data[0] += m2[0];
	this->data[1] += m2[1];
	this->data[2] += m2[2];
	this->data[3] += m2[3];
	return *this;
}

mat4 &mat4::operator-=(const mat4 &m2) {
	this->data[0] -= m2[0];
	this->data[1] -= m2[1];
	this->data[2] -= m2[2];
	this->data[3] -= m2[3];
	return *this;
}

mat4 &mat4::operator*=(float c) {
	this->data[0] *= c;
	this->data[1] *= c;
	this->data[2] *= c;
	this->data[3] *= c;
	return *this;
}

mat4 &mat4::operator/=(float c) {
	this->data[0] /= c;
	this->data[1] /= c;
	this->data[2] /= c;
	this->data[3] /= c;
	return *this;
}

mat4 mat4::operator+(const mat4 &m2) const {
	return mat4(this->data[0] + m2[0], this->data[1] + m2[1],
					this->data[2] + m2[2], this->data[3] + m2[3]);
}

mat4 mat4::operator-(const mat4 &m2) const {
	return mat4(this->data[0] - m2[0], this->data[1] - m2[1],
					this->data[2] - m2[2], this->data[3] - m2[3]);
}

mat4 mat4::operator*(float c) const {
	return mat4(this->data[0] * c, this->data[1] * c,
					this->data[2] * c, this->data[3] * c);
}

mat4 mat4::operator/(float c) const {
	return mat4(this->data[0] / c, this->data[1] / c,
					this->data[2] / c, this->data[3] / c);
}

mat4 mat4::operator*(const mat4 &m2) const {
	vec4 col0, col1, col2, col3;
	col0[0] = this->data[0][0] * m2[0][0] + this->data[1][0] * m2[0][1] +
				this->data[2][0] * m2[0][2] + this->data[3][0] * m2[0][3];
	col0[1] = this->data[0][1] * m2[0][0] + this->data[1][1] * m2[0][1] +
				this->data[2][1] * m2[0][2] + this->data[3][1] * m2[0][3];
	col0[2] = this->data[0][2] * m2[0][0] + this->data[1][2] * m2[0][1] +
				this->data[2][2] * m2[0][2] + this->data[3][2] * m2[0][3];
	col0[3] = this->data[0][3] * m2[0][0] + this->data[1][3] * m2[0][1] +
				this->data[2][3] * m2[0][2] + this->data[3][3] * m2[0][3];
	col1[0] = this->data[0][0] * m2[1][0] + this->data[1][0] * m2[1][1] +
				this->data[2][0] * m2[1][2] + this->data[3][0] * m2[1][3];
	col1[1] = this->data[0][1] * m2[1][0] + this->data[1][1] * m2[1][1] +
				this->data[2][1] * m2[1][2] + this->data[3][1] * m2[1][3];
	col1[2] = this->data[0][2] * m2[1][0] + this->data[1][2] * m2[1][1] +
				this->data[2][2] * m2[1][2] + this->data[3][2] * m2[1][3];
	col1[3] = this->data[0][3] * m2[1][0] + this->data[1][3] * m2[1][1] +
				this->data[2][3] * m2[1][2] + this->data[3][3] * m2[1][3];
	col2[0] = this->data[0][0] * m2[2][0] + this->data[1][0] * m2[2][1] +
				this->data[2][0] * m2[2][2] + this->data[3][0] * m2[2][3];
	col2[1] = this->data[0][1] * m2[2][0] + this->data[1][1] * m2[2][1] +
				this->data[2][1] * m2[2][2] + this->data[3][1] * m2[2][3];
	col2[2] = this->data[0][2] * m2[2][0] + this->data[1][2] * m2[2][1] +
				this->data[2][2] * m2[2][2] + this->data[3][2] * m2[2][3];
	col2[3] = this->data[0][3] * m2[2][0] + this->data[1][3] * m2[2][1] +
				this->data[2][3] * m2[2][2] + this->data[3][3] * m2[2][3];
	col3[0] = this->data[0][0] * m2[3][0] + this->data[1][0] * m2[3][1] +
				this->data[2][0] * m2[3][2] + this->data[3][0] * m2[3][3];
	col3[1] = this->data[0][1] * m2[3][0] + this->data[1][1] * m2[3][1] +
				this->data[2][1] * m2[3][2] + this->data[3][1] * m2[3][3];
	col3[2] = this->data[0][2] * m2[3][0] + this->data[1][2] * m2[3][1] +
				this->data[2][2] * m2[3][2] + this->data[3][2] * m2[3][3];
	col3[3] = this->data[0][3] * m2[3][0] + this->data[1][3] * m2[3][1] +
				this->data[2][3] * m2[3][2] + this->data[3][3] * m2[3][3];
	return mat4(col0, col1, col2, col3);
}

vec4 mat4::operator*(const vec4 &v) const {
	vec4 ret;
	ret[0] = this->data[0][0] * v[0] + this->data[1][0] * v[1] +
				this->data[2][0] * v[2] + this->data[3][0] * v[3];
	ret[1] = this->data[0][1] * v[0] + this->data[1][1] * v[1] +
				this->data[2][1] * v[2] + this->data[3][1] * v[3];
	ret[2] = this->data[0][2] * v[0] + this->data[1][2] * v[1] +
				this->data[2][2] * v[2] + this->data[3][2] * v[3];
	ret[3] = this->data[0][3] * v[0] + this->data[1][3] * v[1] +
				this->data[2][3] * v[2] + this->data[3][3] * v[3];
	return ret;
}

/**
 * Matrix Operations
 */

mat4 mat4::transpose() const {
	mat4 ret = *this;
	std::swap(ret[0][1], ret[1][0]);
	std::swap(ret[0][2], ret[2][0]);
	std::swap(ret[0][3], ret[3][0]);
	std::swap(ret[1][2], ret[2][1]);
	std::swap(ret[1][3], ret[3][1]);
	std::swap(ret[2][3], ret[3][2]);
	return ret;
}

const vec4 &mat4::col(unsigned int index) const {
	return this->data[index];
}

vec4 &mat4::col(unsigned int index) {
	return this->data[index];
}

mat4 operator*(float c, const mat4 &m) {
	return mat4(m[0] * c, m[1] * c, m[2] * c, m[3] * c);
}

vec4 operator*(const vec4 &v, const mat4 &m) {
	vec4 ret;
	ret[0] = v[0] * m[0][0] + v[1] * m[0][1] +
			v[2] * m[0][2] + v[3] * m[0][3];
	ret[1] = v[0] * m[1][0] + v[1] * m[1][1] +
			v[2] * m[1][2] + v[3] * m[1][3];
	ret[2] = v[0] * m[2][0] + v[1] * m[2][1] +
			v[2] * m[2][2] + v[3] * m[2][3];
	ret[3] = v[0] * m[3][0] + v[1] * m[3][1] +
			v[2] * m[3][2] + v[3] * m[3][3];
    return ret;
}

std::ostream &operator<<(std::ostream &o, const mat4 &m) {
	o << std::right << std::setw(10) << std::setprecision(3) <<
	m[0][0] << std::setw(10) << m[1][0] << std::setw(10) << m[2][0] <<
	std::setw(10) << m[3][0] << '\n' <<
	std::setw(10) << m[0][1] << std::setw(10) << m[1][1] <<
	std::setw(10) << m[2][1] << std::setw(10) << m[3][1] << '\n' <<
	std::setw(10) << m[0][2] << std::setw(10) << m[1][2] <<
	std::setw(10) << m[2][2] << std::setw(10) << m[3][2] << '\n' <<
    std::setw(10) << m[0][3] << std::setw(10) << m[1][3] <<
    std::setw(10) << m[2][3] << std::setw(10) << m[3][3] << '\n';
    return o;
}
