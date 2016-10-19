#!/bin/bash -e

# This script should be located in the root of the gnuradio checkout
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Install a superset of the required dependencies
sudo apt-get install -y build-essential git cmake git-core autoconf automake g++ libfftw3-dev libcppunit-dev python-cheetah sdcc guile-2.0 ccache libgsl0-dev libusb-dev alsa-base libasound2 libasound2-dev python-scipy  libtool  python-dev swig pkg-config libboost1.48-all-dev libfftw3-dev libcppunit-dev libgsl0-dev libusb-dev sdcc libsdl1.2-dev python-wxgtk2.8 python-numpy python-cheetah python-lxml doxygen python-qt4 python-qwt5-qt4 libxi-dev libqt4-opengl-dev libqwt5-qt4-dev libfontconfig1-dev libxrender-dev sphinx-common python-gtk2 python-lxml libzeroc-ice34-dev cmake

# Install the UHD driver
echo "UHD: clone"
git clone --depth 1 https://github.com/EttusResearch/uhd.git /tmp/uhd
echo "UHD: mkdir"
mkdir -p /tmp/uhd/host/build
echo "UHD: cd"
cd /tmp/uhd/host/build
echo "UHD: cmake"
cmake ../
echo "UHD: make"
make
echo "UHD: sudo make install"
sudo make install
echo "UHD: done"

# Make sure we can find the uhd shared libraries
sudo ldconfig

# Build and install
echo "GNURADIO: make build dir"
mkdir -p "$script_dir/build"
echo "GNURADIO: cd build directory"
cd "$script_dir/build"
echo "GNURADIO: cmake"
cmake ../
echo "GNURADIO: make"
make
echo "GNURADIO: make install"
sudo make install
echo "GNURADIO: done"

# Cleanup
echo '/usr/local/lib' | sudo tee /etc/ld.so.conf.d/gnuradio.conf
sudo ldconfig

cat << EOF | sudo tee /etc/profile.d/gnuradio.sh
export LD_LIBRARY_PATH="/usr/local/lib"
export PYTHONPATH="/usr/local/lib/python2.7/dist-packages"
export GRC_BLOCKS_PATH="/usr/local/share/gnuradio/grc/blocks"
EOF