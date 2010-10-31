/*
    FastCGI-ajaxtime test util to manually publish to nginx http_push_module
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

#include "ajaxtime_functions.h"

#include "http_push_support.h"

#include "extract_url.h"

int main(int argc, char *argv[]) {

  struct push_pack pc;

  int retval;

  char *publish_url = argc>1 ? argv[1] : "http://localhost/ajaxtime/publish";

  char *id_name = argc>2 ? argv[2] : "channel";

  char json_string[200];

  char *server_name;

  char port[6];

  unsigned short port_numeric;

  retval = prep_push(&pc, publish_url, id_name);

  if (retval==-1) {

    fprintf(stderr, "%s: Trouble with prep_push. What's up with that?\n", __FUNCTION__);
    
    return -1;

  }

  // extract server_name and port from publish_url

  retval = extract_url(&server_name, &port_numeric, publish_url, YES_MALLOC);

  if (retval==-1) {

    fprintf(stderr, "%s: Trouble extracting server_name and/or port with extract_url on url=%s.\n", __FUNCTION__, publish_url);

    return -1;

  }

  assert(server_name!=NULL);

  if (!port_numeric) port_numeric = 80;

  retval = sprintf(port, "%u", port_numeric);

  {

    struct time_pack w_time = { .min = 3, .sec = 20 }, b_time = { .min = 4, .sec = 47 };

    int move_number = 1;

    char *move_string = "e4";

    int white_move = 1;

    retval = reformulate_json(json_string, &w_time, &b_time, move_number, move_string, white_move);

    if (retval==-1) {
      printf("%s: Trouble preparing valid json_string.\n", __FUNCTION__);
      return -1;
    }

  }

  retval = push_submission(&pc, server_name, port, "7932221", 1, json_string);
  if (retval==-1) {

    printf("%s: Trouble with call to push_submission.\n", __FUNCTION__);
    return -1;

  }

  printf("%s: Sent %s\n", __FUNCTION__, json_string);

  return 0;

}
