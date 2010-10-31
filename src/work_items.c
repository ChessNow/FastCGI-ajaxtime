/*
    FastCGI-ajaxtime for displaying a chess clock and moves
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

#include <sys/time.h>

#include "work_items.h"

void reset_dynamic_times(struct work_items *i, struct timeval *game_start) {

  assert(i!=NULL && game_start!=NULL);

  memcpy(&i->game_start, game_start, sizeof(struct timeval));
  memset(&i->initial_black, 0, sizeof(struct timeval));

  memcpy(&i->w_laststamp, &i->game_start, sizeof(struct timeval));
  memset(&i->b_laststamp, 0, sizeof(struct timeval));

  i->tv_prior = &i->w_laststamp;
  i->tv_recent = &i->b_laststamp;

  memcpy(&i->expected_white_end, &i->game_start, sizeof(struct timeval));
  memcpy(&i->expected_black_end, &i->game_start, sizeof(struct timeval));

}

int init_work(struct work_items *i, struct timeval *game_start) {

  assert(i!=NULL && game_start!=NULL);
  
  reset_dynamic_times(i, game_start);

  i->white_move = 1;
  i->move_number = INACTIVE_GAME;

  return 0;

}

