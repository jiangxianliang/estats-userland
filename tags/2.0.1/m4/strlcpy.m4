AC_DEFUN([FUNC_STRLCPY],
[
  AC_CHECK_FUNCS(strlcpy,
                 [need_strlcpy=false],
                 [need_strlcpy=true])

  AM_CONDITIONAL(BUILD_LOCAL_STRLCPY, test x$need_strlcpy = xtrue)
  AC_DEFINE([USE_LOCAL_STRLCPY], 1,
            [Define when using local strlcpy implementation])
])
