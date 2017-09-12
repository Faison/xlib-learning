/**
 * The header file for the mat4 functions.
 */

#include <GL/gl.h>

enum mat4_axis {
  X_AXIS,
  Y_AXIS,
  Z_AXIS
};

void print_mat4(GLfloat *mat4);
GLfloat *get_mat4();
void rotate_mat4(GLfloat *mat4, GLfloat angle, enum mat4_axis axis);
void fps_view(GLfloat *mat4_result, GLfloat *vec3_eye, GLfloat pitch, GLfloat yaw);
// void multiply_mat4(GLfloat *mat4_a, GLfloat *vec2_a_size, GLfloat *mat4_b, GLfloat *vec2_b_size);
