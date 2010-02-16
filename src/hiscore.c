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

#include    "hiscore.h"

static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ";
static int current_pos = 0;     // the current position in which the user is entering a character. From 0 to 2.
static signed int current_selection = 0; // the current character selected
static char current_name[4] = {0};   // the current name selected by the user
static uint32_t scroll_start_ticks = 0;
static uint32_t last_char_flip = 0;
enum ScoreScrollDirection scroll_direction;

void init_hiscore()
{
    current_pos = 0;
    current_selection = 0;
    memset(current_name, ' ', sizeof(current_name));
    current_name[sizeof(current_name)-1] = '\0';
    scroll_start_ticks = 0;
}

void enter_hiscore_character()
{
    current_name[current_pos] = charset[current_selection];
    current_pos++;
}

const char is_hiscore_done()
{
    return (current_pos > 2);
}

const char *get_current_hiscore_name()
{
    current_name[current_pos] = charset[current_selection];
    return current_name;
}

void start_hiscore_scroll(enum ScoreScrollDirection direction)
{
    scroll_start_ticks = SDL_GetTicks();
    scroll_direction = direction;
}

void stop_hiscore_scroll()
{
    if (scroll_start_ticks)
        scroll_start_ticks = 0;
}

void manage_hiscore_state()
{
    uint32_t flip_speed;
    uint32_t current_ticks = SDL_GetTicks();
    signed int charset_size = sizeof(charset);

    if (scroll_start_ticks == 0)
        flip_speed = 0;
    else if (current_ticks - scroll_start_ticks > 1000)
        flip_speed = 80;
    else
        flip_speed = 160;

    if (flip_speed > 0 && current_ticks - last_char_flip >= flip_speed)
    {
        current_selection += (scroll_direction == SCROLL_LEFT ? -1 : 1);
        if (current_selection >= charset_size - 1)
        {
            current_selection = 0;
        }
        else if (current_selection < 0)
        {
            current_selection = charset_size - 2;
        }
        last_char_flip = current_ticks;
    }
}

void read_high_scores(struct HighScore *hiscores)
{
    FILE *fp = NULL;
    char line[15];
    int i = 0;

    fp = fopen(HISCORES_FILE,"r");
    if (fp == NULL)
    {
        trace("Could not open hiscores file for reading!");
        exit(1);
    }

    while (fgets(line, 15, fp) != NULL && i<MAX_HISCORES)
    {
        if (line != NULL)
        {
            sscanf(line, "%3[^,]%7d", &hiscores[i].initials, &hiscores[i].score);
        }
        i++;
    }

    fclose(fp);
}

void write_high_scores(const struct HighScore *hiscores)
{
    FILE *fp = NULL;
    char line[15] = {0};
    int i;
    
    fp = fopen(HISCORES_FILE, "w");
    if (fp == NULL)
    {
        trace("Could not open hiscores file for writing!");
        exit(1);
    }

    for (i=0; i<MAX_HISCORES; i++)
    {
        sprintf(line, "%-3s%07d\n", hiscores[i].initials, hiscores[i].score);
        fputs(line, fp);
    }

    fclose(fp);
}

void set_high_score(struct HighScore *hiscores, const char *initials, const long int score)
{
    int i, j;

    for(i=0; i<MAX_HISCORES; i++)
    {
        if (score >= hiscores[i].score) // score is higher than this one, insert here
        {
            // move all the scores at this point and below down a rank
            for (j=MAX_HISCORES; j>i; j--)
            {
                strncpy(hiscores[j].initials, hiscores[j-1].initials, 3);
                hiscores[j].score = hiscores[j-1].score;
            }
            strncpy(hiscores[i].initials, initials, 3);
            hiscores[i].score = score;
            break;
        }
    }
}
