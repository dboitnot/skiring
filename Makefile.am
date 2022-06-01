ACLOCAL_AMFLAGS = -I m4 --install
AM_CPPFLAGS = -DSYSCONFDIR='"$(sysconfdir)"' -std=gnu11
AM_DISTCHECK_CONFIGURE_FLAGS = skiring_user=$(USER)

bin_PROGRAMS = skiring
bin_SCRIPTS = dbpw
skiring_SOURCES = ring.c ring.h coding.c coding.h skiring.c skiring.h dbpw
EXTRA_DIST = m4/NOTES

if SKIRING_USER_EMPTY
SKIRING_USER = skiring
else
SKIRING_USER = @skiring_user@
endif

if ENABLE_INSTALL_HOOK
install-exec-hook:
	chown $(SKIRING_USER) $(DESTDIR)$(bindir)/skiring$(EXEEXT)
	chmod 4755 $(DESTDIR)$(bindir)/skiring$(EXEEXT)
	mkdir -p $(sysconfdir)/skiring
	chown $(SKIRING_USER) $(sysconfdir)/skiring
	chmod 700 $(sysconfdir)/skiring
	$(DESTDIR)$(bindir)/skiring$(EXEEXT) init
	[ -f $(sysconfdir)/skiring/ring ] || /usr/bin/skiring init
	chmod 600 $(sysconfdir)/skiring/ring
endif

uninstall-hook:
	rm -f $(sysconfdir)/skiring/ring
