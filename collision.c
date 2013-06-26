#include "collision.h"

GList* colliding = NULL;
static GHashTable* collidingY = NULL;
static GHashTable* collidingZ = NULL;

/* lists of all the colldiers sorted on various axes for fast detection */
static GList* colliders;
static GList* xSorted;
static GList* ySorted;
static GList* zSorted;

/* equality function for collision comparison */
gboolean ColEqual(gconstpointer a, gconstpointer b);

/* comparison functions for sorting */
static gint XCompare(gconstpointer a, gconstpointer b);
static gint YCompare(gconstpointer a, gconstpointer b);
static gint ZCompare(gconstpointer a, gconstpointer b);


void Collision_Init()
{
    colliders = NULL;
}

void Collision_AddCollider(Component_Collider* col)
{
    colliders = g_list_append(colliders, col);
    xSorted = g_list_insert_sorted(xSorted, (gpointer)col, XCompare);
    ySorted = g_list_insert_sorted(ySorted, (gpointer)col, YCompare);
    zSorted = g_list_insert_sorted(zSorted, (gpointer)col, ZCompare);
}

void Collision_RemoveCollider(Component_Collider* col)
{
    /* get that s*** outta' here */
    colliders = g_list_remove(colliders, (gpointer)col);
    xSorted = g_list_remove(xSorted, (gpointer)col);
    ySorted = g_list_remove(ySorted, (gpointer)col);
    zSorted = g_list_remove(zSorted, (gpointer)col);
}

void Collision_Detect()
{
    GList* i;
    GList* j;

    Collision* col;
    Component_Collider* c1;
    Component_Collider* c2;

    /* direct hash, custom equal func */
    collidingY = g_hash_table_new(NULL, ColEqual);
    collidingZ = g_hash_table_new(NULL, ColEqual);

    GList* possibleCollisions = NULL;

    /* Check X */
    for(i = xSorted, j = g_list_next(xSorted); i != NULL; j = g_list_next(j)) {
        /* end of the list? */
        if(j == NULL) {
            j = i = g_list_next(i);
        }
        else {
            c1 = (Component_Collider*)(i->data);
            c2 = (Component_Collider*)(j->data);

            /* not overlapping? */
            if((c1->transform->x + c1->aabb.w) <
                     (c2->transform->x)) {
                j = i = g_list_next(i);
            }
            /* X is overlapping */
            else {
                Collision* c = (Collision*)malloc(sizeof(Collision));
                c->col1 = c1;
                c->col2 = c2;
                possibleCollisions = g_list_append(possibleCollisions, (gpointer)c1);
            }
        }
    }
    /* Check Y */
    for(i = ySorted, j = g_list_next(ySorted); i != NULL; j = g_list_next(j)) {
        if(j == NULL) {
            j = i = g_list_next(i);
        }
        else {
            c1 = (Component_Collider*)(i->data);
            c2 = (Component_Collider*)(j->data);
            /* not overlapping? */
            if((c1->transform->y + c1->aabb.h) < 
                    (c2->transform->y)) {
                j = i = g_list_next(i);
            }
            /* overlapping Y */
            else {
                Collision* c = (Collision*)malloc(sizeof(Collision));
                c->col1 = c1;
                c->col2 = c2;
                /* insert both this collision and the reverse */
                g_hash_table_insert(collidingY, (gpointer)c1, (gpointer)c);
                g_hash_table_insert(collidingY, (gpointer)c2, (gpointer)c);
            }
        }
    }

    /* Check Z */
    for(i = zSorted, j = g_list_next(zSorted); i != NULL; j = g_list_next(j)) {
        if(j == NULL) {
            j = i = g_list_next(i);
        }
        else {
            c1 = (Component_Collider*)(i->data);
            c2 = (Component_Collider*)(j->data);
            /* not overlapping? */
            if((c1->transform->z + c1->aabb.d) <
                    (c2->transform->z)) {
                j = i = g_list_next(i);
            }
            /* overlapping Z */
            else {
                Collision* c = (Collision*)malloc(sizeof(Collision));
                c->col1 = c1;
                c->col2 = c2;
                /* insert both this collision and the reverse */
                g_hash_table_insert(collidingZ, (gpointer)c1, (gpointer)c);
                g_hash_table_insert(collidingZ, (gpointer)c2, (gpointer)c);
            }
        }
    }
    
    /* check for matches on all axes, x axis overlap is implicit */
    for(i = possibleCollisions; i != NULL; i = g_list_next(i)) {
        /* check Y overlap */
        if(g_hash_table_lookup(collidingY, i->data) != NULL) {
            /* check Z overlap */
            col = (Collision*)g_hash_table_lookup(collidingZ, i->data);
            if(col != NULL) {
                /* collision occurred */
                colliding = g_list_append(colliding, (gpointer)i);
                printf("collision between {%f,%f,%f} and {%f,%f,%f}\n", 
                        col->col1->transform->x, col->col1->transform->y, col->col1->transform->z,
                        col->col2->transform->x, col->col2->transform->y, col->col2->transform->z);
                fflush(stdout);
            }
        }
    }
}

void Collision_Update()
{

}

gboolean ColEqual(gconstpointer a, gconstpointer b)
{
    Collision* c1 = (Collision*)a;
    Collision* c2 = (Collision*)b;
    if(((c1->col1 == c2->col1) && (c1->col2 == c2->col2)) ||
            ((c1->col2 == c2->col1) && (c1->col1 == c2->col2))) {
        return TRUE;
    }
    return FALSE;
}

gint XCompare(gconstpointer a, gconstpointer b)
{
    Component_Collider* c1 = (Component_Collider*)a;
    Component_Collider* c2 = (Component_Collider*)b;
    if(c1->transform->x < c2->transform->x) {
        return -1;
    }
    else if(c1->transform->x > c2->transform->x) {
        return 1;
    }
    else {
        return 0;
    }
}

gint YCompare(gconstpointer a, gconstpointer b)
{
    Component_Collider* c1 = (Component_Collider*)a;
    Component_Collider* c2 = (Component_Collider*)b;
    if(c1->transform->y < c2->transform->y) {
        return -1;
    }
    else if(c1->transform->y > c2->transform->y) {
        return 1;
    }
    return 0;
}

gint ZCompare(gconstpointer a, gconstpointer b)
{
    Component_Collider* c1 = (Component_Collider*)a;
    Component_Collider* c2 = (Component_Collider*)b;
    if(c1->transform->z < c2->transform->z) {
        return -1;
    }
    else if(c1->transform->z > c2->transform->z) {
        return 1;
    }
    return 0;
}

