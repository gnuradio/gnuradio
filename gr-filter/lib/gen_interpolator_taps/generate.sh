#!/bin/sh
#
# Copyright (C) 2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

gcc      -c -o diff_objective_fct.o             diff_objective_fct.c
gfortran -c -o praxis.o                         praxis.f
gcc      -c -o gen_interp_differentiator_taps.o gen_interp_differentiator_taps.c

gfortran -o gen_interp_differentiator_taps \
        gen_interp_differentiator_taps.o praxis.o diff_objective_fct.o \
        -lgsl -lgslcblas

./gen_interp_differentiator_taps -n 128 -t 8 -B 0.25 \
        > interp_differentiator_taps.h

gcc      -c -o objective_fct.o         objective_fct.c
gcc      -c -o gen_interpolator_taps.o gen_interpolator_taps.c

gfortran -o gen_interpolator_taps \
        gen_interpolator_taps.o praxis.o objective_fct.o \
        -lgsl -lgslcblas

./gen_interpolator_taps -n 128 -t 8 -B 0.25 \
        > interpolator_taps.h

