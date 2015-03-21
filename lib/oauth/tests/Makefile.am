check_PROGRAMS = tceran tcother tcwiki
ACLOCAL_AMFLAGS= -I m4

OAUTHDIR =..
INCLUDES = -I$(srcdir)/$(OAUTHDIR)
MYCFLAGS =  
MYLDADD  = $(OAUTHDIR)/libshare_oauth.a
LIBS     = -lm -lcrypto

TESTS = tceran tcother tcwiki 

if HAVE_CURL
MYCFLAGS += -lcurl
TESTS += oauthtest
check_PROGRAMS += oauthtest oauthtest2
endif

tcwiki_SOURCES = selftest_wiki.c commontest.c commontest.h
tcwiki_LDADD = $(MYLDADD)
#tcwiki_CFLAGS = $(MYCFLAGS)

tceran_SOURCES = selftest_eran.c commontest.c commontest.h
tceran_LDADD = $(MYLDADD)
#tceran_CFLAGS = $(MYCFLAGS)

tcother_SOURCES = selftest_other.c commontest.c commontest.h
tcother_LDADD = $(MYLDADD)
#tcother_CFLAGS = $(MYCFLAGS)

oauthtest_SOURCES = oauthtest.c
oauthtest_LDADD = $(MYLDADD)
oauthtest_CFLAGS = $(MYCFLAGS)

oauthtest2_SOURCES = oauthtest2.c
oauthtest2_LDADD = $(MYLDADD)
oauthtest2_CFLAGS = $(MYCFLAGS)
