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

int apply_trim(struct time_pack *trim_amount, struct time_pack *cleave_pointer) {

  assert(cleave_pointer!=NULL);

  assert(trim_amount!=NULL);

  cleave_pointer->min -= trim_amount->min;
  cleave_pointer->sec -= trim_amount->sec;

  if (cleave_pointer->sec < 0) {
    cleave_pointer->sec += 60;
    cleave_pointer->min -= 1;
  }

  if (cleave_pointer->min < 0 || cleave_pointer->sec < 0) {
    cleave_pointer->min = 0;
    cleave_pointer->sec = 0;
  }

  return 0;

}

int second_compute_trim(time_t start, time_t current, struct time_pack *trim_amount) {

  assert(trim_amount!=NULL);

  trim_amount->min = current - start > 0 ? (current - start) / 60 : 0;

  trim_amount->sec = (current - start) - trim_amount->min * 60;

  return 0;

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

int update_json(struct work_items *w, char *json, int json_len) {

  struct time_pack *cleave_pointer;

  struct time_pack trim_recent, trim_bulk_white, trim_bulk_black;

  struct timeval client_now;

  struct timeval black_extent_time, white_extent_time;

  char string[80];

  int retval;

  int copy_len;

  struct time_pack local_white, local_black;
  
  struct timeval destructive_pryor;

  assert(json!=NULL && json_len>0);

  gettimeofday(&client_now, NULL);

  cleave_pointer = w->white_move ? &local_black : &local_white;

  
  memcpy(&destructive_pryor, &w->game_start, sizeof(struct timeval));
  timeval_subtract(&white_extent_time, w->white_move ? &client_now : &w->w_laststamp, &destructive_pryor);

  memcpy(&destructive_pryor, &w->initial_black, sizeof(struct timeval));
  timeval_subtract(&black_extent_time, !w->white_move ? &client_now : &w->b_laststamp, &destructive_pryor);

  memcpy(&local_white, &w->timeset.w_time, sizeof(struct time_pack));
  memcpy(&local_black, &w->timeset.b_time, sizeof(struct time_pack));

  if (w->tv_recent == NULL) {
    goto setup_json;
  }

  retval = compute_trim(&trim_recent, &client_now, w->tv_recent);
  if (retval==-1) {
    printf("%s: Warning, compute_trim failed for some reason from tv_recent to client_now.\n", __FUNCTION__);
  }

  apply_trim(&trim_recent, w->white_move ? &local_black : &local_white);

  if (!w->white_move || w->move_number > 1) {

    retval = compute_trim(&trim_bulk_black, &w->b_laststamp, &w->artificial_black);
    if (retval==-1) {
      printf("%s: Warning, compute_trim failed for some reason from artificial_black to b_laststamp.\n", __FUNCTION__);
    }

  }

  else {
    trim_bulk_black.min = 0;
    trim_bulk_black.sec = 0;
  }

  retval = compute_trim(&trim_bulk_white, &w->w_laststamp, &w->artificial_start);
  if (retval==-1) {
    printf("%s: Warning, compute_trim failed for some reason from initial_black to tv_recent or tv_prior.\n", __FUNCTION__);
  }

  apply_trim(&trim_bulk_black, &local_black);
  apply_trim(&trim_bulk_white, &local_white);

 setup_json:

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

void add_to_time_pack(struct time_pack *p, struct time_pack *addition) {

  assert(p!=NULL && addition!=NULL);

  p->min += addition->min;
  p->sec += addition->sec;

  while (p->sec >= 60) {

    p->min++;
    p->sec -= 60;

  }

}

void set_time_pack(struct time_pack *d, int seconds) {

  assert(d!=NULL);

  d->min = seconds > 0 ? (seconds / 60) : 0;

  d->sec = seconds - d->min * 60;

}

int advance_time_pack(struct time_pack *p, int seconds) {

  struct time_pack local;

  assert(p!=NULL);

  set_time_pack(&local, seconds);

  add_to_time_pack(p, &local);

  return 0;

}

int apply_increments(struct work_items *w, struct timeval *now, int white_move) {

  struct fixed_time *f;

  assert(w!=NULL);

  f = &w->timeset;

  if (white_move && f->white_increment>0) {
    w->artificial_start.tv_sec -= f->white_increment;
  }

  else 

  if (!white_move && f->black_increment>0) {
    w->artificial_black.tv_sec -= f->black_increment;
  }

  return 0;

}

// called only after the first move has been played to account for time played.

int dampen_artificials(struct work_items *w, int white_move) {

  struct timeval pryor;

  struct timeval chop;

  int retval;

  assert(w!=NULL);

  assert(w->tv_recent != NULL);

  memcpy(&pryor, w->tv_prior == NULL ? &w->game_start : w->tv_prior, sizeof(struct timeval));

  retval = timeval_subtract(&chop, w->tv_recent, &pryor);
  if (retval==-1) {
    printf("%s: Expected a chop difference between a recent and prior (or game start) time, but got nothing.\n", __FUNCTION__);
  }

  // whether the artificial reference point had an increment applied to it or not, chop away
  // the elapsed time

  {
    
    struct timeval *operation = white_move ? &w->artificial_black : &w->artificial_start;

    operation->tv_sec += chop.tv_sec;
    operation->tv_usec += chop.tv_usec;

    while (operation->tv_usec >= 1000000) {
      operation->tv_usec -= 1000000;
      operation->tv_sec += 1;
    }
    
  }

  return 0;

}
