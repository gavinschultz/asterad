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

#include    "geometry2d.h"

static float _x_axis;
static float _y_axis;
static float _time_mult = 1.0f;

void set_axes(float x, float y)
{
    _x_axis = x;
    _y_axis = y;
}

void set_time_mult(float t)
{
    _time_mult = t;
}

PRIMITIVE *create_primitives(int num_of_primitives, int primitive_size)
{
    PRIMITIVE *p;
    int i;
    uint32_t start_ticks = SDL_GetTicks();

    if (num_of_primitives <= 0 || primitive_size <= 0)
        return NULL;

    p = calloc(num_of_primitives, sizeof(PRIMITIVE));
    for (i=0; i<num_of_primitives; i++)
    {
        p[i].start_ticks = start_ticks;
        p[i].size = primitive_size;
        p[i].points = calloc(primitive_size, sizeof(FPOINT));
    }
    return p;
}

PRIMITIVESYSTEM *create_primitive_system(int primitive_count, int primitive_size)
{
    PRIMITIVESYSTEM *ps;

    if (primitive_count == 0 || primitive_size == 0)
        return NULL;

    ps = calloc(1, sizeof(PRIMITIVESYSTEM));
    ps->size = primitive_count;
    ps->primitives = create_primitives(primitive_count, primitive_size);
    return ps;
}

void add_primitive_system(PRIMITIVESYSTEM **list, PRIMITIVESYSTEM *ps)
{
    if (list == NULL || ps == NULL)
        return;

    if (*list != NULL)
        (*list)->prev = ps;
    ps->next = *list;
    ps->prev = NULL;
    *list = ps;
}

void delete_primitive_system(PRIMITIVESYSTEM **list, PRIMITIVESYSTEM *to_delete)
{
    int i;

    if (to_delete == NULL)
        return;

    if (to_delete->prev != NULL)
        to_delete->prev->next = to_delete->next;
    else
        *list = to_delete->next;

    if (to_delete->next != NULL)
        to_delete->next->prev = to_delete->prev;

    for (i=0;i<to_delete->size;i++)
        free(to_delete->primitives[i].points);

    free(to_delete->primitives);
    free(to_delete);
}

void clear_primitive_systems(PRIMITIVESYSTEM **list)
{
    while(*list != NULL)
    {
        delete_primitive_system(list, *list);
    }
}

void move_primitive_systems(PRIMITIVESYSTEM **list)
{
    PRIMITIVESYSTEM *temp_ps, *ps = *list;
    int i;
    char primitive_system_valid = 0;

    while (ps != NULL)
    {
        primitive_system_valid = 0;
        for (i=0;i<ps->size;i++)
        {
            move_primitive(&ps->primitives[i]);
            if (ps->primitives[i].lifetime)
                primitive_system_valid = 1;
        }
        temp_ps = ps;
        ps = ps->next;
        if (!primitive_system_valid)
        {
            delete_primitive_system(list, temp_ps);
        }
    }
}

void move_primitive(PRIMITIVE *p)
{
    uint32_t current_ticks = SDL_GetTicks();

    if (p->lifetime && current_ticks - p->start_ticks <= (uint32_t)(p->lifetime * (1.0/_time_mult)))
    {
        switch (p->size)
        {
            case 1:
                move_point(&p->points[0], p->xVel, p->yVel);
                break;
            case 2:
                move_line(&p->points[0], &p->points[1], &p->centroid, p->xVel, p->yVel, p->rotation);
                break;
            default:
                move_polygon(p->points, p->size, &p->centroid, p->xVel, p->yVel, &p->angle, p->rotation, &p->bounding_box);
                break;
        }
    }
    else
    {
        p->lifetime = 0;
    }
}

void set_primitive_position(PRIMITIVE *p, float x, float y)
{
    int i;
    float offset_x, offset_y;

    if (p == NULL)
        return;

    offset_x = x - p->centroid.x;
    offset_y = y - p->centroid.y;
    p->centroid.x = x;
    p->centroid.y = y;
    for (i=0; i<p->size; i++)
    {
        p->points[i].x += offset_x;
        p->points[i].y += offset_y;
    }
}

void set_primitive_angle(PRIMITIVE *p, float angle)
{
    float offset_angle;
    int i;

    if (p == NULL)
        return;

    offset_angle = angle - p->angle;
    p->angle = angle;
    p->xVel = p->speed * cosf(to_radians(angle));
    p->yVel = p->speed * sinf(to_radians(angle));
    for (i=0; i<p->size; i++)
    {
        point2_rotate(&p->points[i], to_radians(offset_angle), p->centroid.x, p->centroid.y);
    }
}

void move_line(FPOINT *point1, FPOINT *point2, FPOINT *centroid, float xVel, float yVel, float rotation)
{
    FPOINT orig_centroid = *centroid;
    float offset_x, offset_y;

    if (point1 == NULL || point2 == NULL)
        return;

    centroid->x += xVel * _time_mult; centroid->y += yVel * _time_mult;
    wrap_axis(centroid);
    offset_x = centroid->x - orig_centroid.x; 
    offset_y = centroid->y - orig_centroid.y;

    point1->x += offset_x;   point1->y += offset_y;
    point2->x += offset_x;   point2->y += offset_y;

    point2_rotate(point1, to_radians(rotation), centroid->x, centroid->y);
    point2_rotate(point2, to_radians(rotation), centroid->x, centroid->y);
}

void move_point(FPOINT *point, float xVel, float yVel)
{
    if (point == NULL)
        return;

    point->x += xVel * _time_mult;
    point->y += yVel * _time_mult;
    wrap_axis(point);
}

void move_polygon(FPOINT *points, int size, FPOINT *centroid, float xVel, float yVel, float *angle, float rotation, FRECT *bounding_box)
{
    FPOINT orig_centroid = *centroid;
    float offset_x, offset_y;
    float min_x, max_x, min_y, max_y;
    int p;

    centroid->x += xVel * _time_mult;
    centroid->y += yVel * _time_mult;
    wrap_axis(centroid);
    offset_x = centroid->x - orig_centroid.x; 
    offset_y = centroid->y - orig_centroid.y;

    if (angle != NULL)
        *angle = wrapf(*angle + (rotation * _time_mult), FULL_DEG);

    if (points != NULL && size > 0)
    {
        max_x = min_x = centroid->x;
        max_y = min_y = centroid->y;

        for (p=0; p<size; p++)
        {
            points[p].x += offset_x;
            points[p].y += offset_y;
            point2_rotate(&points[p], to_radians(rotation * _time_mult), centroid->x, centroid->y);

            if (bounding_box != NULL)
            {
                if (points[p].x < min_x) 
                    min_x = points[p].x;
                else if (points[p].x > max_x) 
                    max_x = points[p].x;
                if (points[p].y < min_y) 
                    min_y = points[p].y;
                else if (points[p].y > max_y) 
                    max_y = points[p].y;
            }
        }
        if (bounding_box != NULL)
        {
            bounding_box->x = min_x;
            bounding_box->w = max_x - min_x;
            bounding_box->y = min_y;
            bounding_box->h = max_y - min_y;
        }
    }
}

void wrap_axis(FPOINT *point)
{
    if (point->x > _x_axis)
        point->x = -(2*_x_axis) + point->x;
    else if (point->x < -_x_axis)
        point->x = (2*_x_axis) + point->x; 
    
    if (point->y > _y_axis)
        point->y = -(2*_y_axis) + point->y;
    else if (point->y < -_y_axis)
        point->y = (2*_y_axis) + point->y; 
}

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines intersect the intersection point
// may be stored in the floats i_x and i_y.
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    float s, t;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0;
}

char boxes_colliding(struct Rect *a, struct Rect *b)
{
    if (a == NULL || b == NULL)
        return 0;

    if (a->y        > b->y + b->h) return 0;
    if (a->y + a->h < b->y)        return 0;
    if (a->x        > b->x + b->w) return 0;
    if (a->x + a->w < b->x)        return 0; 
    return 1;
}

char box_contains_point(struct Rect *box, float x, float y)
{
    if (box == NULL)
        return 0;

    return (x > box->x && x < (box->x + box->w) &&
            y > box->y && y < (box->y + box->h));
}

char polygon_contains_point(struct Point *polygon, int sz, float px, float py)
{
    int i, counter = 0;
    float xinters;
    struct Point p1, p2;

    p1 = polygon[0];
    for (i=1;i<=sz;i++)
    {
        p2 = polygon[i % sz];
        if (py > MIN(p1.y,p2.y))
            if (py <= MAX(p1.y, p2.y))
                if (px <= MAX(p1.x, p2.x))
                    if (p1.y != p2.y)
                    {
                        xinters = (py-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
                        if (p1.x == p2.x || px <= xinters)
                        {
                            counter++;
                        }
                    }
        p1 = p2;
    }

    return counter % 2;
}

char primitives_colliding(PRIMITIVE *pa, PRIMITIVE *pb)
{
    int i;

    if (pa == NULL || pb == NULL)
        return 0;

    if (!boxes_colliding(&pa->bounding_box, &pb->bounding_box))
        return 0;

    for (i=0; i<pb->size; i++)
    {
        if (polygon_contains_point(pa->points, pa->size, pb->points[i].x, pb->points[i].y))
            return 1;
    }

    for (i=0; i<pa->size; i++)
    {
        if (polygon_contains_point(pb->points, pb->size, pa->points[i].x, pa->points[i].y))
            return 1;
    }

    return 0;
}

char primitive_contains_point(PRIMITIVE *primitive, FPOINT *point)
{
    if (primitive == NULL || point == NULL)
        return 0;

    if (!box_contains_point(&primitive->bounding_box, point->x, point->y))
        return 0;

    if (polygon_contains_point(primitive->points, primitive->size, point->x, point->y))
        return 1;

    return 0;
}

void point2_rotate(struct Point *p, const float radians, const float pivotx, const float pivoty)
{
    float newx, newy;

    p->x -= pivotx;
    p->y -= pivoty;

    newx = p->x*cosf(radians) - p->y*sinf(radians);
    newy = p->x*sinf(radians) + p->y*cosf(radians);

    p->x = newx + pivotx;
    p->y = newy + pivoty;
}

float get_polygon_area(FPOINT *points, int sz)
{
    int i, j;
    float area = 0.0f;

    if (points == NULL || sz == 0)
        return 0.0f;

    for (i=0; i<sz; i++)
    {
        j = ((i+1) % sz);
        area += points[i].x * points[j].y;
        area -= points[i].y * points[j].x;
    }

    return fabsf(area) * 0.5f;
}

float to_radians(float degrees)
{
    return (PI*degrees)/180;
}

float to_degrees(float radians)
{
    return (radians * (180/PI));
}

