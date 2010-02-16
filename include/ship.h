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
  along with Asterad.  If not, see <http://www.gnu.org/licenses/>.
*/ 

#ifndef  SHIP_INC
#define  SHIP_INC

#include    "geometry2d.h"
#include    "timing.h"
#include    "sound.h"

#define SHIP_TURN_SPEED 210.0f      /* default turning speed of the ship (degrees per second) */
#define SHIP_MAX_SPEED  72.0f       /* maximum speed for the ship, units per second */
#define SHIP_ACCEL      120.0f      /* rate of ship acceleration, units per second */
#define SHIP_POINTS     3           /* number of geometric points comprising the ship */

typedef struct Ship
{
    PRIMITIVE primitive;    // Main ship body
    float acceleration;     // Acceleration speed of the ship
    float w, h;             // Width / height of the ship
    int bombs;              // number of bombs the ship has
    int lives;              // number of lives remaining, 0 = game over
    char dead;              // not game over, just waiting for a respawn...
    long int score;         // Play score
    char thrusting;         // 1 = thrust is on
    uint32_t thrusting_time; // number of ticks thrust has been on for
    PRIMITIVE flare;        // Thrust flare behind the ship
    float flare_tip;        // Position of the flare tip; the longer the thrust is held, the further the flare extends from the ship
} SHIP;

void init_ships(void);
SHIP *create_ship(float x, float y);
void delete_ship(SHIP **);
void ship_thrust(SHIP *, int on);

// Made available externally for the benefit of drawing routines
extern const float ship_geometry[10];
extern const float flare_geometry[6];

#endif
