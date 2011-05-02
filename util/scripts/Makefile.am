bin_PROGRAMS = deltavar listconns listvars logread_example logwrite_example readall readconn readvar writevar

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

logread_example_SOURCES = logread_example.c scripts.h
logread_example_LDADD = $(LIBESTATS_LDADDS)

logwrite_example_SOURCES = logwrite_example.c scripts.h
logwrite_example_LDADD = $(LIBESTATS_LDADDS)

readall_SOURCES = readall.c scripts.h
readall_LDADD = $(LIBESTATS_LDADDS)

readconn_SOURCES = readconn.c scripts.h
readconn_LDADD = $(LIBESTATS_LDADDS)

readvar_SOURCES = readvar.c scripts.h
readvar_LDADD = $(LIBESTATS_LDADDS)

writevar_SOURCES = writevar.c scripts.h
writevar_LDADD = $(LIBESTATS_LDADDS)

