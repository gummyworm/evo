/******************************************************************************
 * draw.h
 * This file defines all functions used for rendering within the engine.
 * In theory, this code contains the majority of the platform dependent code in
 * the engine though you'll find some in material.h as well.
 * This should be at least up to 4.0 standard of OpenGL.
 * 
 * Bryce Wilson
 * Created: April, 2013
 * Last Modified: May 4, 2013
 *****************************************************************************/
#ifndef _DRAW_H
#define _DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "glib.h"
#include "material.h"
#include "model.h"
#include "matrix.h"
#include "guilayout.h"

typedef float Vector2[2];
typedef float Vector3[3];


typedef struct tagDrawTarget {
    /* the framebuffer ID of the target. */
    GLuint fbID;
    /* the ID of the texture to render to (if any). */
    GLuint texID;
}DrawTarget;

typedef struct tagMap {
    guint32 width, height;
}Map;

typedef struct tagCamera {
    Vector3 pos;
    Vector2 rot;
    float fov;
    float aspect;
    float near;
    float far;
}Camera;


/**
 * Initialize the libraries needed for rendering. 
 * @return the success of the initialization - 0 on success, !0 on error.
 */
int DrawInit();

/**
 * Clean up all libraries and allocations of the rendering system.
 */
void DrawQuit();

/**
  * Clear the screen and do anything else necessary before beginning 
  * to render a frame.
  */
void DrawStartFrame();

/**
 * Generate a new target to draw to.
 * @return the new target.
 */
DrawTarget* DrawNewTarget();

/**
 * Sets the target of the current render.
 * @param target the target to render to. NULL = back buffer.
 */
void DrawSetTarget(DrawTarget* target);

/**
 * Generate handles to VBOs for the given model.
 * You must call this function before calling DrawModel.
 * @param m the model to generate VBO ID's for.
 * @param attributes flags for each attribute to generate for.
 */
void DrawOptimizeModel(Model* m);

/**
 * Draw GUI ontop of the currently rendered scene.
 * Draw the root widget of the GUILayout system ontop of whatever was last 
 * rendered.
 */
void DrawGUI();

/**
 * Draw the given model.
 * Draws the model m, buffering its data to the GPU if this is the first time
 * that the model has been rendered.
 * @param m the model to draw.
 */
void DrawModel(Model *m);

/**
 * Move the camera by the given X,Y,Z amounts.
 * @param x the amount to move the camera in the X direction.
 * @param y the amount to move the camera in the Y direction.
 * @param z the amount to move the camera in the Z direction.
 */
void DrawMoveCamera(float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif
