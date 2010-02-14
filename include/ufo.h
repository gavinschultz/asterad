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


#ifndef  UFO_INC
#define  UFO_INC

#include    "SDL.h"
#include    "geometry2d.h"

#define     UFO_PRIMITIVE_SIZE          4   // Number of vertices per part
#define     UFO_PRIMITIVE_COUNT         11  // Total number of UFO parts
#define     UFO_COLLIDABLE_PRIMITIVES   3   // Number of UFO parts that can crash into other objects
#define     UFO_BULLET_SPEED            50.0f
#define     UFO_BULLET_RANGE            100.0f
#define     UFO_RELOAD_TIME             1000    // Number of ticks (ms) between shots

enum UFOSkill
{
    UFO_ROOKIE,
    UFO_CAPTAIN,
    UFO_ELITE
};

enum UFOSize
{
    UFO_SMALL = 0,
    UFO_NORMAL = 1,
    UFO_HUGE = 2
};

typedef struct UFO
{
    PRIMITIVE *parts;
    int parts_count;
    char broken;
    enum UFOSkill skill;
    enum UFOSize size;
    int reload_timer;
    int ping_timer;
} UFO;

UFO *create_ufo(float speed, float angle, enum UFOSkill skill, enum UFOSize size, float x, float y);
void break_ufo(UFO **, PRIMITIVE *part_hit);  // breaks the UFO into collidable pieces, which shortly explode
void delete_ufo(UFO **); // deletes the UFO from memory

#endif   /* ----- #ifndef UFO_INC  ----- */
