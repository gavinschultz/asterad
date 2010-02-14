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

#include    "util.h"

float square(float n)
{
    return n*n;
}

int rand_sign()
{
    return (rand() % 2 ? -1 : 1);
}

float randf(float limit)
{
    return (float)rand()/(float)RAND_MAX * limit;
}

float wrapf(float value, float limit)
{
    if (value >= limit)
        return value - limit;
    else if (value < 0.0f)
        return value + limit;
    else
        return value;
}
