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


#include	"ufo.h"

// bottom, middle, top, 3 windows
static const float ufo_geometry[UFO_PRIMITIVE_COUNT * UFO_PRIMITIVE_SIZE * 2] = {
    0.0f, 2.0f, 2.0f, 0.0f, 7.0f, 0.0f, 9.0f, 2.0f,
    0.0f, 2.0f, 2.0f, 4.0f, 7.0f, 4.0f, 9.0f, 2.0f,
    3.0f, 4.0f, 3.0f, 5.0f, 6.0f, 5.0f, 6.0f, 4.0f,
    2.0f, 2.5f, 2.0f, 3.5f, 3.0f, 3.5f, 3.0f, 2.5f,
    4.0f, 2.5f, 4.0f, 3.5f, 5.0f, 3.5f, 5.0f, 2.5f,
    6.0f, 2.5f, 6.0f, 3.5f, 7.0f, 3.5f, 7.0f, 2.5f,
    2.25f, 0.0f, 2.25f, 0.5f, 2.75f, 0.5f, 2.75f, 0.0f,
    3.25f, 0.0f, 3.25f, 0.5f, 3.75f, 0.5f, 3.75f, 0.0f,
    4.25f, 0.0f, 4.25f, 0.5f, 4.75f, 0.5f, 4.75f, 0.0f,
    5.25f, 0.0f, 5.25f, 0.5f, 5.75f, 0.5f, 5.75f, 0.0f,
    6.25f, 0.0f, 6.25f, 0.5f, 6.75f, 0.5f, 6.75f, 0.0f
};
static const float ufo_centroids[UFO_PRIMITIVE_COUNT * 2] = {
    4.5f, 1.0f,
    4.5f, 3.0f,
    4.5f, 4.5f,
    2.5f, 3.0f,
    4.5f, 3.0f,
    6.5f, 3.0f,
    2.5f, 0.25f,
    3.5f, 0.25f,
    4.5f, 0.25f,
    5.5f, 0.25f,
    6.5f, 0.25f
};
static const int ufo_part_explosion_points[UFO_PRIMITIVE_COUNT] = {
    100, 70, 70, 30, 30, 30, 10, 10, 10, 10, 10
};
static const float ufo_size_factors[] = {
    0.8f, 1.3f, 2.5f
};
static const int ufo_primitive_count = sizeof(ufo_geometry) / sizeof(float) / (UFO_PRIMITIVE_SIZE * 2);

UFO *create_ufo(float speed, float angle, enum UFOSkill skill, enum UFOSize size, float x, float y)
{
    int i, j;

    UFO *ufo = calloc(1, sizeof(UFO));
    ufo->skill = skill;
    ufo->size = size;
    ufo->reload_timer = 400;
    ufo->ping_timer = 2000;
    ufo->parts_count = ufo_primitive_count;
    ufo->parts = create_primitives(ufo->parts_count, UFO_PRIMITIVE_SIZE);

    for (i=0; i<ufo_primitive_count; i++)
    {
        ufo->parts[i].centroid.x = x + ufo_centroids[i*2] * ufo_size_factors[size];
        ufo->parts[i].centroid.y = y + ufo_centroids[(i*2)+1] * ufo_size_factors[size];
        for (j=0; j<UFO_PRIMITIVE_SIZE; j++)
        {
            ufo->parts[i].points[j].x = x + ufo_geometry[(i*2*UFO_PRIMITIVE_SIZE) + (j*2)] * ufo_size_factors[size];
            ufo->parts[i].points[j].y = y + ufo_geometry[(i*2*UFO_PRIMITIVE_SIZE) + (j*2) + 1] * ufo_size_factors[size];
        }
        ufo->parts[i].lifetime = -1;
        ufo->parts[i].angle = angle;
        ufo->parts[i].speed = speed;
        ufo->parts[i].xVel = speed * cosf(to_radians(angle));
        ufo->parts[i].yVel = speed * sinf(to_radians(angle));
        ufo->parts[i].area = get_polygon_area(ufo->parts[i].points, ufo->parts[i].size);
    }

    return ufo;
}

void break_ufo(UFO **ufop, PRIMITIVE *part_hit)
{
    int i;
    float ufo_vector = 0.0f;
    char ufo_valid = 0;
    UFO *ufo = *ufop;

    if (ufo == NULL)
        return;

    if (ufo->broken)
    {
        part_hit->lifetime = 0;
        for (i=0; i<ufo->parts_count; i++)
        {
            if (ufo->parts[i].lifetime)
            {
                ufo_valid = 1;
                break;
            }
        }
        if (!ufo_valid)
        {
            delete_ufo(&ufo);
            *ufop = NULL;
        }
    }
    else
    {
        ufo->broken = 1;
        ufo_vector = to_degrees(atan2f(part_hit->yVel, part_hit->xVel));
        for (i=0; i<ufo->parts_count; i++)
        {
            ufo->parts[i].angle = ufo_vector + ((rand() % 2 == 0 ? -1 : 1) * (5 + (rand() % 25)));
            ufo->parts[i].xVel = ufo->parts[i].speed * cosf(to_radians(ufo->parts[i].angle));
            ufo->parts[i].yVel = ufo->parts[i].speed * sinf(to_radians(ufo->parts[i].angle));
            ufo->parts[i].rotation = (float)(rand() % 10);
            ufo->parts[i].lifetime = SDL_GetTicks() - ufo->parts[i].start_ticks + 1000 + (rand() % 1500);
        }
    }
}

void delete_ufo(UFO **ufop)
{
    int i;
    UFO *ufo = *ufop;

    if (ufo == NULL)
        return;

    if (ufo->parts != NULL)
    {
        for (i=0; i<ufo->parts_count; i++)
        {
            free(ufo->parts[i].points);
        }
        free(ufo->parts);
    }
    free(ufo);
    *ufop = NULL;
}
