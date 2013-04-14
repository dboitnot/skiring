ACLOCAL_AMFLAGS = -I m4 --install
AM_CPPFLAGS = -DSYSCONFDIR='"$(sysconfdir)"'

bin_PROGRAMS = skiring
skiring_SOURCES = ring.c ring.h coding.c coding.h skiring.c
EXTRA_DIST = m4/NOTES

install-exec-hook:
	chown skiring $(DESTDIR)$(bindir)/skiring$(EXEEXT)
	chmod 4755 $(DESTDIR)$(bindir)/skiring$(EXEEXT)
