
INCLUDES = 

LIBS = 

AM_CFLAGS = -Wall

bin_PROGRAMS = fastcgi-ajaxtime push_submission

fastcgi_ajaxtime_LDFLAGS = 
fastcgi_ajaxtime_LDADD = -lfcgi $(ZMQ_LIBS)
fastcgi_ajaxtime_CFLAGS = $(AM_CFLAGS) $(ZMQ_CFLAGS)
fastcgi_ajaxtime_SOURCES = fastcgi-ajaxtime.c zhelpers.h time_pack.h work_items.h work_items.c ajaxtime_functions.c ajaxtime_functions.h id_cmd_payload.c id_cmd_payload.h http_push_support.c http_push_support.h

push_submission_LDFLAGS =
push_submission_LDADD =
push_submission_CFLAGS =$(AM_CFLAGS)
push_submission_SOURCES = http_push_support.c http_push_support.h push_submission.c ajaxtime_functions.h ajaxtime_functions.c extract_url.h extract_url.c
