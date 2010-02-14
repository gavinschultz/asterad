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

#ifndef  ASTEROID_INC
#define  ASTEROID_INC

#define	ASTEROID_POINTS 11			/* number of points on an asteroid */
#define ASTEROID_RADIUS 20.0f
#define ASTEROID_SPEED  42.0f
#define ASTEROID_CHUNKS 2           /* number of chunks when an asteroid breaks */
#define ASTEROID_DEBRIS_LIFE    3000

#include	"graphics.h"
#include    "geometry2d.h"
#include    "debug.h"

typedef struct Asteroid
{
    PRIMITIVE primitive;
    struct OpenGLColor color;         // the color (in hex RGB e.g. 0xFFFFFF) of the ship
    int life;               // number of hits the asteroid needs to take to be completely destroyed
    float radius;           // radius of the asteroid - although not circular, radius still determines the size
    struct Asteroid *next;
    struct Asteroid *prev;
} ASTEROID;

ASTEROID *create_asteroid(float speed, float angle, float radius, int life);
void add_asteroid(ASTEROID **list, ASTEROID *new_ast);
void delete_asteroid(ASTEROID **list, ASTEROID *to_delete);
void clear_asteroids(ASTEROID **list);
void break_asteroid(ASTEROID **list, ASTEROID *to_break);

#endif
