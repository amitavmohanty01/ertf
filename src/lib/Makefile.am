MAINTAINERCLEANFILES = Makefile.in

installed_headersdir = $(includedir)/ertf
dist_installed_headers_DATA = \
Ertf.h \
ertf_main.h \
colour.h \
font.h \
input.h \
rtf_to_markup.h \
stylesheet.h \
summary.h

lib_LTLIBRARIES = libertf.la

libertf_la_SOURCES = \
ertf_main.c \
colour.c \
font.c \
input.c \
rtf_to_markup.c \
stylesheet.c \
summary.c

libertf_la_CPPFLAGS = @ERTF_CFLAGS@
libertf_la_LIBADD = @ERTF_LIBS@
libertf_la_LDFLAGS = -no-undefined -version-info @version_info@

EXTRA_DIST = ertf_private.h
