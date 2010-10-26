#ifndef AJAXTIME_FUNCTIONS_H
#define AJAXTIME_FUNCTIONS_H

#include "work_items.h"

#include "time_pack.h"

int fill_from_environment(struct fixed_time *f);

int apply_trim(struct time_pack *trim_amount, struct time_pack *cleave_pointer);
int second_compute_trim(time_t start, time_t current, struct time_pack *trim_amount);
int reformulate_json(char *string, struct time_pack *w_time, struct time_pack *b_time, int move_number, char *move_string, int white_move);

int update_json(struct work_items *w, char *json, int json_len);

int apply_increment(struct work_items *w, int white_move);
int expected_end_boostdiff(struct work_items *w, int white_move);
int expected_end_retractdiff(struct work_items *w, int white_move);

void set_time_pack(struct time_pack *d, int seconds);
void add_to_time_pack(struct time_pack *p, struct time_pack *addition);
int advance_time_pack(struct time_pack *p, int seconds);

#endif
