#include "guilayout.h"

static void FreeWidgetRecursive(gpointer data, gpointer user_data);

void GUILayoutInit()
{

}

void GUILayoutAddWidget(Widget* w, Widget* p)
{
    g_slist_append(w->children, (gpointer)p);
}

void GUILayoutRemoveWidget(Widget* w)
{
    /* remove link from parent to this widget */
    if(w->parent) {
        g_slist_remove(w->parent->children, (gpointer)w);
    }
    /* remove this widget and all its children */
    FreeWidgetRecursive((gpointer)w->children, NULL);
}

Widget* GUILayoutNewWidget(Model* background, Model* contents)
{
    Widget* w = (Widget*)malloc(sizeof(Widget));
    w->contents = contents;
    w->background = background;
    w->children = NULL;
    return w;
}

void FreeWidgetRecursive(gpointer data, gpointer user_data)
{
    Widget* w = (Widget*)data;

    ModelFree(w->contents);
    ModelFree(w->background);

    /* recursively free all the subwidgets of this widget */
    if(w->children != NULL) {
        g_slist_foreach(w->children, FreeWidgetRecursive, NULL);
    }
    free(w);
}

