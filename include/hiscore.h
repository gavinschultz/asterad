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


#ifndef  HISCORE_INC
#define  HISCORE_INC

#include	"SDL.h"
#include    "debug.h"
#include	<stdio.h>

#define HISCORES_FILE   "resources\\hiscore"
#define MAX_HISCORES    9

enum ScoreScrollDirection
{
    SCROLL_LEFT,
    SCROLL_RIGHT
};

struct HighScore
{
    char initials[3];
    long int score;
};
    
void init_hiscore(void);
void enter_hiscore_character(void);
const char *get_current_hiscore_name(void);
void start_hiscore_scroll(enum ScoreScrollDirection);
void stop_hiscore_scroll(void);
void manage_hiscore_state(void);
const char is_hiscore_done(void);
void set_high_score(struct HighScore *, const char *initials, const long int score);
void read_high_scores(struct HighScore *);
void write_high_scores(const struct HighScore *);

#endif   /* ----- #ifndef HISCORE_INC  ----- */
