/* 
  Copyright (C) 2010 Gavin Schultz
  
  This file is part of Asterad.
  
  Asterad is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  Asterad is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/ 

//TODO: Military ranking system w/ high score
//TODO: Implement bombs

#ifndef  GAME_INC
#define  GAME_INC

#define	FPS             60.0f			/* frames per second */
#define	DEFAULT_HRES    800			/* default horizontal resolution */
#define DEFAULT_VRES    600         /* default vertical resolution */

#define	DEFAULT_COLORDEPTH  32		/* default color depth */
#define	DEFAULT_FULLSCREEN  0		/* determines whether to default to full-screen mode by default or not */

#define	Y_AXIS          100.0f			/* extent of y-axis */
#define X_AXIS          100.0f           /* extent of x-axis */

#define STARTING_ASTEROIDS  3

#define	BULLET_SPEED    137.0f			/* speed at which the bullet travels */
#define	MAX_BULLETS     50 			/* maximum number of bullets on-screen / in-game at once */
#define	BULLET_RANGE    125.0f			/*  */
#define DEBRIS_LIFE     2500
#define MESSAGE_LIFE    1750

#define SCORE_1UP       150000

#define RESFOLDER       "resources"
#define BGMUSIC         "resources\\bgmusic.mp3"

#include    <stdlib.h>
#include	<stdio.h>
#include    <math.h>
#include    <time.h>
#include    "asteroid.h"
#include	"debug.h"
#include    "hiscore.h"
#include    "geometry2d.h"
#include	"graphics.h"
#include	"SDL.h"
#include    "ship.h"
#include    "sound.h"
#include	"text.h"
#include	"timing.h"
#include    "ufo.h"
#include	"util.h"

typedef enum GameState
{
    GAME_NOT_STARTED,   // Title screen, game has not yet started
    DISPLAY_HIGH_SCORES,    // Showing high scores, game not yet started
    GAME_PLAYING,       // Game started and in progress
    GAME_OVER,          // All ships gone, game over
    LEVEL_FINISHED,     // Current level has finished, transitioning to next one
    ENTER_HIGH_SCORE    // Player has posted a high score, entering name
} GAMESTATE;

enum ShipType
{
    SHIP_PLAYER,
    SHIP_UFO
};

struct Game
{
    char quit;              // 1 = game is quitting
    char paused;            // 1 = game is paused
    GAMESTATE state;        // current state of the game
    uint32_t state_start_time;    // number of ticks since the game has been in the current state
    int32_t ufo_timer;     // number of ticks under UFO appears again
    int32_t message_timer;
    char fullscreen;        // 1 = fullscreen, 0 = windowed 
    char show_bounding_boxes;
    float time_mult;
    int bitdepth;
    int hRes, vRes;
    struct KeyMap
    {
        SDLKey p1_left;
        SDLKey p1_right;
        SDLKey p1_thrust;
        SDLKey p1_fire;
        SDLKey p1_bomb;
        SDLKey p2_left;
        SDLKey p2_right;
        SDLKey p2_thrust;
        SDLKey p2_fire;
        SDLKey p2_bomb;
    } keymap;
    struct HighScore hiscores[MAX_HISCORES];
    int level;
    char message[200];
} game;

typedef struct Bullet
{
    FPOINT point;
    float xVel, yVel;
    float speed;
    float angle;
    float range;    // the distance the bullet can travel before dissipating
    enum ShipType owner_type;
    union {
        SHIP *ship;
        UFO *ufo;
    } owner;
} BULLET;

void init(void);
void init_graphics(void);
void init_level(void);
void cleanup(void);
void shutdown(void);
void build_demo_objects(void);
void start(void);
void reset(void);
void game_over(void);
void move_objects(void);
void move_ship(SHIP *ship);
void detect_collisions(void);
void draw_scene(void);
void draw_ship(SHIP *);
void draw_scores(void);
void draw_lives(struct Ship *);
void draw_asteroid(ASTEROID *);
void draw_primitive_systems(PRIMITIVESYSTEM **);
void draw_primitive(PRIMITIVE *);
void ship_fire(SHIP *);
void ufo_fire(UFO *);
void hit_asteroid(ASTEROID *, BULLET *);
void hit_ship(SHIP *);
void hit_ufo(UFO **, PRIMITIVE *part, BULLET *);
void hit_all_asteroids(void);
void respawn(SHIP *);
void increment_score(SHIP *, int points);
void display_message(const char *, ...);
void handle_keystate(SHIP *);
void handle_event(SDL_Event *);
void check_game_status(void);
void set_game_state(GAMESTATE);
void reset_ufo_timer(void);
void spawn_ufo(void);
PRIMITIVESYSTEM *create_debris(PRIMITIVE *);
PRIMITIVESYSTEM *create_sparks(PRIMITIVE *source, int spark_count, enum RelativePrimitivePosition position, float min_angle_range, float max_angle_range, float max_speed);
int calculate_hit_score(BULLET *, SHIP *, ASTEROID *);
#endif
