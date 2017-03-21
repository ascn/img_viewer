// Base code written by Jan Allbeck, Chris Czyzewicz, Cory Boatright, Tiantian Liu, Benedict Brown, and Adam Mally
// University of Pennsylvania

// The mat4 class is a 4x4 matrix containing a floating point number in each cell.
// Each mat4 is column-major, meaning the [] operator should return the Nth column of the matrix.
//
// OpenGL expects matrices to be column-major, so you will run into serious problems later
//  in the semester if your matrix library does using column-major.

#ifndef MAT4_H
#define MAT4_H

#include <iostream>
#include "vec4.h"

class mat4 {
 private:
  ///The set of vec4s that represent the columns of the matrix
	vec4 data[4];

 public:
	///----------------------------------------------------------------------
	/// Constructors
	///----------------------------------------------------------------------

	/// Default Constructor.  Initialize to identity matrix.
	mat4();

	/// Initializes the diagonal values of the matrix to diag. All other values are 0.
	mat4(float diag);

  /// Initializes matrix with each vector representing a column in the matrix
  mat4(const vec4 &col0, const vec4 &col1, const vec4 &col2, const vec4& col3);

  mat4(const mat4 &m2); // copy constructor; copies values of m2 into this

	///----------------------------------------------------------------------
	/// Getters
	///----------------------------------------------------------------------	

	/// Returns the values of the column at the index
  /// Does NOT check the array bound because doing so is slow
  const vec4 &operator[](unsigned int index) const;

	/// Returns a reference to the column at the index
  /// Does NOT check the array bound because doing so is slow
  vec4 &operator[](unsigned int index);

  /// Returns the values of the column at the index
  /// DOES check the array bound because doing so is slow
  const vec4 &operator()(unsigned int index) const;

	/// Returns a reference to the column at the index
  /// DOES check the array bound because doing so is slow
  vec4 &operator()(unsigned int index);

	///----------------------------------------------------------------------
	/// Static Initializers
	///----------------------------------------------------------------------	

	/// Creates a 3-D rotation matrix.
	/// Takes an angle in degrees and an axis represented by its xyz components, and outputs a 4x4 rotation matrix
	static mat4 rot(float angle, float x, float y, float z);

	/// Takes an xyz displacement and outputs a 4x4 translation matrix
	static mat4 trans(float x, float y, float z);

	/// Takes an xyz scale and outputs a 4x4 scale matrix
	static mat4 scale(float x, float y, float z);

	static mat4 proj(float l, float r, float t, float b, float n, float f);

	static mat4 view(float eye_x, float eye_y, float eye_z,
					 float center_x, float center_y, float center_z,
					 float up_x, float up_y, float up_z);

	///----------------------------------------------------------------------
	/// Operator Functions
	///----------------------------------------------------------------------

  /// Assign m2 to this and return this
  mat4 &operator=(const mat4 &m2);

  /// Test for equality
 	bool operator==(const mat4 &m2) const;

	/// Test for inequality
	bool operator!=(const mat4 &m2) const;

	/// Element-wise arithmetic
  /// e.g. += adds the elements of m2 to this and returns this (like regular +=)
  ///      +  returns a new matrix whose elements are the sums of this and v2
  mat4& operator+=(const mat4 &m2);
  mat4& operator-=(const mat4 &m2);
  mat4& operator*=(float c);                 // multiplication by a scalar
  mat4& operator/=(float c);                 // division by a scalar
  mat4  operator+(const mat4 &m2) const;
  mat4  operator-(const mat4 &m2) const;
  mat4  operator*(float c) const;             // multiplication by a scalar
  mat4  operator/(float c) const;             // division by a scalar

	/// Matrix multiplication (m1 * m2)
	mat4 operator*(const mat4 &m2) const;

	/// Matrix/vector multiplication (m * v)
	/// Assume v is a column vector (ie. a 4x1 matrix)
	vec4 operator*(const vec4 &v) const;

  ///----------------------------------------------------------------------
  /// Matrix Operations
  ///----------------------------------------------------------------------	

  /// Returns the transpose of the input matrix (v_ij == v_ji)
  mat4 transpose() const;

  /// Returns a column of the input matrix
  const vec4 &col(unsigned int index) const; // const version
  vec4 &col(unsigned int index);             // non-const version
};


/// Scalar multiplication (c * m)
mat4 operator*(float c, const mat4 &m);
	
/// Vector/matrix multiplication (v * m)
/// Assume v is a row vector (ie. a 1x4 matrix)
vec4 operator*(const vec4 &v, const mat4 &m);

/// Prints the matrix to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const mat4 &m);

#endif /* MAT4_H */
