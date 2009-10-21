MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = ertf_evas_test ertf_esmart_test

ertf_evas_test_SOURCES = ertf_evas_test.c

ertf_evas_test_CPPFLAGS = -I$(top_srcdir)/src/lib @ECORE_EVAS_CFLAGS@
ertf_evas_test_LDADD = @ECORE_EVAS_LIBS@ $(top_builddir)/src/lib/libertf.la

ertf_esmart_test_SOURCES = ertf_esmart_test.c

ertf_esmart_test_CPPFLAGS = -I$(top_srcdir)/src/lib @ECORE_EVAS_CFLAGS@
ertf_esmart_test_LDADD = @ECORE_EVAS_LIBS@ $(top_builddir)/src/lib/libertf.la

EXTRA_DIST = GUI.h
