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

#ifndef  GRAPHICS_INC
#define  GRAPHICS_INC

#include    "SDL_opengl.h"
#include    "debug.h"

struct OpenGLColor
{
    float r;
    float g;
    float b;
};

int init_gl(int hRes, int vRes, float x_axis, float y_axis);
void resize_window(GLsizei w, GLsizei h, float x_axis, float y_axis);
void to_glcolor(uint32_t color, struct OpenGLColor *glcolor);

#endif
