AC_INIT([evil], [0.0.1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(1.6 dist-bzip2)

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(VMAJ)
AC_SUBST(version_info)


### Needed information


### Additional options to configure


### Checks for programs
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG


### Checks for libraries

PKG_CHECK_MODULES([ERTF], [eina-0])
PKG_CHECK_MODULES([ECORE_EVAS], [ecore-evas])


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics


### Checks for linker characteristics


### Checks for library functions

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  Installation.........: make install"
echo
echo "    prefix.............: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
