/*
    FastCGI-ajaxtime extract a url string into server and port components
    Copyright (C) 2010  Lester Vecsey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    For feedback on this code email chessnow@fractal-interval.com
    or write to Chess Now, 140 West 69th St, Suite 126c, New York, NY 10023

*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "extract_url.h"

int extract_url(char **server_name, unsigned short *port, char *url, int state) {

  char *sname = NULL, *por = NULL;

  char *p = url, *end;

  long int ploc;

  assert(server_name != NULL || port!=NULL);

  assert(url!=NULL);
  
  if (strncmp(p, "http://", 7)) {
    return -1;
  }

  p+=7;

  for ( end = p; *end && *end != '/'; ) {
    if (*end == ':') por = end;
    end++;
  }

  if (state&YES_MALLOC) {

    int len;

    len = ((por!=NULL) ? por : end) - p;

    if (len<=0) return -1;

    sname = malloc(len+1);
    if (sname==NULL) {
      return -1;
    }

    memcpy(sname, p, len);
    sname[len] = 0;
    
  }

  else sname = p;

  if (server_name!=NULL) *server_name = sname;

  if (por!=NULL) {

    por++;

    ploc = strtol(por, NULL, 10);
    if (ploc == LONG_MIN || ploc == LONG_MAX || ploc < 0 || ploc > 65535) {
      ploc = 0;
    }
    
    else if (port!=NULL) *port = ploc;

  }

  if (por!=NULL && !ploc) {

    if (sname!=NULL && (state&YES_MALLOC)) {
      free(sname);
    }

    return -1;

  }
  
  return 0;

}

