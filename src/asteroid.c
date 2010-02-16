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


#include    "asteroid.h"

void add_asteroid(ASTEROID **list, ASTEROID *new_ast)
{
    if (*list != NULL)
        (*list)->prev = new_ast;
    new_ast->next = *list;
    new_ast->prev = NULL;
    *list = new_ast;
}

void delete_asteroid(ASTEROID **list, ASTEROID *ast_to_delete)
{
    if (ast_to_delete == NULL)
        return;

    if (ast_to_delete->prev != NULL)
        ast_to_delete->prev->next = ast_to_delete->next;
    else
        *list = ast_to_delete->next;

    if (ast_to_delete->next != NULL)
        ast_to_delete->next->prev = ast_to_delete->prev;

    free(ast_to_delete->primitive.points);
    free(ast_to_delete);
} 

void break_asteroid(ASTEROID **list, ASTEROID *ast_to_break)
{
    int i;
    ASTEROID *chunk;

    if (ast_to_break == NULL)
        return;

    if (ast_to_break->life > 0)
    {
        for (i=0; i<ASTEROID_CHUNKS; i++)
        {
            chunk = create_asteroid(ast_to_break->primitive.speed * 1.2f, (i * (360.0f / ASTEROID_CHUNKS)) + (rand() % (360 / ASTEROID_CHUNKS)), ast_to_break->radius / 2, ast_to_break->life - 1);
            set_primitive_position(&chunk->primitive, ast_to_break->primitive.centroid.x, ast_to_break->primitive.centroid.y);
            add_asteroid(list, chunk);
        }
    }

    delete_asteroid(list, ast_to_break);
}

ASTEROID *create_asteroid(float speed, float angle, float radius, int life)
{
    int p;
    ASTEROID *ast = calloc(1, sizeof(ASTEROID));
    ast->primitive.size = ASTEROID_POINTS;
    ast->primitive.points = calloc(ast->primitive.size, sizeof(FPOINT));
    ast->primitive.start_ticks = SDL_GetTicks();
    ast->primitive.speed = speed;
    ast->primitive.angle = angle;
    ast->primitive.xVel = speed * cosf(to_radians(angle));
    ast->primitive.yVel = speed * sinf(to_radians(angle));
    ast->primitive.rotation = (rand() % 2 == 0 ? -1 : 1) * (rand() % 10) * 0.2f;
    ast->primitive.lifetime = -1;
    ast->radius = radius;
    ast->life = life;
    to_glcolor(0xFFFFFF, &ast->color);
    for (p=0; p<ASTEROID_POINTS; p++)
    {
        angle = p * (2.0f*PI) / ASTEROID_POINTS;
        ast->primitive.points[p].x = (ast->radius * (float)((3 + (rand() % 7)) / 10.0)) * sinf(angle); 
        ast->primitive.points[p].y = (ast->radius * (float)((3 + (rand() % 7)) / 10.0)) * cosf(angle);
    }
    return ast;
}

void clear_asteroids(ASTEROID **list)
{
    while(*list != NULL)
    {
        delete_asteroid(list, *list);
    }
}
