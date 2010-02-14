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


#include	"timing.h"
#include    "debug.h"

static uint32_t _start_ticks = 0;
static uint32_t _elapsed_ticks = 0;
static unsigned int _fps = DEFAULT_FPS;
static long frame = 0;
static float frame_rate = 0.0;

void start_timer()
{
    _start_ticks = SDL_GetTicks();
}

void set_fps(int fps)
{
    if (fps < 1)
        _fps = DEFAULT_FPS;
    else if (fps > MAX_FPS)
        _fps = MAX_FPS;
    else
        _fps = fps;
}

void end_frame()
{
    _elapsed_ticks = SDL_GetTicks() - _start_ticks;
    if (_elapsed_ticks < (1000 / _fps))
    {
        SDL_Delay((1000 / _fps) - _elapsed_ticks); 
    }
    _elapsed_ticks = SDL_GetTicks() - _start_ticks;
    frame++;
    frame_rate = 1000.0f / _elapsed_ticks;
}

float get_frame_rate()
{
    return frame_rate;
}

uint32_t get_frame_ticks()
{
    return _start_ticks;
}

uint32_t get_elapsed_ticks()
{
    return _elapsed_ticks;
}

void timetrace(char *str, ...)
{
#if TIMETRACE 
    long ticks = 0;
    ticks = SDL_GetTicks() - _start_ticks;

    va_list ap;
    va_start(ap, str);
    printf("Trace time: %d   ", ticks);
    vprintf(str, ap);
    putchar('\n');
    va_end(ap);
#endif
}
