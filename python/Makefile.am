all-am: Makefile python-build swig-build


python-build: libestats.py libestats_wrap.c
	$(PYTHON) $(srcdir)/setup.py build_ext --include-dirs=$(top_srcdir)/lib/estats:$(top_srcdir)/python
	$(PYTHON) $(srcdir)/setup.py build_py

python-install:
	$(PYTHON) $(srcdir)/setup.py install --install-purelib=${pythondir} --install-platlib=${pyexecdir}

python-clean:
	$(PYTHON) $(srcdir)/setup.py clean

if BUILD_SWIG

#libestats.py libestats_wrap.c: $(top_srcdir)/python/libestats.i
swig-build: $(top_srcdir)/python/libestats.i
	$(SWIG) -python -I$(top_srcdir)/lib -o libestats_wrap.c $(srcdir)/libestats.i

else

swig-build:

endif

SWIG_OUTPUTS = libestats.py libestats_wrap.c

# Maintainer-clean the swig generated files, however, include them in EXTRA_DIST
MAINTAINERCLEANFILES = $(SWIG_OUTPUTS)

install-exec-hook: python-install

uninstall-local: python-clean

EXTRA_DIST=Web10G.py libestats.i libestats.py libestats_wrap.c readall.py setup.py
