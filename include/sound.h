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

#ifndef  SOUND_INC
#define  SOUND_INC

#define INVALID_CHANNEL -99
#define LOOP_FOREVER -1
#define ALL_CHANNELS -1

#include    <string.h>
#include    "SDL.h"
#include    "SDL_mixer.h"
#include    "debug.h"
#include    "dirent.h"

void init_sound(void);
void load_sounds(const char *src_dir, const char **map, const unsigned int mapsz);
int play_sound(int mapped_type);
void play_music(char *file, int loops);
int play_chunk(Mix_Chunk *, int loops);
void stop_channel(int *channel);
void toggle_sound(void);
void shutdown_sound(void);

#endif   /* ----- #ifndef SOUND_INC  ----- */
