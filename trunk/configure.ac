AC_INIT([ertf], [0.0.1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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

requirement_ertf=""
AC_SUBST(requirement_ertf)


### Additional options to configure

use_dpi="no"
AC_ARG_ENABLE([dpi],
   AC_HELP_STRING([--enable-dpi], [enable dpi. @<:@default==disabled@:>@]),
   [use_dpi="yes"],
   [use_dpi="no"])

if test "x${use_dpi}" = "xyes" ; then
   AC_DEFINE(USE_DPI, 1, [Set to 1 if DPI is used])
fi


### Checks for programs
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)

# doxygen
EFL_CHECK_DOXYGEN()

### Checks for libraries

PKG_CHECK_MODULES([ERTF], [evas >= 1.0.0 eina >= 1.0.0])
PKG_CHECK_MODULES([ECORE_EVAS], [ecore-evas])

requirement_ertf="evas eina"

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O

EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])
EFL_COMPILER_FLAG([-Wshadow])


### Checks for linker characteristics


### Checks for library functions

AC_CONFIG_FILES([
Makefile
doc/Makefile
src/Makefile
src/bin/Makefile
src/lib/Makefile
ertf.pc
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
echo "  Documentation........: ${efl_have_doxygen}"
echo "  DPI..................: ${use_dpi}"
echo
echo "  Installation.........: make install"
echo
echo "    prefix.............: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
