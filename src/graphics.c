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


#include	"graphics.h"

int init_gl(int hRes, int vRes, float x_axis, float y_axis)
{
    GLenum response;

    // Setup rendering state
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    trace("resizing window. hRes: %d vRes: %d", hRes, vRes);
    resize_window((GLsizei)hRes, (GLsizei)vRes, x_axis, y_axis);

    response = glGetError();
    if (response != GL_NO_ERROR)
    {
        trace("OpenGL error detected: %d", glGetError());
        return 0;
    }

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glColor3f(1.0f, 1.0f, 1.0f);

    return 1;
}

void resize_window(GLsizei w, GLsizei h, float x_axis, float y_axis)
{
    GLfloat aspect_ratio;

    if (h==0)
        h = 1;

    // Set viewport to dimensions
    glViewport(0, 0, w, h);

    // Reset coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    aspect_ratio = (GLfloat)w / (GLfloat)h;
    if (w <= h)
        gluOrtho2D(-x_axis, y_axis, -x_axis / aspect_ratio, y_axis / aspect_ratio);
    else
        gluOrtho2D(-x_axis * aspect_ratio, y_axis * aspect_ratio, -x_axis, y_axis);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void to_glcolor(uint32_t color, struct OpenGLColor *glcolor)
{
    if (glcolor == NULL)
        return;
    glcolor->r = ((color & 0xFF0000) >> 16) / 255.0f;
    glcolor->g = ((color & 0xFF00) >> 8) / 255.0f;
    glcolor->b = (color & 0xFF) / 255.0f;
}
