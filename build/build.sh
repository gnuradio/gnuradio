#!/bin/bash -e

build_dir="$( cd "$(dirname "$0")" && pwd -P )"
cd "$build_dir"

## Check if uhd is installed
if ! dpkg -s uhd > /dev/null ; then
    if apt-cache policy uhd > /dev/null 1>&2; then
        echo "uhd is not installed and isn't available in apt.  See https://github.com/nsat/uhd"
        exit 
    fi
    sudo apt-get install uhd
fi

## Setup Debian package file
# See if we're building off a clean repo
dirty=$(git status --porcelain)

if [[ ! -z $dirty ]]; then
    cat << MSG

    WARNING: Git repo is dirty (see git status --porcelain).  We'll still build a debian file and
             it'll supercede previous versions, but it shouldn't be used in production.

MSG

    read -p "Continue (Y/n): " continue
    if [ "$continue" == "n" -o "$continue" == "N" ]; then
        exit 0
    fi

    # If repo is dirty, we use wall clock for timestamp instead of last commit time to make this a
    # newer version of the package
    timestamp=$(date +%s%3N)
    suffix="+dirty"
else
    timestamp=$(git log -n1 --format="%at")
    suffix=""
fi

# setup version identifier
tag=$(git describe --abbrev=0 --tags | cut -b 2-) # ignore the leading 'v'
sha=$(git rev-parse HEAD)

if [ -z $tag -o -z $sha -o -z $timestamp ]; then 
    echo "ERROR: Unable to retrieve required git identifiers to set version.  Quitting."
    exit 1
fi
# version format has semantic meaning.  We prioritize on timestamp.  Read debian docs before modifying.
# https://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Version
version="$tag-spire.$timestamp.$sha$suffix"

# debian package metadata.  Installed-size probably isn't accurate; sourced from CPACK-derived template.
cat << CONTROL > deb/DEBIAN/control
Package: gnuradio
Version: $version
Section: devel
Priority: optional
Architecture: amd64
Depends: uhd, python, python-numpy, python-gtk2, python-lxml, python-cheetah, libpulse0, alsa-base, libjack0, libqtcore4, libqwt6, python-qt4, python-qwt5-qt4, libsdl1.2debian, libgsl0ldbl, libboost1.48-all-dev, libcppunit-dev, pkg-config, libfftw3-dev, python-serial, swig
Installed-Size: 264662
Maintainer: jint@spire.com
Description: Customized build of GNU Radio with necessary dependencies for gs-link
CONTROL

## Build GNU Radio and create debian package
# Install superset of required dependencies
sudo apt-get install build-essential git cmake git-core autoconf automake g++ libfftw3-dev libcppunit-dev python-cheetah sdcc guile-2.0 ccache libgsl0-dev libusb-dev alsa-base libasound2 libasound2-dev python-scipy  libtool  python-dev swig pkg-config libboost1.48-all-dev libfftw3-dev libcppunit-dev libgsl0-dev libusb-dev sdcc libsdl1.2-dev python-wxgtk2.8 python-numpy python-cheetah python-lxml doxygen python-qt4 python-qwt5-qt4 libxi-dev libqt4-opengl-dev libqwt5-qt4-dev libfontconfig1-dev libxrender-dev sphinx-common python-gtk2 python-lxml libzeroc-ice34-dev libzmq-dev libqwt5-qt4-dev
# Build and debinify
cmake ..
make
sudo make install DESTDIR=deb
dpkg-deb -b deb gnuradio-$version.deb
