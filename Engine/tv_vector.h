#ifndef TV_VECTOR_H
#define TV_VECTOR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "tv_types.h"

/**
 * A basic type for representing a point in 2-dimensional space.
 */
typedef struct tv_vector2 {
    float x, y;
}tv_vector2;

/**
 * A basic type for representing a point in 3-dimensional space.
 */
typedef struct tv_vector3 {
    float x, y, z;
}tv_vector3;

/**
 * A type for representing the position, scale, and rotation of an object.
 */
typedef struct tagTvTransform {
	tv_vector3 position;
	tv_vector3 scale;
	tv_vector3 rotation;
}TvTransform;

/**
 * A type for representing a rectangle.
 */
typedef struct tagRect {
    float x, y;
    float w, h;
}tv_rect;

/**
 * Checks if the given rectangle contains the given point.
 * @param r the rectangle to check for the point within.
 * @param point the point to determine if is in the bounds of the rect or not.
 * @return TRUE if the rectangle does contain the point, else FALSE.
 */
tvbool tv_rect_contains(tv_rect r, tv_vector2 point);
/**
 * Checks if the given rectangle overlaps the other given rectangle.
 * @param r1 the first rectangle.
 * @param r2 the...second...rectangle.
 * @return TRUE if the rectangles overlap, else FALSE.
 */
tvbool tv_rect_overlaps(tv_rect* r1, tv_rect* r2);

/**
 * A basic type for representing a point in 4-dimensional space
 */
typedef struct tv_vector4 {
	float x, y, z, w;
}tv_vector4;
typedef tv_vector4 tv_quaternion;

extern const tv_vector2 tv_vector2_zero;
extern const tv_vector3 tv_vector3_zero;
extern const tv_vector4 tv_vector4_zero;

extern const tv_vector3 tv_vector3_forward;
extern const tv_vector4 tv_vector4_forward;

/* new */
tv_vector2 tv_vector2_new(tvfloat x, tvfloat y);
tv_vector3 tv_vector3_new(tvfloat x, tvfloat y, tvfloat z);
tv_vector4 tv_vector4_new(tvfloat x, tvfloat y, tvfloat z, tvfloat w);

/* add */
void tv_vector2_add(tv_vector2 v1, tv_vector2 v2, tv_vector2* result);
void tv_vector3_add(tv_vector3 v1, tv_vector3 v2, tv_vector3* result);
void tv_vector4_add(tv_vector4 v1, tv_vector4 v2, tv_vector4* result);

/* sub */
void tv_vector2_sub(tv_vector2 v1, tv_vector2 v2, tv_vector2* result);
void tv_vector3_sub(tv_vector3 v1, tv_vector3 v2, tv_vector3* result);
void tv_vector4_sub(tv_vector4 v1, tv_vector4 v2, tv_vector4* result);

/* normalize */
void tv_vector2_normalize(tv_vector2 v, tv_vector2* result);
void tv_vector3_normalize(tv_vector3 v, tv_vector3* result);

/* scale */
void tv_vector2_scale(tv_vector2 *v, tvfloat factor);
void tv_vector3_scale(tv_vector3 *v, tvfloat factor);
void tv_vector4_scale(tv_vector4 *v, tvfloat factor);

/* cross product */
tvfloat tv_vector2_cross(tv_vector2* v1, tv_vector2* v2);
void tv_vector3_cross(tv_vector3* v1, tv_vector3* v2, tv_vector3* result);
void tv_vector4_cross(tv_vector4* v1, tv_vector4* v2, tv_vector4* result);

/* dot-product */
tvfloat tv_vector2_dot(tv_vector2 v1, tv_vector2 v2);
tvfloat tv_vector3_dot(tv_vector3 v1, tv_vector3 v2);
tvfloat tv_vector4_dot(tv_vector4 v1, tv_vector4 v2);

/* distance */
tvfloat tv_vector2_distance(tv_vector2 v1, tv_vector2 v2);
tvfloat tv_vector3_distance(tv_vector3 v1, tv_vector3 v2);
tvfloat tv_vector4_distance(tv_vector4 v1, tv_vector4 v2);

/* magnitude */
tvfloat tv_vector2_mag(tv_vector2* v);
tvfloat tv_vector3_mag(tv_vector3* v);

/**
 * Get the unit direction vector from v1 to v2.
 * @param v1 the "start" vector.
 * @param v2 the "destination" vector.
 * @return a unit vector representing the direction of v1 to v2.
 */
void tv_vector3_direction(tv_vector3 v1, tv_vector3 v2, tv_vector3* result);

/**
 * Linearly interpolates between the given vectors.
 * @param v1 the first vector to interpolate from.
 * @param v2 the vector to interpolate to.
 * @param t the time (0-1)
 * @return the new vector 
 */
tv_vector2 tv_vector2_lerp(tv_vector2 v1, tv_vector2 v2, float t);

/**
 * Linearly interpolates between the given 3-D vectors.
 * @param v1 the "start" vector.
 * @param v2 the "destination" vector.
 * @param t the time (0-1)
 * @return the new vector
 */
tv_vector3 tv_vector3_lerp(tv_vector3 v1, tv_vector3 v2, float t);

/**
 * Linearly interpolates between the given 4-D vectors.
 * @param v1 the "start" vector.
 * @param v2 the "destination" vector.
 * @param t the time (0-1)
 * @return the new vector
 */
tv_vector4 tv_vector4_lerp(tv_vector4 v1, tv_vector4 v2, float t);

extern UT_icd tv_vector2_icd;
extern UT_icd tv_vector3_icd;
extern UT_icd tv_vector4_icd;

#ifdef __cplusplus
}
#endif
#endif