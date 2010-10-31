/*
    FastCGI-ajaxtime support code for parsing a POST identifier payload cmd
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

#include <string.h>

#include "id_cmd_payload.h"

int cap_formline(char *s, char **endptr) {

  assert(s!=NULL);

  while (*s && *s != '"' && *s != '&') s++;
  
  if (*s == '"' || *s == '&') *s = 0;

  if (endptr!=NULL) {
    *endptr = s;
  }

  return 0;

}

int parse_icp(char *buffer, struct id_cmd_payload *x) {

  char *endptr;

  char *match_pointer;

  assert(buffer!=NULL && x!=NULL);

  match_pointer = strstr(buffer, "id=");
  if (match_pointer==NULL) return -1;
  x->id = match_pointer + 3;

  cap_formline(x->id, &endptr);

  assert(*endptr == 0);

  match_pointer = strstr(endptr+1, "cmd=");
  if (match_pointer==NULL) return -1;
  x->cmd = match_pointer + 4;

  cap_formline(x->cmd, &endptr);

  assert(*endptr == 0);

  match_pointer = strstr(endptr+1, "payload=");
  if (match_pointer==NULL) return -1;
  x->payload = match_pointer + 8;

  cap_formline(x->payload, NULL);

  return 0;

}
