/**
 * Some quick Mat4 function since I don't know how to use glm.
 */

#include "mat4.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

GLfloat vec3_dot_product(GLfloat *vec3_a, GLfloat *vec3_b);

void print_mat4(GLfloat *mat4)
{
  int i = 0;

  for (i = 0; i < 16; i++) {
    printf("%5.2f,", *(mat4 + i));
    if ((i + 1) % 4 == 0) {
      printf("\n");
    }
  }
}

GLfloat *get_mat4() {
  GLfloat *mat4 = calloc( 16, sizeof(GLfloat));
  *(mat4) = 1.0f;
  *(mat4 + 5) = 1.0f;
  *(mat4 + 10) = 1.0f;
  *(mat4 + 15) = 1.0f;

  return mat4;
}

void rotate_mat4(GLfloat *mat4, GLfloat angle, enum mat4_axis axis)
{
  GLfloat cos_a = cos(angle);
  GLfloat sin_a = sin(angle);
  switch (axis) {
    case X_AXIS:
      *(mat4 + 5) = cos_a;
      *(mat4 + 6) = 0 - sin_a;
      *(mat4 + 9) = sin_a;
      *(mat4 + 10) = cos_a;
      break;
    case Y_AXIS:
      *(mat4) = cos_a;
      *(mat4 + 2) = sin_a;
      *(mat4 + 8) = 0 - sin_a;
      *(mat4 + 10) = cos_a;
      break;
    case Z_AXIS:
      *(mat4) = cos_a;
      *(mat4 + 1) = 0 - sin_a;
      *(mat4 + 4) = sin_a;
      *(mat4 + 5) = cos_a;
      break;
  }
}

void fps_view(GLfloat *mat4_result, GLfloat *vec3_eye, GLfloat pitch, GLfloat yaw)
{
  GLfloat cos_pitch = cos(pitch);
  GLfloat sin_pitch = sin(pitch);
  GLfloat cos_yaw = cos(yaw);
  GLfloat sin_yaw = sin(yaw);

  GLfloat xaxis[3] = { cos_yaw, 0, -sin_yaw };
  GLfloat yaxis[3] = { sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch };
  GLfloat zaxis[3] = { sin_yaw * cos_pitch, -sin_pitch, cos_pitch * cos_yaw };

  *(mat4_result) = xaxis[0];
  *(mat4_result + 1) = xaxis[1];
  *(mat4_result + 2) = xaxis[2];
  *(mat4_result + 3) = -vec3_dot_product(xaxis, vec3_eye);
  *(mat4_result + 4) = yaxis[0];
  *(mat4_result + 5) = yaxis[1];
  *(mat4_result + 6) = yaxis[2];
  *(mat4_result + 7) = -vec3_dot_product(yaxis, vec3_eye);
  *(mat4_result + 8) = zaxis[0];
  *(mat4_result + 9) = zaxis[1];
  *(mat4_result + 10) = zaxis[2];
  *(mat4_result + 11) = -vec3_dot_product(zaxis, vec3_eye);
  *(mat4_result + 12) = 0;
  *(mat4_result + 13) = 0;
  *(mat4_result + 14) = 0;
  *(mat4_result + 15) = 1;
}

GLfloat vec3_dot_product(GLfloat *vec3_a, GLfloat *vec3_b)
{
  return vec3_a[0] * vec3_b[0] + vec3_a[1] * vec3_b[1] + vec3_a[2] * vec3_b[2] + vec3_a[3] * vec3_b[3];
}
