noinst_LTLIBRARIES = libcompat.la

if BUILD_LOCAL_STRLCPY
STRLCPY_SRC = strlcpy.c strlcpy.h
else
STRLCPY_SRC =
endif

libcompat_la_SOURCES = \
  $(STRLCPY_SRC)
