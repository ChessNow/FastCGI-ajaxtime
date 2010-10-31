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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <errno.h>

#include "http_push_support.h"

// by default we submit to http://localhost/ajaxtime/publish?channel=oyuahrsors

int push_submission(struct push_pack *p, char *server_name, char *port, char *identifier, int move_number, char *json_string) {

  int retval;

  struct addrinfo hints, *result, *rp;

  struct sockaddr my_addr;

  char http_header_post_line[50];
  char http_header_host_line[50];
  char http_header_content_length[30];

  char response_buffer[2048];

  int json_string_len;
  
  int len;

  int s = -1;

  assert(p!=NULL && identifier!=NULL);

  assert(json_string!=NULL);

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */
	   
  retval = getaddrinfo(server_name, port, &hints, &result);
  if (retval != 0) {
    fprintf(stderr, "%s: Trouble with call to getaddrinfo on %s.\n", __FUNCTION__, server_name);
    fprintf(stderr, "%s: getaddrinfo returned %s\n", __FUNCTION__, gai_strerror(retval));
    return -1;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {

    // usually AF_INET, SOCK_STREAM, 0
    s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (s==-1) {
      perror("socket");
      fprintf(stderr, "%s: Trouble opening socket.\n", __FUNCTION__);
      continue;
    }

    // bind is optional, but might leave it bound until there is a failure.

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sa_family = AF_INET;

    retval = bind(s, &my_addr, (socklen_t) sizeof(struct sockaddr_in));
    if (retval==-1) {
      perror("bind");
      fprintf(stderr, "%s: Trouble with bind call on socket s=%d\n", __FUNCTION__, s);
    }

    retval = connect(s, rp->ai_addr, rp->ai_addrlen);

    if (!retval) {
      printf("%s: Connect returned OK.\n", __FUNCTION__);
      break;
    }

    else

    if (retval==-1) {
      perror("connect");
      printf("%s: Trouble with connect on socket s=%d\n", __FUNCTION__, s);
    }

    printf("%s: Closing socket s=%d\n", __FUNCTION__, s);

    close(s);

  }

  printf("%s: Preparing HTTP headers.\n", __FUNCTION__);

  len = sprintf(http_header_post_line, "POST /ajaxtime/publish?%s=%s HTTP/1.0\r\n", p->id_name, identifier);
    retval = write(s, http_header_post_line, len);
  if (retval!=len) {
    fprintf(stderr, "%s: Trouble with write for POST line. Expected %d but got %d.\n", __FUNCTION__, len, retval);
    return -1;
  }

  printf("%s: Posting to server_name=%s with %s", __FUNCTION__, server_name, http_header_post_line);

  len = sprintf(http_header_host_line, "Host: %s\r\n", server_name);
  retval = write(s, http_header_host_line, len);
  if (retval!=len) {
    fprintf(stderr, "%s: Trouble with write for Host line. Expected %d but got %d.\n", __FUNCTION__, len, retval);
    return -1;
  }

  json_string_len = strlen(json_string);

  len = sprintf(http_header_content_length, "Content-Length: %d\r\n\r\n", json_string_len);
  retval = write(s, http_header_content_length, len);
  if (retval!=len) {
    fprintf(stderr, "%s: Trouble with write for content_length line. Expected %d but got %d.\n", __FUNCTION__, json_string_len, retval);
    return -1;
  }

  retval = write(s, json_string, json_string_len);
  if (retval!=json_string_len) {
    fprintf(stderr, "%s: Trouble with write. Expected %d but got %d.\n", __FUNCTION__, json_string_len, retval);
    return -1;
  }

  // read result

  retval = read(s, response_buffer, sizeof(response_buffer));
  printf("%s: read returned %d.\n", __FUNCTION__, retval);

  if (retval>0) {

    int write_retval;

    write_retval = write(1, response_buffer, retval);

  }

  retval = close(s);
  if (retval==-1) {
    perror("close");
    fprintf(stderr, "%s: Trouble closing socket.\n", __FUNCTION__);
    return -1;    
  }

  return 0;

}

int prep_push(struct push_pack *p, char *publish_url, char *id_name) {

  assert(p!=NULL && publish_url!=NULL && id_name!=NULL);

  p->url = publish_url;
  
  p->id_name = id_name;

  return 0;

}
