/*
    FastCGI-ajaxtime support code for posting to nginx http_push_module
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
