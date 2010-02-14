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


#ifndef  UTIL_INC
#define  UTIL_INC

#define MEMFREE(x)  if (x != NULL) { free(x); x = NULL; }
#define	MIN(x,y)    (x < y ? x : y)
#define MAX(x,y)    (x > y ? x : y)

#include    <math.h>
#include    <stdlib.h>

float square(float);
int rand_sign(void);
float randf(float limit);
float wrapf(float value, float limit);

#endif
