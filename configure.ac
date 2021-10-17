AC_INIT([libamia],
[m4_esyscmd([build-aux/git-version-gen .tarball-version])],
[https://github.com/dutch/libamia/issues],
[],
[https://github.com/dutch/libamia])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_TESTDIR([tests])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_PROG_AR
LT_INIT
AMIA_INIT
AC_PROG_CC
PKG_INSTALLDIR
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile libamia.pc])
AC_OUTPUT
