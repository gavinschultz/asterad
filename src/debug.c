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


#include    <stdarg.h>
#include    <stdio.h>
#include    "debug.h"

void trace(char *str, ...)
{
#if TRACE
    va_list ap;
    va_start(ap, str);
    vprintf(str, ap);
    putchar('\n');
    va_end(ap);
    fflush(stdout);
#endif
}
