# Header files for public installation
estats_pub_h_sources = \
	agent.h \
	connection.h \
	error.h \
	estats.h \
	log.h \
	snapshot.h \
	sockinfo.h \
	types.h \
	value.h \
	var.h

# Header files not installed
estats_pri_h_sources = \
	debug-int.h \
	error-int.h \
	estats-int.h \
	group-int.h \
	libcwrap-int.h \
	list-int.h \
	string-int.h \
	types-int.h \
	value-int.h \
	var-int.h
	
# C sources to build the library from
estats_c_sources = \
	agent.c \
	connection.c \
	debug-int.c \
	error.c \
	group.c \
	libcwrap-int.c \
	log.c \
	snapshot.c \
	sockinfo.c \
	string-int.c \
	value.c \
	var.c

# cpp flags to be used with all compilation
# Note: -D_GNU_SOURCE required so headers declare vfscanf (possibly among
#       others)
AM_CPPFLAGS = @STRIP_BEGIN@ \
	-I$(top_srcdir)/compat \
	-I$(top_srcdir)/lib \
	-D_GNU_SOURCE \
	@STRIP_END@

# Libraries to compile and install
lib_LTLIBRARIES = libestats.la

libestats_la_LDFLAGS = @STRIP_BEGIN@ \
	-version-info $(LIBESTATS_LT_CURRENT):$(LIBESTATS_LT_REVISION):$(LIBESTATS_LT_AGE) \
	-export-symbols-regex '^estats_.*' \
	@STRIP_END@

libestats_la_SOURCES = $(estats_c_sources) $(estats_pub_h_sources) $(estats_pri_h_sources)
libestats_la_LIBADD = $(top_builddir)/compat/libcompat.la

# Install headers into include/estats
# NOTE: If you update this, be sure to update estats-config
libestatsincludedir = $(ESTATS_INCLUDE_DIR)/estats
libestatsinclude_HEADERS = $(estats_pub_h_sources)
