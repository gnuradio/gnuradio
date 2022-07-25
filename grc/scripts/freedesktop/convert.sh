#!/bin/sh

SRC="gnuradio_logo_icon-square.svg"
SIZES="16 24 32 48 64 128 256"

for size in $SIZES
do
    inkscape --without-gui \
        --export-png="grc-icon-$size.png" \
        --export-area=8.0:8.0:141.316:141.316 \
        --export-width=$size \
        --export-height=$size \
        $SRC;
done
