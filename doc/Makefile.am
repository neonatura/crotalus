
info_TEXINFOS = crotalus.texi

dist_docdir = $(datadir)/@PACKAGE@

if HAVE_DOXYGEN
#EXTRA_DIST = crotalus_html.zip
dist_doc_DATA = crotalus_html.zip

crotalus_html.zip:
	mkdir -p html man xml
	$(DOXYGEN) doxygen.conf
	zip -r crotalus_html.zip html

CLEANFILES = crotalus_html.zip
endif

