ACLOCAL_AMFLAGS = -I m4
BUILT_SOURCES = $(top_srcdir)/.version
EXTRA_DIST = $(top_srcdir)/.version $(top_srcdir)/libamia.pc.in
pkgconfig_DATA = libamia.pc

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

include $(srcdir)/src/include.mk
include $(srcdir)/include/include.mk
include $(srcdir)/tests/include.mk
