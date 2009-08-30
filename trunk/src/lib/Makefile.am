MAINTAINERCLEANFILES = Makefile.in

installed_headersdir = $(includedir)/ertf
dist_installed_headers_DATA = \
Ertf.h \
ertf_main.h \
ertf_color.h \
ertf_font.h \
ertf_input.h \
ertf_rtf_to_markup.h \
ertf_stylesheet.h \
ertf_summary.h

lib_LTLIBRARIES = libertf.la

libertf_la_SOURCES = \
ertf_main.c \
ertf_color.c \
ertf_font.c \
ertf_input.c \
ertf_rtf_to_markup.c \
ertf_stylesheet.c \
ertf_summary.c

libertf_la_CPPFLAGS = @ERTF_CFLAGS@
libertf_la_LIBADD = @ERTF_LIBS@
libertf_la_LDFLAGS = -no-undefined -version-info @version_info@

EXTRA_DIST = ertf_private.h
