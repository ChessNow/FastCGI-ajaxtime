#include <stdio.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <errno.h>

#include "fcgi_config.h"

#include <stdlib.h>
#include <values.h>

extern char **environ;

#include "fcgi_stdio.h"

#include <zmq.h>

#include "zhelpers.h"

#include "id_cmd_payload.h"

#include "zmsg.c"

#include <pthread.h>

#define THREAD_STARTED 0x1
#define THREAD_ACTIVE 0x2
#define SUBSCRIPTION_RECEIVE 0x4

#include "work_items.h"

#include "ajaxtime_functions.h"

struct work_items w = { .state = 0, .move_number = 1, .white_move = 1 };

struct thread_main_share {

  char json[80];

};

struct thread_main_share tms;

unsigned int fclose_stderr_failures;
unsigned int fclose_stdout_failures;

int show_status() {

    printf("Content-type: text/html\r\n\r\n");

    printf("Sorry, GET requests are ignored.\n");

    printf("<p>%s %s %s</p>\n"
	   , w.state&THREAD_STARTED?"THREAD_STARTED":""
	   , w.state&THREAD_ACTIVE?"THREAD_ACTIVE":""
	   , w.state&SUBSCRIPTION_RECEIVE?"SUBSCRIPTION_RECEIVE":""
	   );

    printf("<p>move_number=%d white_move=%d</p>\n", w.move_number, w.white_move);

    printf("<p>move_string=%s</p>\n", w.move_string);

    printf("<p>json=%s</p>\n", tms.json);

    printf("<p>fclose_stderr_failures=%lu</p>\n", fclose_stderr_failures);

    printf("<p>fclose_stdout_failures=%lu</p>\n", fclose_stdout_failures);

}

void *context;

void *sub_socket;

// expecting moves of the form: 1. e4
// 1... e5

void *subscription_receiver(void *argument) {

  void *result = NULL;

  int debug = 0;

  long int conversion;
  
  printf("%s: Starting ZeroMQ subscriber thread to extract published moves.\n", __FUNCTION__);

  pthread_mutex_lock(&w.threadstate_lock);
  w.state |= THREAD_ACTIVE;
  pthread_mutex_unlock(&w.threadstate_lock);

  for ( ;; ) {

    char *message;

    pthread_mutex_lock(&w.threadstate_lock);
    w.state &= ~(w.state&SUBSCRIPTION_RECEIVE);
    pthread_mutex_unlock(&w.threadstate_lock);
  
    message = s_recv(sub_socket);

    //    message = "4... Bxc3";

    if (message!=NULL) {

      pthread_mutex_lock(&w.threadstate_lock);

      w.state |= SUBSCRIPTION_RECEIVE;

      conversion = strtol(message, NULL, 10);
      if (conversion >= 1 && conversion < LONG_MAX && errno!=ERANGE) {

	char *p;

	w.move_number = conversion;

	w.white_move = (NULL==strstr(message, "..."));

	p = message;
	
	for ( p = message; *p && *p != ' '; p++);

	if (*p == ' ') {

	  int max_len;

	  p++;

	  max_len = strlen(p);
	    
	  if (max_len+1 > sizeof(w.move_string)) {
	    max_len = sizeof(w.move_string) - 1;
	  }

	  memcpy(w.move_string, p, max_len);
	  w.move_string[max_len] = 0;

	}
	
      }

      pthread_mutex_unlock(&w.threadstate_lock);

    }

  }

}

int main(int argc, char *argv[]) {

  char buffer[4096];

  int read_retval;

  int identifier;

  char hostname[64];

  int retval;

  unsigned char zmq_identifier[17];

  int log_enabled = getenv("FASTCGI_LOGGING") != NULL;

  int log_fd = log_enabled ? open("fastcgi-json_relay.log", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR) : -1;

  char *server_name = argc>1 ? argv[1] : "*";

  char *port_string = argc>2 ? argv[2] : "4921";

  int port;

  int rc;

  int show_json(char *response) {

    assert(response!=NULL);

    printf("%s\n", response);

    return 0;

  }

  int error_out(char *err, int retval) {

    assert(err!=NULL);

    printf("{ \"%s\": %d, \"identifier\": -1 }\n", err, retval);

    if (log_fd!=-1) {
      char string[80];
      sprintf(string, "%s: Trouble with err=%s retval=%d\n", __FUNCTION__, err, retval);
      retval = write(log_fd, string, strlen(string));
    }

    return 0;

  }

  retval = gethostname(hostname, sizeof(hostname) - 1);

  if (retval==-1) {
    perror("gethostname");
    fprintf(stderr, "%s: Trouble with call to gethostname.\n", __FUNCTION__);
    return -1;
  }

  retval = fill_from_environment(&w);

  if (retval==-1) {
    fprintf(stderr, "%s: Trouble with ajaxtime fill_from_environment function.\n", __FUNCTION__);
    return -1;

  }

  if (log_fd!=-1) {
    char *s = "Starting up.\n";
    retval = write(log_fd, s, strlen(s));
  }

  context = zmq_init(1);
  if (context==NULL) {
    fprintf(stderr, "%s: Trouble with call to zmq_init.\n", __FUNCTION__);
    return -1;
  }

  sub_socket = zmq_socket(context, ZMQ_SUB);
  if (sub_socket==NULL) {
    fprintf(stderr, "%s: Trouble with call to zmq_socket.\n", __FUNCTION__);
    return -1;
  }

  port = strtol(port_string, NULL, 10);
  if (port<1 || port>=LONG_MAX || errno==ERANGE) {
    printf("%s: Invalid port, out of range.\n", __FUNCTION__);
    return -1;
  }

  retval = zmq_setsockopt (sub_socket, ZMQ_SUBSCRIBE, "", 0);
  if (retval!=0) {
    fprintf(stderr, "%s: Trouble with zmq_setsockopt for full on subscription.\n", __FUNCTION__);
    return -1;
  }

  {

    char transport_string[80];

    sprintf(transport_string, "tcp://%s:%d", server_name, port);

    retval = zmq_connect(sub_socket, transport_string);
    if (retval!=0) {
      fprintf(stderr, "%s: Trouble with call to zmq_connect. Is the zmq_forwarder up?\n", __FUNCTION__);
      return -1;
    }

  }

  pthread_mutex_init(&w.threadstate_lock, NULL);

  rc = pthread_create(&w.subscription_thread_reader, NULL, &subscription_receiver, NULL);

  if (rc != 0) {
    printf("%s: Trouble creating subscription_thread_reader.\n", __FUNCTION__);
  }

  pthread_mutex_lock(&w.threadstate_lock);
  w.state |= THREAD_STARTED;
  pthread_mutex_unlock(&w.threadstate_lock);

  while (FCGI_Accept() >= 0) {

    char *contentLength = getenv("CONTENT_LENGTH");

    char *requestMethod = getenv("REQUEST_METHOD");

    unsigned int len;

    if (requestMethod != NULL && !strncmp(requestMethod, "GET", 3)) {

      show_status();

      continue;

    }

    printf("Content-type: application/javascript\r\n\r\n");

    if (contentLength != NULL) {

      len = strtol(contentLength, NULL, 10);

      if (log_fd!=-1) {
	char string[80];
	sprintf(string, "%s: len=%u and contentLength=%s\n", __FUNCTION__, len, contentLength);
	retval = write(log_fd, string, strlen(string));
      }

      if (!len) {

	identifier = 5932321;

	error_out("strtol", len);

	continue;

      }

      else

      if (len>0) {

	struct id_cmd_payload x;

	// buffer looks like id=98723432&cmd=POLL&payload=empty

	read_retval = fread(buffer, 1, sizeof(buffer), stdin);

	if (log_fd!=-1) {
	  char string[40];
	  sprintf(string, "%s: fread=%d\n", __FUNCTION__, read_retval);
	  retval = write(log_fd, string, strlen(string));
	}

	if (read_retval == len) {

	  if (read_retval==sizeof(buffer)) continue;

	  buffer[read_retval] = 0;

	  retval = parse_icp(buffer, &x);

	  if (log_fd!=-1) {
	    char string[80];
	    sprintf(string, "%s: parse_icp=%d for buffer=%s\n", __FUNCTION__, retval, buffer);
	    retval = write(log_fd, string, strlen(string));
	  }

	  if (retval==-1) {

	    // problem, malformed json submission. who are they?

	    error_out("parse", retval);

	    continue;

	  }

	  assert(x.id != NULL && x.cmd != NULL && x.payload !=NULL);

	  {

	    // look up response maintained by the subscription thread.

	    int thread_busy = 1;

	    while (thread_busy) {

	      pthread_mutex_lock(&w.threadstate_lock);
	      if (! (w.state & SUBSCRIPTION_RECEIVE)) {
		thread_busy = 0;
	      }
	      pthread_mutex_unlock(&w.threadstate_lock);

	    }

	    pthread_mutex_lock(&w.threadstate_lock);

	    update_json(&w, tms.json, sizeof(tms.json));

	    printf("%s\n", tms.json);

	    pthread_mutex_unlock(&w.threadstate_lock);

	  }

	}

      }

    }

      else {

	// problem.

	error_out("fread", read_retval);

	continue;

      }

    retval = fclose(stderr);
    if (retval!=0) {
      fclose_stderr_failures++;
      if (log_fd!=-1) {
	char *s = "The client probably disappeared, fclose(stderr) failure.\n";
	retval = write(log_fd, s, strlen(s));
      }
    }

    retval = fclose(stdout);
    if (retval!=0) {
      fclose_stdout_failures++;
      if (log_fd!=-1) {
	char *s = "The client probably disappeared, fclose(stdout) failure.\n";
	retval = write(log_fd, s, strlen(s));
      }
    }

  }

  if (log_fd!=-1) {
    char *s = "Calling zmq_term.\n";
    retval = write(log_fd, s, strlen(s));
  }

  zmq_term(context);

  if (log_fd!=-1) {
    char *s = "Closing log file.\n";
    retval = write(log_fd, s, strlen(s));
    close(log_fd);
  }

  return 0;

}
