from distutils.core import setup, Extension

setup(name="libestats",
      description="SWIG wrapper for libestats",
      url="http://www.web10g.org/",
      ext_modules=[Extension("_libestats", ["libestats_wrap.c"], \
                             include_dirs=["../lib"], \
                             library_dirs=["../lib/estats/.libs"], libraries=["estats"])],
      py_modules=['libestats'])

setup(name="Web10G",
      version="1.0",
      description="Web100 statistics interface",
      author="John Heffner",
      author_email="jheffner@psc.edu",
      url="http://www.web10g.org/",
      license="LGPL 1.2",
      py_modules=['Web10G'])
