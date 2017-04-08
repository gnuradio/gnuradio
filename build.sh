#!/bin/bash -e

function canon_path {
    if [ -d "$1" ] ; then
        canon_dirname "$@"
    else
        echo $(canon_dirname "$@")/$(basename "$@")
    fi
}

function canon_dirname {
    # dirname isn't smart enough to check if the argument is already a
    # directory, it just gives you one up (unless you give it /, of
    # course)
    local dir=$(dirname "$1")

    # initially, we assume that the argument is a file path, not a
    # directory path.  We therefore assume that we will have to change
    # to the dirname from above to discover the canonical directory
    # path
    local tgt="${dir}"

    # if we are proven wrong, and have been passed a directory, then
    # the target is adjusted accordingly
    if [ -d "${1}" ] ; then
        tgt="$1"
    fi

    # now that we know where to go, we go there, echo the canonical
    # path, and go back
    pushd "${tgt}" > /dev/null 2>&1
    pwd
    popd > /dev/null 2>&1
}

function base_dir(){
    canon_dirname "$(dirname $0)"
}

function check_uhd(){
    if ! dpkg -s uhd > /dev/null ; then
        if apt-cache policy uhd > /dev/null 1>&2; then
            echo "uhd is not installed and isn't available in apt.  See https://github.com/nsat/uhd"
            exit 
        fi
        sudo apt-get install uhd
    fi
}

function init_build_dir(){
    local base_dir="$1"
    local build_dir="$2"
    local version="$3"
    local deb_dir="${build_dir}/deb/DEBIAN"
    mkdir -p "${build_dir}"
    cp -r "${base_dir}/deb" "${build_dir}/"

    local ctrl_name="control.new"
    lsb_release -a | grep -q 12.04 && mv "${deb_dir}/control.1204" "${deb_dir}/control"
    rm -f ${deb_dir}/control.*

    sed -i "s/__version__/${version}/" "${deb_dir}/control"
}

function check_dirty(){
    local dirty=$(git status --porcelain)
    if [[ ! -z $dirty ]]; then
        cat << MSG

    WARNING: Git repo is dirty (see git status --porcelain).  We'll
             still build a debian file and it'll supercede previous
             versions, but it shouldn't be used in production.

MSG

        read -p "Continue (Y/n): " continue
        if [ "$continue" == "n" -o "$continue" == "N" ]; then
            popd
            exit 0
        fi
    fi
}

function calculate_version(){

    local dirty=$(git status --porcelain)

    if [[ ! -z $dirty ]]; then
        suffix="+dirty"
    else
        suffix=""
    fi

    # setup version identifier
    local tag=$(git describe --abbrev=0 --tags | cut -b 2-) # ignore the leading 'v'
    local sha=$(git rev-parse HEAD | cut -b 1-8)

    if [ -z $tag -o -z $sha ]; then 
        echo "ERROR: Unable to retrieve required git identifiers to set version.  Quitting."
        popd
        exit 1
    fi

    local platform="$(lsb_release -a 2>&1 | grep Release | fmt -1 | tail -n 1)"
    echo "$tag-spire-${platform}.${sha}${suffix}"
}

function install_packages(){
    local pkgs_1204="
libboost1.48-all-dev
libqwt-dev
python-qt4
libzeroc-ice34-dev
"

    local pkgs_new="
libboost-all-dev
libqwt-dev
python-qt4
libzeroc-ice35-dev
"

    local base_pkgs="
build-essential
git
cmake
git-core
autoconf
automake
g++
libfftw3-dev
libcppunit-dev
python-cheetah
sdcc
guile-2.0
ccache
libgsl0-dev
libusb-dev
alsa-base
libasound2
libasound2-dev
python-scipy
libtool
python-dev
swig
pkg-config
libfftw3-dev
libcppunit-dev
libgsl0-dev
libusb-dev
sdcc
libsdl1.2-dev
python-wxgtk2.8
python-numpy
python-cheetah
python-lxml
doxygen
libxi-dev
libfontconfig1-dev
libxrender-dev
sphinx-common
python-gtk2
python-lxml
libzmq-dev
libcanberra-gtk-module
"

    local rel_pkgs="${pkgs_new}"
    lsb_release -a | grep -q 12.04 && rel_pkgs="${pkgs_1204}"

sudo apt-get install ${base_pkgs} ${rel_pkgs}
}

function num_threads(){
    local n="$(( $(cat /proc/cpuinfo  | grep processor | wc -l) * 3))"
    [ ${n} -lt 3 ] && echo 3 || echo ${n}
}

function build_and_debify(){
    local version="$1"
    cmake ..
    make -j $(num_threads)
    sudo make install DESTDIR=deb
    sudo chmod -R a+rX deb
    dpkg-deb -b deb gnuradio-$version.deb
}

function main(){

    install_packages

    local base_dir="$(base_dir)"
    local build_dir="${base_dir}/build"
    
    pushd "${base_dir}"

    check_dirty
    local version="$(calculate_version)"

    init_build_dir "${base_dir}" "${build_dir}" "${version}"

    popd

    pushd "${build_dir}"
    build_and_debify "${version}"
    popd
}

main "$@"
