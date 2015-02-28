
sbin_PROGRAMS = crotalus
crotalus_SOURCES = alias.c boa.c buffer.c cgi_header.c config.c escape.c \
	get.c ip.c log.c mmap_cache.c pipe.c queue.c range.c \
	read.c request.c response.c signals.c util.c sublog.c \
	select.c crotalus.c \
	cache/cr_hash.c cache/cr_hash.h \
	cache/cr_pref.c cache/cr_pref.h \
	mime/mime_interp.c mime/mime_interp.h \
	mime/mime_def.c mime/mime_def.h \
	mime/mime_cgi.c mime/mime_cgi.h \
	mime/mime_zlib.c mime/mime_zlib.h
crotalus_LDADD = $(top_builddir)/lib/zlib/libz.a
crotalus_CFLAGS = -I$(top_srcdir)/lib/zlib

if ACCESS_CONTROL
crotalus_SOURCES += cache/cr_access.c cache/cr_access.h
endif

if HAVE_INITD_DIR
EXTRA_DIST = $(top_builddir)/scripts/initd/shared

install-exec-hook:
	rm -f $(INITD_DIR)/init.d/crotalus  
	rm -f $(INITD_DIR)/rc3.d/S86crotalus  
	rm -f $(INITD_DIR)/rc3.d/K86crotalus  
	rm -f $(INITD_DIR)/rc4.d/S86crotalus  
	rm -f $(INITD_DIR)/rc4.d/K86crotalus  
	rm -f $(INITD_DIR)/rc5.d/S86crotalus  
	rm -f $(INITD_DIR)/rc5.d/K86crotalus  
	cp $(top_builddir)/scripts/initd/crotalus $(INITD_DIR)/init.d/
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/crotalus S86crotalus
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/crotalus K86crotalus
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/crotalus S86crotalus
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/crotalus K86crotalus
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/crotalus S86crotalus
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/crotalus K86crotalus

uninstall-hook:
	rm -f $(INITD_DIR)/init.d/crotalus  
	rm -f $(INITD_DIR)/rc3.d/S86crotalus  
	rm -f $(INITD_DIR)/rc3.d/K86crotalus  
	rm -f $(INITD_DIR)/rc4.d/S86crotalus  
	rm -f $(INITD_DIR)/rc4.d/K86crotalus  
	rm -f $(INITD_DIR)/rc5.d/S86crotalus  
	rm -f $(INITD_DIR)/rc5.d/K86crotalus  
endif

