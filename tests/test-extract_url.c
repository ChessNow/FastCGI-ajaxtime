/*
    FastCGI-ajaxtime test code to stress the extract_url functions.
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

struct harness {

  char *url;

  int expected_result;

  size_t expected_sn_len;

};

#include "extract_url.h"

int main(int argc, char *argv[]) {

  struct harness tests[] = {
    { .url = "ht//example.com/publish?channel=rusth932", .expected_result = 0 } // bad prefix
    , { .url = "http:///publish?channel=rusth932", .expected_result = 0 } // bad host
    , { .url = "http://localhost:/publish?channel=rusth932", .expected_result = 0 } // bad port
    , { .url = "http://localhost:ah2/publish?channel=rusth932", .expected_result = 0 } // bad port
    , { .url = "http://localhost:52/publish?channel=rusth932", .expected_result = 1, .expected_sn_len = 9 }
    , { .url = "http://localhost/publish?channel=rusth932", .expected_result = 1, .expected_sn_len = 9 }
  }, *s = tests, *e = s + sizeof(tests) / sizeof(struct harness);

  char *server_name;

  unsigned short port;

  int retval;

  for ( ; s < e; s++) {

    port = 0;

    retval = extract_url(&server_name, &port, s->url, YES_MALLOC);

    if (retval == -1) {

      if (s->expected_result == 0) continue;

      printf("%s: Expected test for %s to pass, but extract_url failed!\n", __FUNCTION__, s->url);

      printf("FAIL\n");
      
      return -1;      

    }

    else {

      if (s->expected_result) {
	
	if (s->expected_sn_len != strlen(server_name)) {

	  printf("%s: Almost had a success but the server_name length didn't match to the expected for %s from url=%s.\n", __FUNCTION__, server_name, s->url);

	  printf("FAIL\n");

	  return -1;

	}

	printf("%s: Success with server_name=%s port=%u\n", __FUNCTION__, server_name, port);

	continue;

      }

      printf("%s: Expected test for %s to fail, but extract_url passed!\n", __FUNCTION__, s->url);

      printf("FAIL\n");

      return -1;

    }

  }

  printf("OK\n");

  return 0;

}
