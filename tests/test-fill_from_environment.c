#include <stdio.h>

#include <assert.h>
#include <stdlib.h>

#include "ajaxtime_functions.h"

int main(int argc, char *argv[]) {

  struct fixed_time f;

  int retval;

  int success() {

    printf("OK\n");

    exit(0);

  }

  retval = putenv("WHITE_TIME=25:50");
  if (retval) return -1;

  retval = putenv("BLACK_TIME=24:20");
  if (retval) return -1;  

  retval = putenv("WHITE_INCREMENT=12");
  if (retval) return -1;

  retval = putenv("BLACK_INCREMENT=15");
  if (retval) return -1;

  printf("%s: Calling fill_from_environment.\n", __FUNCTION__);

  retval = fill_from_environment(&f);
  if (retval==-1) {
    printf("%s: Failure on fill_from_environment.\n", __FUNCTION__);
    return -1;
  }

  if (f.w_time.min != 25 || f.w_time.sec != 50) {
    printf("%s: Failure on setting white time.\n", __FUNCTION__);
    goto fail_out;
  }

  if (f.b_time.min != 24 || f.b_time.sec != 20) {
    printf("%s: Failure on setting black time.\n", __FUNCTION__);
    goto fail_out;
  }

  if (f.white_increment != 12 || f.black_increment != 15) {
  
    printf("%s: Failure on setting increments.\n", __FUNCTION__);
    goto fail_out;

  }

  success();

 fail_out:
  
  printf("FAIL\n");

  return -1;

}
