#!/bin/sh
# fq20.shsh
# David Rowe 27 July 2010
# 
# Decode a file with fully quantised codec at 20ms frame rate

../src/sinedec ../raw/$1.raw $1.mdl -o $1_phase0_lsp_20_EWo2.raw --phase 0 --lpc 10 --lsp --postfilter --dec

