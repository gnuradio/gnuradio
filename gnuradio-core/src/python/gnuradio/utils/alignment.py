#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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
"""
This module contains functions for aligning sequences.

>>> import random
>>> random.seed(1234)
>>> ran_seq = [random.randint(0,1) for i in range(0, 100)]
>>> offset_seq = [0] * 20 + ran_seq
>>> correct, overlap, offset = align_sequences(ran_seq, offset_seq)
>>> print(correct, overlap, offset)
(1.0, 100, -20)
>>> offset_err_seq = []
>>> for bit in offset_seq:
...     if random.randint(0,4) == 4:
...         offset_err_seq.append(random.randint(0,1))
...     else:
...         offset_err_seq.append(bit)
>>> correct, overlap, offset = align_sequences(ran_seq, offset_err_seq)
>>> print(overlap, offset)
(100, -20)

"""

import random

# DEFAULT PARAMETERS
# If the fraction of matching bits between two sequences is greater than
# this the sequences are assumed to be aligned.
def_correct_cutoff = 0.9
# The maximum offset to test during sequence alignment.
def_max_offset = 500
# The maximum number of samples to take from two sequences to check alignment.
def_num_samples = 1000

def compare_sequences(d1, d2, offset, sample_indices=None):
    """
    Takes two binary sequences and an offset and returns the number of
    matching entries and the number of compared entries.
    d1 & d2 -- sequences
    offset -- offset of d2 relative to d1
    sample_indices -- a list of indices to use for the comparison
    """
    max_index = min(len(d1), len(d2)+offset)
    if sample_indices is None:
        sample_indices = range(0, max_index)
    correct = 0
    total = 0
    for i in sample_indices:
        if i >= max_index:
            break
        if d1[i] == d2[i-offset]:
            correct += 1
        total += 1
    return (correct, total)

def random_sample(size, num_samples=def_num_samples, seed=None):
    """
    Returns a set of random integers between 0 and (size-1).
    The set contains no more than num_samples integers.
    """
    random.seed(seed)
    if num_samples > size:
        indices = set(range(0, size))
    else:
        if num_samples > size/2:
            num_samples = num_samples/2
        indices = set([])
        while len(indices) < num_samples:
            index = random.randint(0, size-1)
            indices.add(index)
    indices = list(indices)
    indices.sort()
    return indices

def align_sequences(d1, d2,
                    num_samples=def_num_samples,
                    max_offset=def_max_offset,
                    correct_cutoff=def_correct_cutoff,
                    seed=None,
                    indices=None):
    """
    Takes two sequences and finds the offset and which the two sequences best
    match.  It returns the fraction correct, the number of entries compared,
    the offset.
    d1 & d2 -- sequences to compare
    num_samples -- the maximum number of entries to compare
    max_offset -- the maximum offset between the sequences that is checked
    correct_cutoff -- If the fraction of bits correct is greater than this then
                      the offset is assumed to optimum.
    seed -- a random number seed
    indices -- an explicit list of the indices used to compare the two sequences
    """
    max_overlap = max(len(d1), len(d2))
    if indices is None:
        indices = random_sample(max_overlap, num_samples, seed)
    max_frac_correct = 0
    best_offset = None
    best_compared = None
    best_correct = None
    pos_range = range(0, min(len(d1), max_offset))
    neg_range = range(-1, -min(len(d2), max_offset), -1)
    # Interleave the positive and negative offsets.
    int_range = [item for items in zip(pos_range, neg_range) for item in items]
    for offset in int_range:
        correct, compared = compare_sequences(d1, d2, offset, indices)
        frac_correct = 1.0*correct/compared
        if frac_correct > max_frac_correct:
            max_frac_correct = frac_correct
            best_offset = offset
            best_compared = compared
            best_correct = correct
            if frac_correct > correct_cutoff:
                break
    return max_frac_correct, best_compared, best_offset, indices
    
if __name__ == "__main__":
    import doctest
    doctest.testmod()
    
