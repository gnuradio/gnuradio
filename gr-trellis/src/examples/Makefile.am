#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

SUBDIRS = fsm_files

EXTRA_DIST =			\
        README			\
	fsm_utils.py		\
	test_tcm.py		\
	test_tcm1.py		\
	test_tcm2.py          	\
	test_tcm_parallel.py	\
	test_tcm_combined.py	\
	test_sccc_hard.py	\
	test_sccc_soft.py     	\
	test_sccc_turbo.py    	\
	test_viterbi_equalization1.py	\
	test_viterbi_equalization.py	\
	test_turbo_equalization.py	\
	test_turbo_equalization1.py	\
	test_turbo_equalization2.py	


ourdatadir = $(exampledir)/trellis
ourdata_DATA = $(EXTRA_DIST)

# Make example scripts with #! executable
install-data-local: install-ourdataDATA
	for i in `find $(ourdatadir) -type f ! -perm 755`; do \
	  if head -1 $$i | grep -q '^#!'; then \
	    chmod 755 $$i; \
	    echo "made executable: $$i"; \
	  fi; \
	done

MOSTLYCLEANFILES = *.pyc
