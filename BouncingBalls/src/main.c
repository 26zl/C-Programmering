/*
* Bouncing Balls Animation
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "drawline.h"
#include "triangle.h"
#include "list.h"
#include "teapot_data.h"
#include "sphere_data.h"
#include "object.h"

/* Two macro's that find the lesser or greater of two values */
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

/*
 * Clear the surface by filling it with 0x00000000(black).
 */
void clear_screen(SDL_Surface *surface)
{
    if(SDL_FillRect(surface, NULL, 0x00000000) < 0){
        fprintf(stderr, "Unable to clear the surface. Error returned: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

/*
 * Accelerate the object; altering its speed based on the boost given.
 */
void accelerate_object(object_t *a, float boost, float maxspeed)
{
    float s;
    float news;
    
    /* Calculate lenght of speed vector */
    s = sqrtf(a->speedx * a->speedx + a->speedy * a->speedy);
    if (s == 0.0f) {
        return;
    }

    /* Boost speed */
    news = s * boost;
    if (news < 0.0)
        news = 0.0;
    if (news > maxspeed)
        news = maxspeed;    
    a->speedx *= news/s;
    a->speedy *= news/s;
}
/*
 * Animate bouncing balls on the screen.
 */
void bouncing_balls(SDL_Window *window)
{
    srand((unsigned int)time(NULL));
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface) {
        fprintf(stderr, "Unable to get window surface: %s\n", SDL_GetError());
        return;
    }
    /* Create list to hold all ball objects */
    list_t *balls = list_create();
    if (!balls) {
        fprintf(stderr, "Failed to create ball list.\n");
        return;
    }
    /* Remove balls 5 seconds after they have settled on the ground */
    const unsigned int BALL_TTL = 5000;
    const float REST_SPEED = 0.50f;
    /* Spawn 10 balls with random speeds */
    const int NUM_BALLS = 10;
    for (int i = 0; i < NUM_BALLS; i++) {
        object_t *ball = create_object(surface, sphere_model, SPHERE_NUMTRIANGLES);
        if (!ball) {
            fprintf(stderr, "Failed to create ball %d\n", i);
            continue;
        }
        /* Give each ball random size, position, and speed */
        ball->scale  = 0.15f + ((float)rand() / (float)RAND_MAX) * 0.15f;  
        int usable_w = MAX(1, surface->w - 200);
        int usable_h = MAX(1, surface->h / 3);
        ball->tx     = (float)(rand() % usable_w) + 100.0f;
        ball->ty     = (float)(rand() % usable_h) + 50.0f;
        ball->speedx = ((float)rand() / (float)RAND_MAX) * 100.0f - 50.0f; 
        ball->speedy = ((float)rand() / (float)RAND_MAX) * 80.0f  - 60.0f; 
        /* TTL starts when the ball comes to rest */
        ball->ttl = 0;
        list_addlast(balls, ball);
    }

    /* Physics constants */
    const float GRAVITY = 0.35f;
    const float AIR     = 0.985f; 
    const float BOUNCE  = 0.78f; 

    /* Main animation loop */
    do {
        int running = 1;
        list_iterator_t *it = list_createiterator(balls);
        if (!it) {
            list_iterator_t *tmp = list_createiterator(balls);
            object_t *bf;
            while (tmp && (bf = list_next(tmp)) != NULL)
                destroy_object(bf);
            if (tmp) 
            list_destroyiterator(tmp);
            list_destroy(balls);
            break;
        }
        while (running) {
            /* Handle input events */
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT)
                    running = 0;
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;
            }
            clear_screen(surface);
            unsigned int current = SDL_GetTicks();

            /* Update and draw each ball */
            object_t *ball;
            while ((ball = list_next(it)) != NULL) {
                /* Remove balls whose lifetime after settling has expired */
                if (ball->ttl > 0 && current >= ball->ttl) {
                    list_remove(balls, ball);
                    destroy_object(ball);
                    continue;
                }
                int r = (int)((500.0f * ball->scale) + 10.0f);

                /* Update physics */
                ball->speedy += GRAVITY;
                ball->speedx *= AIR;
                ball->speedy *= AIR;
                ball->tx += ball->speedx;
                ball->ty += ball->speedy;

                /* Handle collisions with walls */
                if (ball->tx - r < 0) {
                    ball->tx = r;
                    ball->speedx = -ball->speedx * BOUNCE;
                }
                if (ball->tx + r > surface->w) {
                    ball->tx = surface->w - r;
                    ball->speedx = -ball->speedx * BOUNCE;
                }
                if (ball->ty - r < 0) {
                    ball->ty = r;
                    ball->speedy = -ball->speedy * BOUNCE;
                }
                if (ball->ty + r > surface->h) {
                    ball->ty = surface->h - r;
                    ball->speedy = -ball->speedy * BOUNCE;
                }
                /* If the ball is resting on the ground, stop its motion and start/maintain TTL. */
                int ground = (ball->ty + r >= surface->h - 1);
                int resting = ground &&
                              fabsf(ball->speedx) < REST_SPEED &&
                              fabsf(ball->speedy) < REST_SPEED;
                if (resting) {
                    ball->speedx = 0.0f;
                    ball->speedy = 0.0f;
                    ball->ty = surface->h - r;
                    if (ball->ttl == 0) {
                        ball->ttl = current + BALL_TTL;
                    }
                } else if (ball->ttl != 0) {
                    ball->ttl = 0;
                } else {
                    /* Ball is still moving */
                }   
                draw_object(ball);
            }

            /* If no balls remain, stop the animation loop */
            if (list_size(balls) == 0) {
                running = 0;
            }

            /* Reset iterator for next frame */
            list_resetiterator(it);
            SDL_UpdateWindowSurface(window);
            SDL_Delay(1);
        }
        /* Cleanup */
        list_iterator_t *it2 = list_createiterator(balls);
        if (it2) {
            object_t *b2;
            while ((b2 = list_next(it2)) != NULL)
                destroy_object(b2);
            list_destroyiterator(it2);
        }
        list_destroyiterator(it);
        list_destroy(balls);
    } while (0);
}
/*
 * Main program entry point
 */
int main(void)
{
    const size_t bufsize = 100;
    
    /* Change the screen width and height to your own liking */
    const int screen_w = 1600;
    const int screen_h = 900;

    char errmsg[bufsize];
    SDL_Window *window;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        snprintf(errmsg, bufsize, "Unable to initialize SDL.");
        goto error;
    }
    
    /* Create a 1600x900 window */
    window = SDL_CreateWindow("The Amazing Bouncing Balls",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screen_w, screen_h,
                              0);
    if(!window) {
        snprintf(errmsg, bufsize, "Unable to get video surface.");
        goto error;
    }

    /* Start bouncing some balls */
    bouncing_balls(window);

    /* Destroy the window now that we're done */
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    /* Shut down SDL */
    SDL_Quit();

    return 0;

    /* Upon an error, print message and quit properly */
error:
    fprintf(stderr, "%s Error returned: %s\n", errmsg, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}
