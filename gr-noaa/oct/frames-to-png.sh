#!/bin/sh

octave --eval frames_to_ppm

convert chan1.ppm chan1.png && rm chan1.ppm
convert chan2.ppm chan2.png && rm chan2.ppm
convert chan3.ppm chan3.png && rm chan3.ppm
convert chan4.ppm chan4.png && rm chan4.ppm
convert chan5.ppm chan5.png && rm chan5.ppm
