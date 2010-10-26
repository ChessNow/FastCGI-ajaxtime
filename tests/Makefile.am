
INCLUDES = 

LIBS = 

AM_CFLAGS =

bin_PROGRAMS = zmsg_test test-id_cmd_payload test-fill_from_environment

TESTS = $(bin_PROGRAMS)

zmsg_test_LDFLAGS = 
zmsg_test_LDADD = $(ZMQ_LIBS)
zmsg_test_CFLAGS = $(ZMQ_CFLAGS) -I$(top_srcdir)/src
zmsg_test_SOURCES = zmsg_test.c $(top_srcdir)/src/zhelpers.h

test_id_cmd_payload_LDFLAGS =
test_id_cmd_payload_LDADD =
test_id_cmd_payload_CFLAGS = -I$(top_srcdir)/src
test_id_cmd_payload_SOURCES = test-id_cmd_payload.c $(top_srcdir)/src/id_cmd_payload.h $(top_srcdir)/src/id_cmd_payload.c

test_fill_from_environment_LDFLAGS = 
test_fill_from_environment_LDADD = 
test_fill_from_environment_CFLAGS = -I$(top_srcdir)/src
test_fill_from_environment_SOURCES = test-fill_from_environment.c $(top_srcdir)/src/ajaxtime_functions.h $(top_srcdir)/src/ajaxtime_functions.c
