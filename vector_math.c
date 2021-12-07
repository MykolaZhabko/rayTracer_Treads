#include "vector_math.h"
// #include <stdbool.h> 



/* Subtract two vectors and return the resulting vector */
vector vectorSub(vector * v1, vector * v2) {
  vector result = {
    v1 -> x - v2 -> x,
    v1 -> y - v2 -> y,
    v1 -> z - v2 -> z
  };
  return result;
}

/* Multiply two vectors and return the resulting scalar (dot product) */
/* Example: v•w = vx*wx + vy*wy + vz*wz = number = v*w*cos(ß) */
/* 2i+3j-2k • 3i-2j+3k = (2*3)+(3*(-2))+((-2)*3) = 6 - 6 -6 = -6*/
float vectorDot(vector * v1, vector * v2) {
  return v1 -> x * v2 -> x + v1 -> y * v2 -> y + v1 -> z * v2 -> z;
}

/* Calculate Vector x Scalar and return resulting Vector*/
/*Example 2*(2i+3j+6k) = 4i+6j+12k */
vector vectorScale(float c, vector * v) {
  vector result = {
    v -> x * c,
    v -> y * c,
    v -> z * c
  };
  return result;
}

/* Add two vectors and return the resulting vector */
/* Example (2i+2j-4k) + (3i+5j+6k) = 5i+7j+2k  */
vector vectorAdd(vector * v1, vector * v2) {
  vector result = {
    v1 -> x + v2 -> x,
    v1 -> y + v2 -> y,
    v1 -> z + v2 -> z
  };
  return result;
}

/* Check if the ray and sphere intersect */
bool intersectRaySphere(ray* r, sphere* s, float* t) {

  bool retval = false;

  /* A = d.d, the vector dot product of the direction */
  float A = vectorDot( &r -> dir, &r -> dir);

  /* We need a vector representing the distance between the start of 
   * the ray and the position of the circle.
   * This is the term (p0 - c) 
   */
  vector dist = vectorSub( &r -> start, &s -> pos);

  /* 2d.(p0 - c) */
  float B = 2 * vectorDot( &r -> dir, &dist);

  /* (p0 - c).(p0 - c) - r^2 */
  float C = vectorDot( &dist, &dist) - (s -> radius * s -> radius);

  /* Solving the discriminant */
  float discr = B * B - 4 * A * C;

  /* If the discriminant is negative, there are no real roots.
   * Return false in that case as the ray misses the sphere.
   * Return true in all other cases (can be one or two intersections)
   * t represents the distance between the start of the ray and
   * the point on the sphere where it intersects.
   */
  if (discr < 0)
    retval = false;
  else {
    float sqrtdiscr = sqrtf(discr);
    float t0 = (-B + sqrtdiscr) / (2*A);
    float t1 = (-B - sqrtdiscr) / (2*A);

    /* We want the closest one */
    if (t0 > t1)
      t0 = t1;

    /* Verify t1 larger than 0 and less than the original t */
    if ((t0 > 0.001f) && (t0 < * t)) {
      *t = t0;
      retval = true;
    } else
      retval = false;
  }

  return retval;
}
