#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <string.h>
#include <sys/time.h>

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

}

int second_compute_trim(time_t start, time_t current, struct time_pack *trim_amount) {

  assert(trim_amount!=NULL);

  trim_amount->min = current - start > 0 ? (current - start) / 60 : 0;

  trim_amount->sec = (current - start) - trim_amount->min * 60;

}

int reformulate_json(char *string, struct time_pack *w_time, struct time_pack *b_time, int move_number, char *move_string, int white_move) {

  assert(string!=NULL && move_string!=NULL);

  assert(w_time!=NULL && b_time!=NULL);

  sprintf(string, "{ \"w_time\": \"%d:%02d\", \"b_time\": \"%d:%02d\", \"last_move\": \"%d. %s%s\" }\n", 
	  w_time->min, w_time->sec, b_time->min, b_time->sec, move_number, white_move?"":"...", move_string);

  return 0;

}

int update_json(struct work_items *w, char *json, int json_len) {

  struct time_pack *cleave_pointer;

  struct time_pack trim_amount;

  struct timeval now;

  char string[80];

  int retval;

  int copy_len;
  
  assert(json!=NULL && json_len>0);

  gettimeofday(&now, NULL);

  second_compute_trim(w->tv_recent.tv_sec, now.tv_sec, &trim_amount);

  cleave_pointer = w->white_move ? &w->b_time : &w->w_time;

  //  trim_amount.min = 0;
  //  trim_amount.sec = w->tv_prior.tv_sec - w->tv_prior.tv_sec;
  //  trim_amount.sec = 5;

  apply_trim(&trim_amount, cleave_pointer);

  reformulate_json(string, &w->w_time, &w->b_time, w->move_number, w->move_string, w->white_move);

  copy_len = strlen(string);
  if (copy_len>json_len-1) copy_len=json_len-1;

  memcpy(json, string, copy_len);

  json[copy_len] = 0;

  return 0;

}

// example: WHITE_TIME=8 BLACK_TIME=8 WHITE_INCREMENT=10 BLACK_INCREMENT=10 spawn-fcgi -n -f ./fastcgi-ajaxtime -p 5700

int fill_from_environment(struct work_items *w) {

  char *starting_white_time = getenv("WHITE_TIME");

  char *starting_black_time = getenv("BLACK_TIME");

  char *white_increment_seconds = getenv("WHITE_INCREMENT");

  char *black_increment_seconds = getenv("BLACK_INCREMENT");

  int default_white_time_min = 5;
  int default_white_time_sec = 30;
  int default_black_time_min = 5;
  int default_black_time_sec = 30;

  int default_white_increment_seconds = 0;
  int default_black_increment_seconds = 0;

  char *sec_work;

  w->w_time.min = starting_white_time!=NULL ? strtol(starting_white_time, NULL, 10) : default_white_time_min;
  sec_work = starting_white_time!=NULL ? strchr(starting_white_time, ':') : NULL;
  w->w_time.sec = sec_work!=NULL ? strtol(sec_work+1, NULL, 10) : default_white_time_sec;

  w->b_time.min = starting_black_time!=NULL ? strtol(starting_black_time, NULL, 10) : default_black_time_min;
  sec_work = starting_black_time!=NULL ? strchr(starting_black_time, ':') : NULL;
  w->b_time.sec = sec_work!=NULL ? strtol(sec_work+1, NULL, 10) : default_black_time_sec;

  w->white_increment = white_increment_seconds != NULL ? strtol(white_increment_seconds, NULL, 10) : default_white_increment_seconds;
  w->black_increment = black_increment_seconds != NULL ? strtol(black_increment_seconds, NULL, 10) : default_black_increment_seconds;

  gettimeofday(&w->game_start, NULL);

  memcpy(&w->tv_prior, &w->game_start, sizeof(struct timeval));
  memcpy(&w->tv_recent, &w->game_start, sizeof(struct timeval));

}
