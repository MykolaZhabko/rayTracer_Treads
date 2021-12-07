#include <stdbool.h>
#include <math.h>

/* The vector structure */
typedef struct {
  float x, y, z;
}
vector;

/* The ray */
typedef struct {
  vector start;
  vector dir;
}
ray;

/* The sphere */
typedef struct {
  vector pos;
  float radius;
  int material;
}
sphere;

/* Colour */
typedef struct {
  float red, green, blue;
}
colour;

/* Material Definition */
typedef struct {
  colour diffuse;
  float reflection;
}
material;

/* Lightsource definition */
typedef struct {
  vector pos;
  colour intensity;
}
light;

/* Subtract two vectors and return the resulting vector */
vector vectorSub(vector * v1, vector * v2);

/* Multiply two vectors and return the resulting scalar (dot product) */
/* Example: v•w = vx*wx + vy*wy + vz*wz = number = v*w*cos(ß) */
/* 2i+3j-2k • 3i-2j+3k = (2*3)+(3*(-2))+((-2)*3) = 6 - 6 -6 = -6*/
float vectorDot(vector * v1, vector * v2);

/* Calculate Vector x Scalar and return resulting Vector*/
/*Example 2*(2i+3j+6k) = 4i+6j+12k */
vector vectorScale(float c, vector * v);

/* Add two vectors and return the resulting vector */
/* Example (2i+2j-4k) + (3i+5j+6k) = 5i+7j+2k  */
vector vectorAdd(vector * v1, vector * v2);

/* Check if the ray and sphere intersect */
bool intersectRaySphere(ray* r, sphere* s, float* t);