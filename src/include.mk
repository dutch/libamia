lib_LTLIBRARIES = %D%/libamia.la
%C%_libamia_la_SOURCES = %D%/amia.c
%C%_libamia_la_CPPFLAGS = -I$(top_srcdir)/include
%C%_libamia_la_LDFLAGS = -version-info 0:0:0

dist_man_MANS = %D%/daemonize.3
