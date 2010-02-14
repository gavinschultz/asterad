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


#include	"sound.h"

static const char **filemap;    // array of strings containing the base filenames, each one corresponding to a sound type
static int mapsize = 0;         // number of sound types that we have mapped
static int *filecounts;         // array of ints, where each item corresponds to the number of sounds loaded for a particular sound type
static Mix_Chunk ***sounds;     // uh-oh, this is probably foolish. An array of arrays to Mix_Chunk pointers.
static Mix_Music *music;
static int sound_on = 1;

void init_sound()
{
    Mix_Chunk *chunk = NULL;
    if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 ) {
        trace("Could not initialise sound mixer: %s", SDL_GetError());
        exit(1);
    } 
}

void load_sounds(const char *src_dir, const char **map, const unsigned int mapsz)
{
    int i, j;
    DIR *dir;
    struct dirent *dir_entity;
    char basename[MAX_PATH] = {0};

    if (map == NULL || mapsz == 0)
        return;

    filemap = map;
    mapsize = mapsz;
    filecounts = calloc(mapsize, sizeof(*filecounts));
    sounds = malloc(mapsize * sizeof(*sounds));

    dir = opendir(src_dir);
    if (dir == NULL)
    {
        trace("Could not load sounds from directory %s!", src_dir);
        exit(0);
    }

    while((dir_entity = readdir(dir)) != NULL)
    {
        sscanf(dir_entity->d_name, "%[^.0-9]%*d.%s", basename);
        for (i=0; i<mapsize; i++)
            if (strcmp(basename, filemap[i]) == 0)
                filecounts[i]++;
    }

    for (i=0; i<mapsize; i++)
    {
        sounds[i] = malloc(filecounts[i] * sizeof(**sounds));
        for (j=0; j<filecounts[i]; j++)
        {
            char filename[MAX_PATH];

            // Must load in specific format "basenameNN.wav"
            sprintf(filename, "%s\\%s%02d.wav", src_dir, filemap[i], j+1);
            sounds[i][j] = Mix_LoadWAV(filename);
            if (sounds[i][j] == NULL)
            {
                char *msg = Mix_GetError();
                trace("Failed to load wav file %s! (%s)", filename, msg);
            }
            else
            {
                trace("Loaded @ [%d][%d] wav file '%s'", i, j, filename);
            }
        }
    }

    closedir(dir);
}

void play_music(char *file, int loops)
{
    music = Mix_LoadMUS(file);
    if (music == NULL)
    {
        trace("Error loading background music: %s", Mix_GetError());
        return;
    }
    
    Mix_PlayMusic(music, LOOP_FOREVER);
}

int play_chunk(Mix_Chunk *chunk, int loops)
{
    if (!sound_on)
        return INVALID_CHANNEL;

    return Mix_PlayChannel(-1, chunk, loops);
}

int play_sound(int mapped_type)
{
    int listsize;
    Mix_Chunk *chunk;
    if (!sound_on)
        return INVALID_CHANNEL;

    if (mapped_type >= mapsize)
        return INVALID_CHANNEL;

    listsize = filecounts[mapped_type];
    if (listsize <= 0)
        return INVALID_CHANNEL;

    chunk = sounds[mapped_type][(rand() % listsize)];
    if (chunk == NULL)
        return INVALID_CHANNEL;

    return play_chunk(chunk, 0);
}

void stop_channel(int *channel)
{
    if (*channel == INVALID_CHANNEL)
        return;

    Mix_HaltChannel(*channel);
    *channel = INVALID_CHANNEL;
}

void toggle_sound()
{
    sound_on = !sound_on;
    if (sound_on)
        Mix_VolumeMusic(MIX_MAX_VOLUME);
    else
        Mix_VolumeMusic(0);
}

void shutdown_sound()
{
    int i, j;

    for (i=0; i<mapsize; i++)
    {
        for (j=0; j<filecounts[i]; j++)
        {
            Mix_FreeChunk(sounds[i][j]);
        }
        free(sounds[i]);
    }

    Mix_CloseAudio();
}
