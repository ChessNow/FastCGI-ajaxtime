#ifndef AJAXTIME_FUNCTIONS_H
#define AJAXTIME_FUNCTIONS_H

#include "work_items.h"

#include "time_pack.h"

int fill_from_environment(struct fixed_time *f);

int reformulate_json(char *string, struct time_pack *w_time, struct time_pack *b_time, int move_number, char *move_string, int white_move);

int update_json(struct work_items *w, char *json, int json_len, int *game_over);

int apply_increment(struct work_items *w, int white_move);
int expected_end_boostdiff(struct work_items *w, int white_move);

#endif
