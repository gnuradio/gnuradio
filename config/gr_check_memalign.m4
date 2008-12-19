# Copyright 2008 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Boston, MA
# 02110-1301, USA.

AC_DEFUN([GR_CHECK_MEMALIGN],
[
  AC_REQUIRE([AC_CANONICAL_HOST])

  # Check for allocing memory alignment functions
  # If 'posix_memalign' is available, use it solely.
  AC_CHECK_FUNCS([posix_memalign],,[
    # Otherwise, check for valloc and the
    # (a-priori known) alignment of 'malloc'
    AC_CHECK_FUNCS([valloc])
    aligned_malloc=0
    # 'malloc' is 16-byte aligned on (at least) Darwin 8 and 9
    case "$host_os" in
      darwin8*) aligned_malloc=16 ;;
      darwin9*) aligned_malloc=16 ;;
      *) ;;
    esac
    if test $aligned_malloc = 0; then
      AC_MSG_RESULT([malloc has unknown alignment.])
    else
      AC_MSG_RESULT([malloc is $aligned_malloc-byte aligned.])
    fi
    AC_DEFINE_UNQUOTED([ALIGNED_MALLOC],[$aligned_malloc],[Define as the alignment of malloc if known; otherwise 0.])
  ])
])
