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

#ifndef WORK_ITEMS_H
#define WORK_ITEMS_H

#include "time_pack.h"

#include <sys/time.h>

#include <pthread.h>

struct fixed_time {

  int white_increment, black_increment;

  struct time_pack w_time, b_time;

};

#define INACTIVE_GAME 0

struct work_items {

  struct fixed_time timeset;

  struct timeval game_start, initial_black;

  struct timeval w_laststamp, b_laststamp;

  struct timeval *tv_prior, *tv_recent;

  struct timeval expected_white_end, expected_black_end;

  int white_move;
  unsigned int move_number;
  char move_string[10];

  int state;

  pthread_t subscription_thread_reader;

  pthread_mutex_t threadstate_lock;

};

int init_work(struct work_items *i, struct timeval *game_start);

#endif
