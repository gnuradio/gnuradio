#serial 4

# On some hosts (e.g., HP-UX 10.20, SunOS 4.1.4, Solaris 2.5.1), mkstemp has a
# silly limit that it can create no more than 26 files from a given template.
# Other systems lack mkstemp altogether.
# On OSF1/Tru64 V4.0F, the system-provided mkstemp function can create
# only 32 files per process.
# On systems like the above, arrange to use the replacement function.
AC_DEFUN([UTILS_FUNC_MKSTEMP],
[dnl
  AC_REPLACE_FUNCS(mkstemp)
  if test $ac_cv_func_mkstemp = no; then
    utils_cv_func_mkstemp_limitations=yes
  else
    AC_CACHE_CHECK([for mkstemp limitations],
      utils_cv_func_mkstemp_limitations,
      [
	AC_TRY_RUN([
#         include <stdlib.h>
	  int main ()
	  {
	    int i;
	    for (i = 0; i < 70; i++)
	      {
		char template[] = "conftestXXXXXX";
		int fd = mkstemp (template);
		if (fd == -1)
		  exit (1);
		close (fd);
	      }
	    exit (0);
	  }
	  ],
	utils_cv_func_mkstemp_limitations=no,
	utils_cv_func_mkstemp_limitations=yes,
	utils_cv_func_mkstemp_limitations=yes
	)
      ]
    )
  fi

  if test $utils_cv_func_mkstemp_limitations = yes; then
    AC_LIBOBJ(mkstemp)
    AC_LIBOBJ(tempname)
    AC_DEFINE(mkstemp, rpl_mkstemp,
      [Define to rpl_mkstemp if the replacement function should be used.])
    gl_PREREQ_MKSTEMP
    jm_PREREQ_TEMPNAME
  fi
])

# Prerequisites of lib/mkstemp.c.
AC_DEFUN([gl_PREREQ_MKSTEMP],
[
  AH_BOTTOM(
  [
  #ifndef HAVE_MKSTEMP
  #ifdef  __cplusplus
  extern "C" {
  #endif
  int rpl_mkstemp (char *templ);
  #ifdef  __cplusplus
  }
  #endif
  #endif
  ])
])

# Prerequisites of lib/tempname.c.
AC_DEFUN([jm_PREREQ_TEMPNAME],
[
  AC_REQUIRE([AC_HEADER_STAT])
  AC_CHECK_HEADERS_ONCE(fcntl.h sys/time.h unistd.h)
  AC_CHECK_HEADERS(stdint.h)
  AC_CHECK_FUNCS(__secure_getenv gettimeofday lstat)
  AC_CHECK_DECLS_ONCE(getenv)
  # AC_REQUIRE([jm_AC_TYPE_UINTMAX_T])
])
