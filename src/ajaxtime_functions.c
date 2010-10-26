#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <string.h>
#include <sys/time.h>

#include "time_pack.h"

#include "work_items.h"

#include "ajaxtime_functions.h"

// http://www.gnu.org/s/libc/manual/html_node/Elapsed-Time.html

     int
     timeval_subtract (result, x, y)
          struct timeval *result, *x, *y;
     {
       /* Perform the carry for the later subtraction by updating y. */
       if (x->tv_usec < y->tv_usec) {
         int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
         y->tv_usec -= 1000000 * nsec;
         y->tv_sec += nsec;
       }
       if (x->tv_usec - y->tv_usec > 1000000) {
         int nsec = (x->tv_usec - y->tv_usec) / 1000000;
         y->tv_usec += 1000000 * nsec;
         y->tv_sec -= nsec;
       }
     
       /* Compute the time remaining to wait.
          tv_usec is certainly positive. */
       result->tv_sec = x->tv_sec - y->tv_sec;
       result->tv_usec = x->tv_usec - y->tv_usec;
     
       /* Return 1 if result is negative. */
       return x->tv_sec < y->tv_sec;
     }

int reformulate_json(char *string, struct time_pack *w_time, struct time_pack *b_time, int move_number, char *move_string, int white_move) {

  assert(string!=NULL && move_string!=NULL);

  assert(w_time!=NULL && b_time!=NULL);

  sprintf(string, "{ \"w_time\": \"%d:%02d\", \"b_time\": \"%d:%02d\", \"last_move\": \"%d. %s%s\" }\n", 
	  w_time->min, w_time->sec, b_time->min, b_time->sec, move_number, white_move?"":"...", move_string);

  return 0;

}

int compute_trim(struct time_pack *d, struct timeval *current, struct timeval *previous) {

  struct timeval difference;

  struct timeval destructive_pryor;

  int retval;

  assert(d!=NULL);

  assert(current!=NULL && previous!=NULL);

  memcpy(&destructive_pryor, previous, sizeof(struct timeval));
 
  retval = timeval_subtract(&difference, current, &destructive_pryor);
  if (retval!=1) {

    d->min = difference.tv_sec > 0 ? difference.tv_sec / 60 : 0;
    d->sec = difference.tv_sec > (d->min*60) ? difference.tv_sec - d->min*60 : 0;

    return 0;

  }

  // negative time? maybe the local clock was adjusted.

  d->min = 0;
  d->sec = 0;

  return -1;

}

int update_json(struct work_items *w, char *json, int json_len, int *game_over) {

  struct timeval client_now;

  char string[80];

  int black_retval, white_retval;

  int copy_len;

  struct time_pack local_white, local_black;
  
  assert(json!=NULL && json_len>0);

  gettimeofday(&client_now, NULL);

  if (!w->move_number) {
    memcpy(&local_white, &w->timeset.w_time, sizeof(struct time_pack));
    memcpy(&local_black, &w->timeset.b_time, sizeof(struct time_pack));
  }

  if (w->move_number >= 1) {

    white_retval = compute_trim(&local_white, &w->expected_white_end, w->white_move ? &w->w_laststamp : &client_now);

    black_retval = compute_trim(&local_black, &w->expected_black_end, !w->white_move ? &w->b_laststamp : &client_now);

    if (white_retval==-1 || black_retval==-1) {

      // game over condition

      if (game_over!=NULL) {
	*game_over = !white_retval;
      }

    }

  }

  reformulate_json(string, &local_white, &local_black, w->move_number, w->move_string, w->white_move);

  copy_len = strlen(string);
  if (copy_len>json_len-1) copy_len=json_len-1;

  memcpy(json, string, copy_len);

  json[copy_len] = 0;

  return 0;

}

// example: WHITE_TIME=8 BLACK_TIME=8 WHITE_INCREMENT=10 BLACK_INCREMENT=10 spawn-fcgi -n -p 5700 ./fastcgi-ajaxtime -- zmq_address zmq_port

int fill_from_environment(struct fixed_time *f) {

  char *starting_white_time = getenv("WHITE_TIME");

  char *starting_black_time = getenv("BLACK_TIME");

  char *white_increment_seconds = getenv("WHITE_INCREMENT");

  char *black_increment_seconds = getenv("BLACK_INCREMENT");

  int default_white_increment_seconds = 0;

  int default_black_increment_seconds = 0;

  int default_white_time_min = 5;
  int default_white_time_sec = 30;
  int default_black_time_min = 5;
  int default_black_time_sec = 30;

  char *sec_work;

  assert(f!=NULL);

  f->w_time.min = starting_white_time!=NULL ? strtol(starting_white_time, NULL, 10) : default_white_time_min;
  if (starting_white_time!=NULL) {
    sec_work = strchr(starting_white_time, ':');
    f->w_time.sec = sec_work!=NULL ? strtol(sec_work+1, NULL, 10) : default_white_time_sec;
  }

  f->b_time.min = starting_black_time!=NULL ? strtol(starting_black_time, NULL, 10) : default_black_time_min;
  if (starting_black_time!=NULL) {
    sec_work = strchr(starting_black_time, ':');
    f->b_time.sec = sec_work!=NULL ? strtol(sec_work+1, NULL, 10) : default_black_time_sec;
  }

  f->white_increment = white_increment_seconds != NULL ? strtol(white_increment_seconds, NULL, 10) : default_white_increment_seconds;
  f->black_increment = black_increment_seconds != NULL ? strtol(black_increment_seconds, NULL, 10) : default_black_increment_seconds;

  return 0;

}

int apply_increment(struct work_items *w, int white_move) {

  struct fixed_time *f;

  assert(w!=NULL);

  f = &w->timeset;

  assert(white_move==0 || white_move==1);

  switch(white_move) {

  case 0:
    if (f->black_increment>0) {
      w->expected_black_end.tv_sec += f->black_increment;
    }
    break;

  case 1: 
    if (f->white_increment>0) {
      w->expected_white_end.tv_sec += f->white_increment;
    }
    break;

  }

  return 0;

}

int fill_idle_lastmove(struct timeval *idle, struct work_items *w) {

  struct timeval pryor;

  int retval;

  memcpy(&pryor, w->tv_prior == NULL ? &w->game_start : w->tv_prior, sizeof(struct timeval));

  retval = timeval_subtract(idle, w->tv_recent, &pryor);
  if (retval==1) {
    printf("%s: Expected an idle difference between a recent and prior (or game start) time, but got nothing.\n", __FUNCTION__);
  }

  return 0;

}

// called only after the first move has been played to account for time played.

int expected_end_boostdiff(struct work_items *w, int white_move) {

  struct timeval idle;

  int retval;

  assert(w!=NULL);

  assert(w->tv_recent != NULL);

  retval = fill_idle_lastmove(&idle, w);
  if (retval==-1) {
    printf("%s: Trouble getting elapsed time of last move.\n", __FUNCTION__);
    idle.tv_sec = 0;
    idle.tv_usec = 0;
  }

  // extend the current side based on how long the opposite play (last move) took.

  {
    
    struct timeval *operation = white_move ?  &w->expected_black_end : &w->expected_white_end;

    operation->tv_sec += idle.tv_sec;
    operation->tv_usec += idle.tv_usec;

    while (operation->tv_usec > 1000000) {
      operation->tv_usec -= 1000000;
      operation->tv_sec += 1;
    }
    
  }

  return 0;

}
