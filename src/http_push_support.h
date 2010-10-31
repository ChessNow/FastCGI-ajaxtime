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

#ifndef HTTP_PUSH_SUPPORT_H
#define HTTP_PUSH_SUPPORT_H

struct push_pack {

  char *url;

  char *id_name;

};

int push_submission(struct push_pack *p, char *server_name, char *port, char *identifier, int move_number, char *json_string);

int prep_push(struct push_pack *p, char *publish_url, char *id_name);

#endif
