AM_CFLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual \
  -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs \
  -Wall -W -Wno-unused -Winline -Wwrite-strings -Wundef -pedantic

bin_PROGRAMS = crotalus
crotalus_SOURCES = alias.c boa.c buffer.c cgi.c cgi_header.c config.c escape.c \
	get.c ip.c log.c mmap_cache.c pipe.c queue.c range.c \
	read.c request.c response.c signals.c util.c sublog.c \
	select.c crotalus.c \
	cache/cr_hash.c cache/cr_hash.h cache/cr_pref.c cache/cr_pref.h
