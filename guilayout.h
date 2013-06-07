/**
 * This file contains definitions for functions used to compose the GUI.
 * This includes high level objects such as widgets, text boxes, etc.
 * No code to actually render said objects should be put here. As with
 * all other rendered items, that belongs in draw.h/draw.c 
 * Bryce Wilson
 * 6/5/2013
 */
#ifndef _GUILAYOUT_H
#define _GUILAYOUT_H

#ifndef __cplusplus
extern "C" {
#endif

#include <glib.h>

/**
 * The widget structure.
 * Widgets are physically similar to models, but carry a little extra
 * information to assist in prettily laying out the GUI. When setting the
 * coordinates of the model within the widget, use a scale from 0 to 1 to allow
 * for flexible sizing on monitors of all resolutions.
 */
typedef struct tagWidget {
    /* the minimum dimensions of the widget */
    float minW, minH;
    /* maximum dimensions of the widget or 0 for no limit */
    float maxW, maxH;
    /* the background that is displayed by this widget - not scrolled */
    Model* background;
    /* the contents that this window displays - can be scrolled. */
    Model* contents
    /* the children of the widget - a list of widgets */
    GSList* children;
    /* the parent widget of this widget. NULL if this is the root widget */
    Widget* parent;
}Widget;

/**
 * Initialize the GUILayout system.
 */
void GUILayoutInit();

/**
 * Create a new widget.
 * @param background the background to draw for this widget.
 * @param contents the contents to display within the widget.
 */
Widget* GUILayoutNewWidget(Model* background, Model* contents);

/**
 * Create a textbox widget.
 * @param background the background to use for the textbox.
 * @param text the text to display within the textbox.
 * @return a textbox widget containing the specified text.
 */
Widget* GUILayoutNewTextBox(Model* background, char* text);

/**
 * Add the specified widget to the specified parent widget. 
 * @param w the widget to add.
 * @param p the widget to parent this widget to, or NULL for root widget.
 * @param x the x location to add the widget at (range 0-1).
 * @param y the y location to add the widget at (range 0-1).
 */
void GUILayoutAddWidget(Widget* w, Widget* p, float x, float y);

/**
 * Remove the specified widget from the GUILayout system.
 * The widget is deleted upon removal and its resources freed.
 * @param w the widget to remove.
 */
void GUILayoutRemoveWidget(Widget* w);

#ifndef __cplusplus
}
#endif
#endif

