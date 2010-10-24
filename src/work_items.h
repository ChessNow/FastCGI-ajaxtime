#ifndef WORK_ITEMS_H
#define WORK_ITEMS_H

#include "time_pack.h"

#include <sys/time.h>

#include <pthread.h>

struct work_items {

  char *json_file;
  int fd;

  struct time_pack w_time, b_time;

  int white_increment, black_increment;

  struct timeval game_start;

  struct timeval tv_prior, tv_recent;

  int white_move;
  unsigned int move_number;
  char move_string[10];

  int state;

  pthread_t subscription_thread_reader;

  pthread_mutex_t threadstate_lock;

};

extern struct work_items w;

#endif
