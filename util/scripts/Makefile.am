bin_PROGRAMS = deltavar listconns listvars logrtest logwtest readall readconn readvar writevar

LIBESTATS_LDADDS = @STRIP_BEGIN@ \
	$(top_builddir)/lib/estats/libestats.la \
	@STRIP_END@

INCLUDES = @STRIP_BEGIN@ \
	-I$(top_srcdir)/lib \
	@STRIP_END@

deltavar_SOURCES = deltavar.c scripts.h
deltavar_LDADD = $(LIBESTATS_LDADDS)

listconns_SOURCES = listconns.c scripts.h
listconns_LDADD = $(LIBESTATS_LDADDS)

listvars_SOURCES = listvars.c scripts.h
listvars_LDADD = $(LIBESTATS_LDADDS)

logrtest_SOURCES = logrtest.c scripts.h
logrtest_LDADD = $(LIBESTATS_LDADDS)

logwtest_SOURCES = logwtest.c scripts.h
logwtest_LDADD = $(LIBESTATS_LDADDS)

readall_SOURCES = readall.c scripts.h
readall_LDADD = $(LIBESTATS_LDADDS)

readconn_SOURCES = readconn.c scripts.h
readconn_LDADD = $(LIBESTATS_LDADDS)

readvar_SOURCES = readvar.c scripts.h
readvar_LDADD = $(LIBESTATS_LDADDS)

writevar_SOURCES = writevar.c scripts.h
writevar_LDADD = $(LIBESTATS_LDADDS)

