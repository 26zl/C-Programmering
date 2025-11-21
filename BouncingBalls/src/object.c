/*
 * Object module: creation, drawing and destruction of renderable objects.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "drawline.h"
#include "triangle.h"
#include "object.h"


/* Return a newly created object with default transform and velocity. */
object_t *create_object(SDL_Surface *surface, triangle_t *model, int numtriangles)
{
    object_t *object;

    if (!surface || !model || numtriangles <= 0) {
        return NULL;
    }

    object = malloc(sizeof(*object));
    if (!object) {
        return NULL;
    }

    object->model = malloc(sizeof(triangle_t) * numtriangles);
    if (!object->model) {
        free(object);
        return NULL;
    }

    memcpy(object->model, model, sizeof(triangle_t) * numtriangles);

    object->surface = surface;
    object->numtriangles = numtriangles;

    object->scale = 1.0f;
    object->rotation = 0.0f;
    object->rotation_way = 0;
    object->tx = 0.0f;
    object->ty = 0.0f;
    object->speedx = 0.0f;
    object->speedy = 0.0f;
    /* Default TTL; used as an absolute expiration timestamp in ms once set. */
    object->ttl = 0;

    return object;
}

/* Destroy the object, freeing its model data and itself. */
void destroy_object(object_t *object)
{
    if (!object) {
        return;
    }

    free(object->model);
    free(object);
}

/* Draw the object on its surface using its triangle model. */
void draw_object(object_t *object)
{
    int i;

    if (!object) {
        return;
    }

    for (i = 0; i < object->numtriangles; i++) {
        triangle_t tri = object->model[i];

        tri.scale = object->scale;
        tri.rotation = object->rotation;
        tri.tx = (int)object->tx;
        tri.ty = (int)object->ty;

        draw_triangle(object->surface, &tri);
    }
} 
