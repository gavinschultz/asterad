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


#include    "text.h"
#include    <stdlib.h>
#include    <string.h>

#define SCALE_Y _text_height / 3.0f
#define SCALE_X _text_height / 3.0f

static float _text_height = 9.0f;    // Unit height of a single character
static float _text_width = 6.0f;     // Unit width of a single character
static float _line_width = 1.0f;
static float _char_spacing = 1.5f;
static enum HTextAlign _halign;
static enum VTextAlign _valign;
static float bound_x1;
static float bound_y1;
static float bound_x2;
static float bound_y2;
static float bound_w;
static float bound_h;

// Arrays of characters. All lines will be draw with GL_LINESTRIP, but a value of -1 indicate that the pen should "lift off" the page and start again at the next point.
static const float ca[] = { 0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 3.0f, 2.0f, 2.0f, 2.0f, 0.0f, -1.0f, 0.0f, 1.0f, 2.0f, 1.0f };
static const float cb[] = { 1.0f, 1.5f, 2.0f, 0.75f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 1.0f, 3.0f, 2.0f, 2.25f, 1.0f, 1.5f, 0.0f, 1.5f };
static const float cc[] = { 2.0f, 2.75f, 1.0f, 3.0f, 0.0f, 2.0f, 0.0f, 1.0f, 1.0f, 0.0f, 2.0f, 0.25f };
static const float cd[] = { 0.0f, 0.0f, 0.0f, 3.0f, 1.0f, 3.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f };
static const float ce[] = { 2.0f, 3.0f, 0.0f, 3.0f, 0.0f, 0.0f, 2.0f, 0.0f, -1.0f, 0.0f, 1.5f, 1.0f, 1.5f };
static const float cf[] = { 2.0f, 3.0f, 0.0f, 3.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.5f, 1.0f, 1.5f };
static const float cg[] = { 2.0f, 2.75f, 1.0f, 3.0f, 0.0f, 2.0f, 0.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 1.0f, 1.0f };
static const float ch[] = { 0.0f, 0.0f, 0.0f, 3.0f, -1.0f, 0.0f, 1.5f, 2.0f, 1.5f, -1.0f, 2.0f, 3.0f, 2.0f, 0.0f };
static const float ci[] = { 0.0f, 0.0f, 2.0f, 0.0f, -1.0f, 0.0f, 3.0f, 2.0f, 3.0f, -1.0f, 1.0f, 0.0f, 1.0f, 3.0f };
static const float cj[] = { 1.0f, 3.0f, 2.0f, 3.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
static const float ck[] = { 0.0f, 0.0f, 0.0f, 3.0f, -1.0f, 2.0f, 3.0f, 0.0f, 1.5f, 2.0f, 0.0f };
static const float cl[] = { 0.0f, 3.0f, 0.0f, 0.0f, 2.0f, 0.0f };
static const float cm[] = { 0.0f, 0.0f, 0.0f, 2.0f, 0.5f, 3.0f, 1.0f, 1.0f, 1.5f, 3.0f, 2.0f, 2.0f, 2.0f, 0.0f };
static const float cn[] = { 0.0f, 0.0f, 0.0f, 3.0f, 2.0f, 0.0f, 2.0f, 3.0f };
static const float co[] = { 0.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
static const float cp[] = { 0.0f, 0.0f, 0.0f, 3.0f, 1.0f, 3.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 1.0f };
static const float cq[] = { 0.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, 2.0f, 0.0f };
static const float cr[] = { 0.0f, 0.0f, 0.0f, 3.0f, 1.0f, 3.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, 2.0f, 0.0f };
static const float cs[] = { 2.0f, 3.0f, 1.0f, 3.0f, 0.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f };
static const float ct[] = { 0.0f, 3.0f, 2.0f, 3.0f, -1.0f, 1.0f, 3.0f, 1.0f, 0.0f };
static const float cu[] = { 0.0f, 3.0f, 0.0f, 0.5f, 1.0f, 0.0f, 2.0f, 0.5f, 2.0f, 3.0f };
static const float cv[] = { 0.0f, 3.0f, 0.0f, 2.0f, 1.0f, 0.0f, 2.0f, 2.0f, 2.0f, 3.0f };
static const float cw[] = { 0.0f, 3.0f, 0.0f, 1.0f, 0.5f, 0.0f, 1.0f, 2.0f, 1.5f, 0.0f, 2.0f, 1.0f, 2.0f, 3.0f };
static const float cx[] = { 0.0f, 3.0f, 2.0f, 0.0f, -1.0f, 0.0f, 0.0f, 2.0f, 3.0f };
static const float cy[] = { 0.0f, 3.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 2.0f, 3.0f };
static const float cz[] = { 0.0f, 3.0f, 2.0f, 3.0f, 0.0f, 0.0f, 2.0f, 0.0f };
static const float c0[] = { 0.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 2.0f, 2.0f };
static const float c1[] = { 1.0f, 3.0f, 1.0f, 0.0f };
static const float c2[] = { 0.0f, 2.0f, 1.0f, 3.0f, 2.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f };
static const float c3[] = { 0.0f, 3.0f, 2.0f, 3.0f, 2.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.5f, 2.0f, 1.5f };
static const float c4[] = { 0.0f, 3.0f, 0.0f, 1.0f, 2.0f, 1.0f, 2.0f, 3.0f, 2.0f, 0.0f };
static const float c5[] = { 2.0f, 3.0f, 0.0f, 3.0f, 0.0f, 1.5f, 2.0f, 1.5f, 2.0f, 0.0f, 0.0f, 0.0f };
static const float c6[] = { 0.0f, 3.0f, 0.0f, 0.0f, 2.0f, 0.0f, 2.0f, 1.5f, 0.0f, 1.5f };
static const float c7[] = { 0.0f, 3.0f, 2.0f, 3.0f, 2.0f, 0.0f };
static const float c8[] = { 0.0f, 0.0f, 0.0f, 3.0f, 2.0f, 3.0f, 2.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.5f, 2.0f, 1.5f };
static const float c9[] = { 2.0f, 0.0f, 2.0f, 3.0f, 0.0f, 3.0f, 0.0f, 1.5f, 2.0f, 1.5f };
static const float c_period[] = { 0.75f, 0.0f, 0.75f, 0.5f, 1.25f, 0.5f, 1.25f, 0.0f, 0.75f, 0.0f };
static const float *charset[] = { ca, cb, cc, cd, ce, cf, cg, ch, ci, cj, ck, cl, cm, cn, co, cp, cq, cr, cs, ct, cu, cv, cw, cx, cy, cz }; 
static const int   charset_sz[] = { sizeof ca, sizeof cb, sizeof cc, sizeof cd, sizeof ce, sizeof cf, sizeof cg, sizeof ch, sizeof ci, sizeof cj, sizeof ck,
    sizeof cl, sizeof cm, sizeof cn, sizeof co, sizeof cp, sizeof cq, sizeof cr, sizeof cs, sizeof ct, sizeof cu, sizeof cv, sizeof cw, sizeof cx, sizeof cy,
    sizeof cz};
static const float *numset[] = { c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 };
static const int   numset_sz[] = { sizeof c0, sizeof c1, sizeof c2, sizeof c3, sizeof c4, sizeof c5, sizeof c6, sizeof c7, sizeof c8, sizeof c9 };
static const int   charset_size = sizeof(charset) / sizeof(*charset);

// natural ratio of width:height is 2:3
void set_text_dimensions(float height, float line_width)
{
    _text_height = height;
    _text_width = height * (2.0f/3.0f);
    _line_width = line_width;
    _char_spacing = _text_width / 4.0f;
}

// Draw text based on the boundary and alignment settings
void draw_text(const char *text)
{
    float width;
    int str_length;
    float x = bound_x1;
    float y = bound_y2;

    // determine total width of text
    str_length = strlen(text);
    width = (str_length * _text_width) + ((str_length - 1) * _char_spacing);
//    trace("Estimated width of text is %3.1f for \"%s\"", width, text);

    switch (_halign)
    {
        case HALIGN_CENTER:
            x = bound_x1 + ((bound_w - width) / 2.0f);
            break;
        case HALIGN_RIGHT:
            x = bound_x2 - width;
            break;
    }

    switch (_valign)
    {
        case VALIGN_CENTER:
            y = bound_y2 + ((bound_h + _text_height) / 2.0f);
            break;
        case VALIGN_TOP:
            y = bound_y1 - _text_height;
            break;
    }

    draw_text_at(text, x, y);
}

void draw_text_at(const char *text, float x, float y)
{
    char c;
    int index, size;
    int i;
    const float *character = NULL;

    glLineWidth(_line_width);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
//    trace("charset size: %d, charset* size: %d", sizeof(charset), sizeof(*charset));
//    trace("Drawing text \"%s\"", text);
    while ((c = *text++) != '\0')
    {
//        trace("Drawing '%c'", c);

        if (c >= 'A' && c <= 'Z')
        {
            index = c - 'A';
            character = charset[index];
            size = charset_sz[index] / sizeof(float);
        }
        else if (c >= '0' && c <= '9')
        {
            index = c - '0';
            character = numset[index];
            size = numset_sz[index] / sizeof(float);
        }
        else if (c == '.')
        {
            character = c_period;
            size = sizeof(c_period) / sizeof(float);
        }
        else if (c == ' ')
        {
            // do nothing...
        }
        else if (c == '\n')
        {
            glTranslatef(0, -(_text_height + (_text_height / 4.0f)), 0.0f);
        }
        else
        {
            trace("character '%c' not found in character set!", c);
            continue;   // Still not found, move on
        }

        if (character != NULL)
        {
            glBegin(GL_LINE_STRIP);
//            trace("character '%c' found: %p, size: %d", c, charset[index], size);
            for (i=0; i<size-1; i=i+2)
            {
                if (character[i] < 0)
                {
                    //                    trace("(%d) pen lifting off", i);
                    i--;
                    glEnd();
                    glBegin(GL_LINE_STRIP);
                }
                else
                {
//                    trace("(%d) drawing from (%1.1f, %1.1f)", i, character[i] * SCALE_X, character[i+1] * SCALE_Y);
                    glVertex2f(character[i] * SCALE_X, character[i+1] * SCALE_Y);
                }
            }
            character = NULL;
            glEnd();
        }
        //            trace("\t%f", (_text_width + (_text_width / 4.0f)));
        glTranslatef((_text_width + (_text_width / 4.0f)), 0.0f, 0.0f);
    }
    glEnd();
    glPopMatrix();
    glLineWidth(0.5f);
}

void set_text_boundaries(float left, float top, float right, float bottom)
{
    bound_x1 = left;
    bound_y1 = top;
    bound_x2 = right;
    bound_y2 = bottom;
    bound_w = right - left;
    bound_h = top - bottom;
}

void set_horizontal_align(enum HTextAlign align)
{
    _halign = align;
}

void set_vertical_align(enum VTextAlign align)
{
    _valign = align;
}
