MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = reader

reader_SOURCES = reader.c GUI.c

reader_CPPFLAGS = -I$(top_srcdir)/src/lib @ECORE_EVAS_CFLAGS@
reader_LDADD = @ECORE_EVAS_LIBS@ $(top_builddir)/src/lib/libertf.la

EXTRA_DIST = GUI.h
