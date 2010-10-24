#include <stdio.h>

#include <assert.h>

#include "id_cmd_payload.h"

struct test_buffers {

  char *buffer;

  int expected_result;

};

int main(int argc, char *argv[]) {

  struct id_cmd_payload icp;

  struct test_buffers bufset[] = {
    { .buffer = "ihuyr=239847234&cmd=MOVE&payload=e2e4", .expected_result = 0 } // no id
    ,{ .buffer = "id=239847234&chrb=MOVE&pahuissnr", .expected_result = 0 } // no cmd
    ,{ .buffer = "id=239847234&cmd=MOVE&pahuissnr", .expected_result = 0 } // no payload
    ,{ .buffer = "id=239847234&cmd=MOVE&payload=e2e4", .expected_result = 1 } // OK
  }, *bs = bufset, *be = bs + sizeof(bufset) / sizeof(struct test_buffers);

  char buffer_rewrite_space[128];

  int retval;

  printf("%s: Running POST style format tests.\n", __FUNCTION__);

  for ( ; bs < be; bs++) {

    int buffer_len = strlen(bs->buffer);

    if (!buffer_len) continue;

    assert(buffer_len + 1 < sizeof(buffer_rewrite_space));

    memcpy(buffer_rewrite_space, bs->buffer, buffer_len);
    buffer_rewrite_space[buffer_len] = 0;

    if (bs->expected_result) {
      printf("%s: Valid string expected to pass: %s\n", __FUNCTION__, bs->buffer);
    }

    retval = parse_icp(buffer_rewrite_space, &icp);

    if (retval==-1 && bs->expected_result==1) {

      printf("%s: buffer=%s\n", __FUNCTION__, bs->buffer);
      printf("%s: Expected parse_icp to pass but it failed!\n", __FUNCTION__);

      return -1;

    }

    else {

      if (retval==0 && bs->expected_result==0) {

	printf("%s: buffer=%s\n", __FUNCTION__, bs->buffer);
	printf("%s: Expected parse_icp to fail but it passed!\n", __FUNCTION__);

	return -1;

      }

    }

  }

  if (bs == be) {
    printf("%s\n", "OK");
    return 0;
  }

  printf("%s\n", "FAIL");
  return -1;

}
