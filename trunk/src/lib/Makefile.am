MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libertf.la

libertf_la_SOURCES = \
colour.c \
font.c \
input.c \
rtf_to_markup.c \
stylesheet.c \
summary.c

libertf_la_CPPFLAGS = @ERTF_CFLAGS@
libertf_la_LIBADD = @ERTF_LIBS@
libertf_la_LDFLAGS = -no-undefined -version-info @version_info@

EXTRA_DIST = \
colour.h \
font.h \
input.h \
rtf_to_markup.h \
stylesheet.h \
summary.h