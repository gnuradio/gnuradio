#!/bin/sh

ISSUE=`cut -f 1 -d ' ' /etc/issue`

if test "$ISSUE" = "Ubuntu"; then
    FFTW3DEV=fftw3-dev
else
    FFTW3DEV=libfftw3-dev
fi

# FIXME: these are missing gr-comedi and gr-qtgui dependencies
sudo apt-get install build-essential automake libtool pkg-config \
    python-dev swig $FFTW3DEV libcppunit-dev libboost1.35-dev \
    libgsl0-dev guile-1.8 libasound2-dev python-wxgtk2.8 python-numpy \
    doxygen xmlto sdcc-nf libusb-dev libjack-dev portaudio19-dev \
    libsdl1.2-dev python-opengl python-matplotlib python-tk \
    python-cheetah python-gtk2 python-lxml
