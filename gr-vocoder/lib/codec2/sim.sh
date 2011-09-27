#!/bin/sh
# sim.sh
# David Rowe 10 Sep 2009

# Process a source file using the codec 2 simulation.  An output
# speech file is generated for each major processing step, from the
# unquantised siusoidal model to fully quantised.  This way we can
# listen to the effect of each processing step.  Use listensim.sh to
# test the output files.

../src/c2sim ../raw/$1.raw -o $1_uq.raw
../src/c2sim ../raw/$1.raw --phase0 -o $1_phase0.raw --postfilter
../src/c2sim ../raw/$1.raw --lpc 10 -o $1_lpc10.raw --postfilter
../src/c2sim ../raw/$1.raw --phase0 --lpc 10 -o $1_phase0_lpc10.raw --postfilter
../src/c2sim ../raw/$1.raw --phase0 --lpc 10 --dec -o $1_phase0_lpc10_dec.raw --postfilter
../src/c2sim ../raw/$1.raw --phase0 --lpc 10 --lsp --dec -o $1_phase0_lsp_dec.raw --postfilter

#../src/c2sim ../raw/$1.raw --lpc 10 --lsp -o $1_lsp.raw
#../src/c2sim ../raw/$1.raw --phase0 --lpc 10 -o $1_phase0_lpc10.raw --postfilter
#../src/c2sim ../raw/$1.raw --phase0 --lpc 10 --lsp -o $1_phase0_lsp.raw --postfilter
#../src/c2sim ../raw/$1.raw --phase0 --lpc 10 --lsp -o $1_phase0_lsp_dec.raw --postfilter --dec

