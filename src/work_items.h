#ifndef WORK_ITEMS_H
#define WORK_ITEMS_H

#include "time_pack.h"

#include <sys/time.h>

#include <pthread.h>

struct fixed_time {

  int white_increment, black_increment;

  struct time_pack w_time, b_time;

};

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
