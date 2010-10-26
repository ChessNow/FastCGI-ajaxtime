#ifndef ID_CMD_PAYLOAD_H
#define ID_CMD_PAYLOAD_H

struct id_cmd_payload {

  char *id, *cmd, *payload;

};

int parse_icp(char *buffer, struct id_cmd_payload *x);

#endif
