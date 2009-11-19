ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
config.guess \
config.h.in \
config.h.in~ \
config.sub \
configure \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing

EXTRA_DIST = \
AUTHORS \
COPYING \
README \
autogen.sh \
ertf.pc.in \
m4/efl_doxygen.m4

.PHONY: doc

# Documentation

doc:
	@echo "entering doc/"
	make -C doc doc
