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


#ifndef  TEXT_INC
#define  TEXT_INC

#include	"SDL_opengl.h"
#include    "debug.h"

enum HTextAlign
{
    HALIGN_LEFT,
    HALIGN_CENTER,
    HALIGN_RIGHT,
    HALIGN_NONE
};

enum VTextAlign
{
    VALIGN_TOP,
    VALIGN_CENTER,
    VALIGN_BOTTOM,
    VALIGN_NONE
};

void set_text_dimensions(float height, float line_width);
void set_text_boundaries(float left, float top, float right, float bottom);
void set_horizontal_align(enum HTextAlign);
void set_vertical_align(enum VTextAlign);
void draw_text(const char *text);
void draw_text_at(const char *text, float x, float y);

#endif
