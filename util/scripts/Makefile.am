bin_PROGRAMS = deltavar listconn listvars logtest logrtest logwtest readall readconn readvar writevar

LIBESTATS_NOGTK_LDADDS = @STRIP_BEGIN@ \
	$(top_builddir)/lib/estats/libestats.la \
	@STRIP_END@

INCLUDES = @STRIP_BEGIN@ \
	-I$(top_srcdir)/lib \
	@STRIP_END@

deltavar_SOURCES = deltavar.c scripts.h
deltavar_LDADD = $(LIBESTATS_NOGTK_LDADDS)

listconn_SOURCES = listconn.c scripts.h
listconn_LDADD = $(LIBESTATS_NOGTK_LDADDS)

listvars_SOURCES = listvars.c scripts.h
listvars_LDADD = $(LIBESTATS_NOGTK_LDADDS)

logtest_SOURCES = logtest.c scripts.h
logtest_LDADD = $(LIBESTATS_NOGTK_LDADDS)

logrtest_SOURCES = logrtest.c scripts.h
logrtest_LDADD = $(LIBESTATS_NOGTK_LDADDS)

logwtest_SOURCES = logwtest.c scripts.h
logwtest_LDADD = $(LIBESTATS_NOGTK_LDADDS)

readall_SOURCES = readall.c scripts.h
readall_LDADD = $(LIBESTATS_NOGTK_LDADDS)

readconn_SOURCES = readconn.c scripts.h
readconn_LDADD = $(LIBESTATS_NOGTK_LDADDS)

readvar_SOURCES = readvar.c scripts.h
readvar_LDADD = $(LIBESTATS_NOGTK_LDADDS)

writevar_SOURCES = writevar.c scripts.h
writevar_LDADD = $(LIBESTATS_NOGTK_LDADDS)

