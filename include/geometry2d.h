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

#ifndef  GEOMETRY2D_INC
#define  GEOMETRY2D_INC

#include    <stdlib.h>
#include	"SDL.h"
#include    "util.h"
#include    "debug.h"

#define	PI          3.1415f		/* Simple definition of PI */
#define FULL_DEG    360.0f

typedef struct Point
{
    float x, y;
} FPOINT;

typedef struct Rect
{
    float x, y, w, h;
} FRECT;

typedef struct Primitive
{
    uint32_t start_ticks;   // the number of ticks at which this primitive was created
    int32_t lifetime;   // lifetime in milliseconds before the primitive expires; -1 = infinite
    FPOINT *points;      // The points comprising the primitive
    int size;           // The number of points in the primitive
    FPOINT centroid;     // Central point of the primitive; used as the axis around which rotation occurs
    float xVel, yVel;   // The X and Y velocity of the primitive
    float angle;        // The angle at which the primitive is rotated; generally only relevant for polygons of 3 sides or more
    float rotation;     // The rate of rotation
    float speed;        // The speed of the primitive. Can be derived from the angle, xVel and yVel, but often handy to store.
    float area;
    int invisible;
    FRECT bounding_box;
} PRIMITIVE;

// A group of primitives. This structure is convenient as a logical grouping but also allows
// memory for the primitives to be initialised and freed all at once.
// Also a linked list to other primitive systems, allowing for further grouping of entire systems
typedef struct PrimitiveSystem
{
    PRIMITIVE *primitives;
    int size;
    enum PolygonMode mode;
    struct PrimitiveSystem *next;
    struct PrimitiveSystem *prev;
} PRIMITIVESYSTEM;

enum RelativePrimitivePosition
{
    FROM_CENTER,
    FROM_AREA
};

void set_axes(float x, float y);
void set_time_mult(float t);

// Primitive system movement / management
PRIMITIVESYSTEM *create_primitive_system(int primitive_count, int primitive_size);
void add_primitive_system(PRIMITIVESYSTEM **list, PRIMITIVESYSTEM *ps);
void delete_primitive_system(PRIMITIVESYSTEM **list, PRIMITIVESYSTEM *to_delete);
void move_primitive_systems(PRIMITIVESYSTEM **list);
void clear_primitive_systems(PRIMITIVESYSTEM **list);
PRIMITIVE *create_primitives(int num_of_primitives, int primitive_size);
void set_primitive_position(PRIMITIVE *, float x, float y);
void set_primitive_angle(PRIMITIVE *, float angle);
void move_primitive(PRIMITIVE *);
//PRIMITIVESYSTEM *break_primitive(PRIMITIVE *, PRIMITIVESYSTEM **);  // Breaks apart a primitive into lines
//void delete_primitives(PRIMITIVE *, int size);

// For moving objects in 2D space
void move_polygon(FPOINT *points, int size, FPOINT *centroid, float xVel, float yVel, float *angle, float rotation, FRECT *bounding_box);
void move_line(FPOINT *point1, FPOINT *point2, FPOINT *centroid, float xVel, float yVel, float rotation);
void move_point(FPOINT *point, float xVel, float yVel);
void wrap_axis(FPOINT *);

// General geometric formulas
float to_radians(float);
float to_degrees(float);
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);
void point2_rotate(FPOINT *, const float, const float, const float);
char boxes_colliding(struct Rect *, struct Rect *);
char box_contains_point(struct Rect *, float, float);
char polygon_contains_point(FPOINT *polygon, int sz, float px, float py);
char primitives_colliding(PRIMITIVE *, PRIMITIVE *);
char primitive_contains_point(PRIMITIVE *, FPOINT *);
float get_polygon_area(FPOINT *polygon, int sz);

#endif
