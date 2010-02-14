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

#include	"ship.h"

const float ship_geometry[] = {
    0.0f, 0.0f, 1.9f, 5.6f, 3.8f, 0.0f, 3.2f, 0.5f, 0.6f, 0.5f
};
const float flare_geometry[] = {
    0.8f, 0.5f, 1.9f, 0.0f, 3.0f, 0.5f
};
static const float ship_centroid_x = 1.9f;
static const float ship_centroid_y = 2.8f;
static const float max_flare_tip = 2.0f;

static int channel_start_thrust = INVALID_CHANNEL;
static int channel_continue_thrust = INVALID_CHANNEL;
static int channel_stop_thrust = INVALID_CHANNEL;
static Mix_Chunk *chunk_start_thrust = NULL;
static Mix_Chunk *chunk_continue_thrust = NULL;
static Mix_Chunk *chunk_stop_thrust = NULL;

void init_ships()
{
    chunk_start_thrust = Mix_LoadWAV("resources\\ship_thrust_begin.wav");
    chunk_continue_thrust = Mix_LoadWAV("resources\\ship_thrust_main.wav");
    chunk_stop_thrust = Mix_LoadWAV("resources\\ship_thrust_end.wav");
}

SHIP *create_ship(float x, float y)
{
    int i, j, k;
    SHIP *ship = NULL;

    ship = calloc(1, sizeof(SHIP));
    ship->primitive.lifetime = -1;
    ship->primitive.size = sizeof(ship_geometry) / sizeof(float) / 2;
    ship->primitive.centroid.x = x;
    ship->primitive.centroid.y = y;
    ship->primitive.points = calloc(ship->primitive.size, sizeof(FPOINT));
    for (i=0; i<ship->primitive.size; i++)
    {
        j = i*2;
        k = (i*2)+1;
        ship->primitive.points[i].x = x + ship_centroid_x - ship_geometry[j];
        ship->primitive.points[i].y = y + ship_centroid_y - ship_geometry[k];
        point2_rotate(&ship->primitive.points[i], to_radians(90.0f), ship->primitive.centroid.x, ship->primitive.centroid.y);
        if (ship->w < ship_geometry[j])
            ship->w = ship_geometry[j];
        if (ship->h < ship_geometry[k])
            ship->h = ship_geometry[k];
    }
    ship->acceleration = SHIP_ACCEL;
    ship->lives = 3;
    ship->bombs = 2;

    ship->flare.lifetime = -1;
    ship->flare.size = sizeof(flare_geometry) / sizeof(float) / 2;
    ship->flare.centroid.x = x;
    ship->flare.centroid.y = y;
    ship->flare.points = calloc(ship->flare.size, sizeof(FPOINT));
    for (i=0; i<ship->flare.size; i++)
    {
        j = i*2;
        k = (i*2)+1;
        ship->flare.points[i].x = x + ship_centroid_x - flare_geometry[j];
        ship->flare.points[i].y = y + ship_centroid_y - flare_geometry[k];
        point2_rotate(&ship->flare.points[i], to_radians(90.0f), ship->flare.centroid.x, ship->flare.centroid.y);
    }
    return ship;
}

void delete_ship(SHIP **shipp)
{
    if (*shipp == NULL)
        return;

    free((*shipp)->primitive.points);
    free((*shipp)->flare.points);
    free((*shipp));
    *shipp = NULL;
}

void ship_thrust(SHIP *ship, int on)
{
    if (on)
        ship->flare_tip += 0.13f;
    else
        ship->flare_tip -= 0.5f;

    if (ship->flare_tip >= max_flare_tip && on)
        ship->flare_tip -= 0.3f + randf(0.4f);
    else if (ship->flare_tip < 0.0f)
        ship->flare_tip = 0.0f;

    ship->flare.invisible = (ship->flare_tip <= 0.0f);
    ship->flare.points[1].x = ship->primitive.centroid.x - ((ship_centroid_x + ship->flare_tip) * cosf(to_radians(ship->primitive.angle)));
    ship->flare.points[1].y = ship->primitive.centroid.y - ((ship_centroid_y + ship->flare_tip) * sinf(to_radians(ship->primitive.angle)));
    //    ship->flare.points[1].y = ship->primitive.centroid.y - (ship_centroid_y * sinf(to_radians(ship->primitive.angle))) - (ship->flare_tip * sinf(to_radians(ship->primitive.angle)));

    if (on)
    {
//        stop_channel(channel_stop_thrust);
//        trace("Stopping channel_stop_thrust");
        if (ship->thrusting_time == 0)
        {
            channel_start_thrust = play_chunk(chunk_start_thrust, 0);
            trace("Starting channel_start_thrust");
        }
        else if (ship->thrusting_time > 300 && channel_continue_thrust == INVALID_CHANNEL)
        {
            stop_channel(&channel_start_thrust);
            trace("Stopping channel_start_thrust");
            channel_continue_thrust = play_chunk(chunk_continue_thrust, LOOP_FOREVER);
            trace("Starting channel_continue_thrust");
        }

        ship->thrusting_time += get_elapsed_ticks();
//        trace("Ship thrusting for %d ticks", ship->thrusting_time);
    }
    else
    {
        if (ship->thrusting_time > 300)
        {
            channel_stop_thrust = play_chunk(chunk_stop_thrust, 0);
            trace("Starting channel_stop_thrust");
        }
        stop_channel(&channel_start_thrust);
        stop_channel(&channel_continue_thrust);
        ship->thrusting_time = 0;
    }
}
