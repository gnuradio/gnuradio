#!/bin/sh
#
# Copyright (C) 2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
set -e
CC=gcc
F=gfortran
FFLAGS="$FFLAGS -std=legacy -Wall"
CFLAGS="$CFLAGS -std=c99 -Wall -Wpedantic"
$CC $CFLAGS -c -o diff_objective_fct.o diff_objective_fct.c
$F $FFLAGS -c -o praxis.o praxis.f
$CC $CFLAGS -c -o gen_interp_differentiator_taps.o gen_interp_differentiator_taps.c

$F $FFLAGS -o gen_interp_differentiator_taps gen_interp_differentiator_taps.o praxis.o diff_objective_fct.o -lgsl -lgslcblas

./gen_interp_differentiator_taps -n 128 -t 8 -B 0.25 > interp_differentiator_taps.h

$CC $CFLAGS -c -o objective_fct.o objective_fct.c
$CC $CFLAGS -c -o gen_interpolator_taps.o gen_interpolator_taps.c

$F $FFLAGS -o gen_interpolator_taps gen_interpolator_taps.o praxis.o objective_fct.o -lgsl -lgslcblas

./gen_interpolator_taps -n 128 -t 8 -B 0.25 > interpolator_taps.h

