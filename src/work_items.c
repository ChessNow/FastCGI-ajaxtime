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
  i->move_number = 1;

  return 0;

}

