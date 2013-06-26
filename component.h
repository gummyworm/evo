/******************************************************************************
 * component.h
 * This file defines the component type, which is the type that defines the 
 * behavior of pretty much everything in the engine.
 * This file should be included by components themselves.
 *
 * Bryce Wilson
 * Created June 24, 2013
 *****************************************************************************/
#ifndef COMPONENT_H
#define COMPONENT_H
#ifdef __cplusplus
extern {
#endif

typedef void (*Component_StartFunc)();
typedef void (*Component_UpdateFunc)();

/**
 * The component structure. The collection of an entities' components defines
 * its behavior. This layout just defines the base structure of the component.
 * When you make your own components, ensure that the first fields match those
 * of this struct (even in order).
 */
typedef struct tagComponent {
    /* the start function, called upon adding this to an entity */
    Component_StartFunc start;
    /* the update function, called once a frame */
    Component_UpdateFunc update;
    /* the entity this componennt is attached to */
    struct Entity* entity;
    /* the ID of the component (tells what type it is) */
    unsigned id;
}Component;


#include "stdcomponents.h"

#ifdef __cplusplus
extern {
#endif
#endif

