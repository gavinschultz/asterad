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


#include    "SDL.h"

#define TIMETRACE       0       /* Enable / disable time tracing */
#define DEFAULT_FPS     60      /* default frames per second */
#define MAX_FPS         200     /* maximum frames per second */

void set_fps(int);
void start_timer(void);
void end_frame(void);
void timetrace(char *,...);
float get_frame_rate(void);
uint32_t get_frame_ticks(void);     // Gets the raw number of ticks at the time this frame started
uint32_t get_elapsed_ticks(void);   // Gets the number of ticks elapsed since last frame
