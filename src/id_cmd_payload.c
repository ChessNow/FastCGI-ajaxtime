#include <stdio.h>
#include <assert.h>

#include <string.h>

#include "id_cmd_payload.h"

int cap_formline(char *s, char **endptr) {

  assert(s!=NULL);

  while (*s && *s != '"' && *s != '&') s++;
  
  if (*s == '"' || *s == '&') *s = 0;

  if (endptr!=NULL) {
    *endptr = s;
  }

  return 0;

}

int parse_icp(char *buffer, struct id_cmd_payload *x) {

  char *endptr;

  char *match_pointer;

  assert(buffer!=NULL && x!=NULL);

  match_pointer = strstr(buffer, "id=");
  if (match_pointer==NULL) return -1;
  x->id = match_pointer + 3;

  cap_formline(x->id, &endptr);

  assert(*endptr == 0);

  match_pointer = strstr(endptr+1, "cmd=");
  if (match_pointer==NULL) return -1;
  x->cmd = match_pointer + 4;

  cap_formline(x->cmd, &endptr);

  assert(*endptr == 0);

  match_pointer = strstr(endptr+1, "payload=");
  if (match_pointer==NULL) return -1;
  x->payload = match_pointer + 8;

  cap_formline(x->payload, NULL);

  return 0;

}
