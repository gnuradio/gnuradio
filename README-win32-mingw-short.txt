Building and using gnuradio on windows (win32) using mingw

Required tools and libraries

MingW and Msys
Download the latest stable version from the mingw site oand follow their installation instructions)

Python for windows version 2.4 (or higher)
You do not need to build this yourself.
You can just install the windows executable which you can find on the python site.
I am not sure if it will work if you have speces in your python pathname.
I recommend installing it in C:\Python24 or D:\Python24

Libtool
If you are building from cvs you need a recent libtool
msys comes with libtool, but the version distributed with current mingw doesn't work with gnuradio.
download, build and install a recent libtool

cppunit
Build and install cppunit

boost
build and install boost (maybe you can get away with only unpacking the source, we only use the boost header files)
If you build boost, you first have to download jam (boost jam) for win32. (Do not use build and use the cygwin version)

build environment:
You need to have the following files on your PATH:
python.exe python24.dll libcppunit-1-10-2.dll libfftw3f-3.dll fftwf-wisdom.exe cppunit-config
If you have cygwin installed Make sure that NO cygwin executables are on your path.

needed on PATH:
/usr/local/bin
/mingw/bin
/bin
/c/Python24/		python.exe
/c/Python24/libs	python24.dll
/c/Python24/DLLs
/usr/local/bin or /mingw/bin or /my/special/installed/lib/folder/bin
			libcppunit-1-10-2.dll
			libfftw3f-3.dll
			fftw-wisdom-to-conf
			fftwf-wisdom.exe
			cppunit-config
/c/WINNT/system32
/c/WINNT


I made a little script set_clean_path.sh to set my path for building gnuradio where I just discard the original PATH (to get rid of the cygwin executables on my default path)  and just include what is needed:

#!/bin/sh
export PATH=".:/usr/local/bin:/mingw/bin:/bin:/c/Python24:/c/Python24/DLLs:/c/Python24/libs:/my/special/installed/lib/folder/bin:/c/WINNT/system32:/c/WINNT:/c/WINNT/System32/Wbem:.

You need to source this script to set the PATH.
. ./set_clean_path.sh
(notice the extra dot and space in the beginning of the line, this means source this file. Sourcing means execute it and remember all environment variables set in this script)

If you are building from cvs it is recommended that you edit bootstrap to your needs and use it
If you built a recent libtool and didn't overwrite the original libtool
(because you installed the new version in /usr/local) then you have to tell aclocal to use the more recent libtool m4 macros.
You can do this by appending -I /usr/local/share/aclocal to the aclocal commandline
I also changed the aclocal and automake invocations to use the most recent version in my bootstrap script
Here follows the bootstrap script I use

#!/bin/sh
rm -fr config.cache autom4te*.cache

aclocal-1.8 -I config -I /usr/local/share/aclocal
autoconf
autoheader
libtoolize --automake
automake-1.8 --add-missing

If you run this script it will convert a clean cvs checkout to a version which you can configure, build and install

So now you can configure gnuradio.
On win32 /mingw you need to give it a few parameters
You need to tell it where cppunit is installed
where boost include files are to be found
where the pkg-config of libfftw is to be found
to use a generic cpu (no 3Dnow,SSE,MMX) (This option will not be needed anymore soon)
If you have boost installed in C:\boost_1_32_0 and cppunit and fftw in /usr/local then you would need the following configur commandline
$ ./configure --with-md-cpu=generic --with-cppunit-prefix=/usr/local --with-boost-include-dir=/c/boost_1_32_0/include/boost-1_32 PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

If everything went well you cannow do
make
make install

Now you have a working gnuradio-core
Now you can go on building and installing gr-audio-windows and windows and wxgui
remember that all gnuradio and python dlls need to be on your path to use gnuradio
The gnuradio dlls are installed at
/c/Python24/Lib/site-packages:/c/Python24/Lib/site-packages/gnuradio:/c/Python24/Lib/site-packages/gnuradio/gr
