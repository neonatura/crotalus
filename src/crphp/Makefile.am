
if USE_PHP_RUNTIME
bin_PROGRAMS = crphp

#crphp_SOURCES = crphp_main.c
crphp_SOURCES = php_cli.c ps_title.c php_cli_process_title.c php_cli_server.c php_http_parser.c

crphp_CFLAGS = -DCRPHP \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/php \
	-I$(top_srcdir)/php/main \
	-I$(top_srcdir)/php/Zend \
	-I$(top_srcdir)/php/TSRM \
	-I$(top_srcdir)/php/sapi/cli
#	-I$(top_srcdir)/php/sapi/crotalus

crphp_LDADD = \
	$(top_builddir)/php/libphp_sapi.a \
	$(top_builddir)/lib/ancillary/libancillary.a \
	$(top_builddir)/lib/pcre/libpcre.a \
	$(top_builddir)/lib/tsrm/libtsrm.a \
	$(top_builddir)/lib/libxml2/libxml2.a

if STATIC_LIBSHARE
crphp_LDADD += $(LIBSHARE)/src/share-lib/libshare.a
crphp_CFLAGS += -I$(LIBSHARE)/include
endif

if HAVE_LIBRT
crphp_LDADD += -lrt
endif
if HAVE_LIBXML
crphp_LDADD += -lxml2
endif
if HAVE_LIBPTHREAD
crphp_LDADD += -lpthread
endif
if HAVE_LIBZ
crphp_LDADD += -lz
else
crphp_LDADD += $(top_builddir)/lib/zlib/libz.a
endif

endif
