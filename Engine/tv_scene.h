/*****************************************************************************/
/* scene.h                                                                   */
/* This file contains definitions for functions to manage the "scene" in the */
/* engine. The scene is a spatial representation of the objects to be        */
/* rendered. When a Model or Widget component is added to the engine, it gets*/
/* added automatically to the scene system.                                  */
/* Bryce Wilson                                                              */
/* created: June 18, 2013                                                    */
/*****************************************************************************/
#ifndef _SCENE_H
#define _SCENE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "tv_types.h"
#include "camera.h"
#include "tv_collider.h"
#include "tv_entity.h"
#include "tv_light.h"
#include "model.h"
#include "tv_vector.h"

#define TV_SCENE_MAX_LIGHTS 1024

typedef tvint (*tv_scene_sort_func)(const void* a, const void* b);

/**
 * Initialize the scene system.
 * @return zero on success, nonzero on failure.
 */
tvint tv_scene_init();

/**
 * Destroy all items in the scene and clean up.
 * @return zero on success, nonzero on failure.
 */
tvint tv_scene_quit();

/** 
 * Add the given light to the scene.
 * @param light the light to add to the scene.
 */
void tv_scene_add_light(tv_light *light);

/**
 * Get the array of lights currently in the scene.
 * @return an array of the lights in the scene.
 */
tv_array *tv_scene_lights();

/**
 * Get the lights that effect the given position.
 * @param pos the location to get the lights closest to.
 * @return the lights that effect the given position.
 */
tv_array *tv_scene_lights_at(tv_vector3 pos);

/**
 * Add the given entity to the scene.
 * @param entity a pointer to the entity to add to the scene.
 */
void tv_scene_add_entity(tv_entity *entity);
/** 
 * Get the array of all the entities in the scene.
 * @return a pointer to an array of pointers to the entities in the scene.
 */
tv_array *tv_scene_entities();

/**
 * Set the sort function for entities that are added to the scene.
 * @param a comparison function that determines how the entities in the scene
 *  are stored.
 */
void tv_scene_set_entity_sort_func(tv_scene_sort_func func);

/**
 * Set the light sorting function for the scene.
 * @param a comparison function that determines how lights in the scene are
 *  stored.
 */
void tv_scene_set_light_sort_func(tv_scene_sort_func func);

/**
 * Update the scene.
 */
void tv_scene_update();

/**
 * Returns the screen coordinates of the given position.
 * @param scene_coordinates the scene coordinates to find the screen 
 *  coordinates of.
 * @return the screen coordinates of the given scene coordinates.
 */
tv_vector2 tv_scene_to_screen_coordinates(tv_vector3 scene_coordinates);

/**
 * Returns the scene coordinates of the given screen position.
 * @param screen_coordinates the screen coordinates to find the scene
 *  coordinates of.
 * @return the scene coordinates of the given screen coordinates.
 */
tv_vector3 tv_screen_to_scene_coordinates(tv_vector2 screen_coordinates);

/** 
 * Cast a ray from the given position in the given direction for the given 
 * length distance.
 * @param start the origin of the line segment to cast.
 * @param dir the direction (unit-vector) of the line.
 * @param len the length of the line to cast (up to TV_INF).
 * @return an array of tv_colliders that were intersected by the ray.
 */
tv_array* tv_scene_raycast(tv_vector3 start, tv_vector3 dir, tvfloat len);

/**
 * Create a ray and perform a tv_scene_raycast to find collisions.
 * @param screen_coordinates the 2D window coordinates to pick at.
 * @return an array of tv_colliders that the ray intersected. 
 */
tv_array* tv_scene_raypick(tv_vector2 screen_coordinates);

#ifdef __cplusplus
}
#endif
#endif

