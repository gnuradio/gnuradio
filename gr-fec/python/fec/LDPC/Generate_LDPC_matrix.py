#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
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

from Generate_LDPC_matrix_functions import *

# This is an example of how to generate a parity check matrix for
# use with the LDPC Richardson Urbanke encoder. A significant amount
# of matrix manipulation is required, so this process should be done
# before using the encoder at run-time. This process can take quite
# a while, with more time required for larger matrices. 

# Not all attempts to create a parity check matrix will be
# successful. The script will terminate and output error messages
# when the process fails. To increase verbosity, edit the verbose
# variable at the top of Generate_LDPC_matrix_functions.py.

# Because random number generation and
# shuffling methods are used, it is not possible to predict what
# starting conditions will result in success. It requires a bit of 
# trial and error. 

# ----------------------------------------------------------------- #

# First, generate a regular LDPC parity check matrix. Specify
# the properties desired. For example:
n = 200   # number of columns, corresponds to codeword length
p = 3     # column weight
q = 5     # row weight

parity_check_matrix = LDPC_matrix(n_p_q = [n,p,q])

# Richardson and Urbanke's preprocessing method requires a full rank
# matrix to start. The matrices generated by the 
# regular_LDPC_code_contructor function will never be full rank. So,
# use the get_full_rank_H_matrix function. 
newH = get_full_rank_H_matrix(parity_check_matrix.H)

# At this point, the matrix is no longer regular. (The row/column
# weights are not the same for all rows/columns.)

# Next, some preprocessing steps need to be performed as described
# Richardson and Urbanke in Modern Coding Theory, Appendix A. This
# can take a while...
[bestH,g] = get_best_matrix(newH,100)

# Print out some of the resulting properties. 
n = bestH.shape[1]
k = n - bestH.shape[0]
print "Parity check matrix properties:"
print "\tSize :", bestH.shape
print "\tRank :", linalg.matrix_rank(bestH)
print "\tRate : %.3f" % ((k*1.0)/n)
print "\tn    :", n, " (codeword length)"
print "\tk    :", k, " (info word length)"
print "\tgap  : %i" % g

# Save the matrix to an alist file for future use: 
alist_filename = "n_%04i_k_%04i_gap_%02i.alist" % (n,k,g)
parity_check_matrix.write_alist_file(alist_filename,bestH)
print '\nMatrix saved to alist file:', alist_filename, "\n"