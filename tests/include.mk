EXTRA_DIST += %D%/testsuite.at %D%/package.m4 %D%/testsuite
AUTOM4TE = $(top_srcdir)/build-aux/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=Autotest
DISTCLEANFILES = %D%/atconfig $(top_builddir)/testsuite.log

%D%/package.m4: $(top_srcdir)/configure.ac
	:;{ \
	  echo 'm4_define([AT_PACKAGE_NAME], [$(PACKAGE_NAME)])' && \
	  echo 'm4_define([AT_PACKAGE_TARNAME], [$(PACKAGE_TARNAME)])' && \
	  echo 'm4_define([AT_PACKAGE_VERSION], [$(PACKAGE_VERSION)])' && \
	  echo 'm4_define([AT_PACKAGE_STRING], [$(PACKAGE_STRING)])' && \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT], [$(PACKAGE_BUGREPORT)])' && \
	  echo 'm4_define([AT_PACKAGE_URL], [$(PACKAGE_URL)])'; \
	} >$@

%D%/testsuite: %D%/testsuite.at %D%/package.m4
	$(AUTOTEST) -I %D% -o $@.tmp $(top_srcdir)/$@.at && mv $@.tmp $@

check-local: %D%/atconfig %D%/testsuite
	$(top_srcdir)/%D%/testsuite $(TESTSUITEFLAGS)

installcheck-local: %D%/atconfig %D%/testsuite
	$(top_srcdir)/%D%/testsuite AUTOTEST_PATH='$(bindir)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f %D%/testsuite || %D%/testsuite --clean
