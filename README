GNU Radio - Spire Edition!
===

```
Copyright 2001-2007,2009,2012 Free Software Foundation, Inc.

This file is part of GNU Radio

GNU Radio is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GNU Radio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Radio; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street,
Boston, MA 02110-1301, USA.
```

This is a Spire fork of the GNU Radio project made mostly to enforce internal
versioning and build procedures.  Little, if any, internal code has been
modified.

Please see http://gnuradio.org for the wiki, bug tracking,
and source code viewer.

If you've got questions about GNU Radio, please subscribe to the
discuss-gnuradio mailing list and post your questions there.
http://gnuradio.org/redmine/projects/gnuradio/wiki/MailingLists

There is also a "Build Guide" in the wiki that contains OS specific
recommendations:
http://gnuradio.org/redmine/projects/gnuradio/wiki/BuildGuide

For information about using Git, please see:
http://gnuradio.org/redmine/projects/gnuradio/wiki/DevelopingWithGit


## How to Build GNU Radio:

Spire has modified the build process to generate a debian package customized
for internal use.  To build:

```
cd build
./build.sh
```
The original directions for compilation can be found in the README.md at 
https://github.com/gnuradio/gnuradio.


## KNOWN INCOMPATIBILITIES


  GNU Radio triggers bugs in g++ 3.3 for X86.  DO NOT USE GCC 3.3 on
  the X86 platform.  g++ 3.2, 3.4, and the 4.* series are known to work well.

## NOTES

To run the examples you may need to set PYTHONPATH.  Note that the
prefix and python version number in the path needs to match your
installed version of python.

```
$ export PYTHONPATH=/usr/local/lib/python2.7/dist-packages
```

You may want to add this to your shell init file (~/.bash_profile if
you use bash).


Another handy trick if for example your fftw includes and libs are
installed in, say ~/local/include and ~/local/lib, instead of
/usr/local is this:

```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/local/lib
$ make CPPFLAGS="-I$HOME/local/include"
```

Sometimes the prerequisites are installed in a location which is not
included in the default compiler and linker search paths.  This
happens with pkgsrc and NetBSD.  To build, tell configure to use these
locations:

```
LDFLAGS="-L/usr/pkg/lib -R/usr/pkg/lib" CPPFLAGS="-I/usr/pkg/include" ./configure --prefix=/usr/gnuradio
```

##Legal Matters

Some files have been changed many times throughout the
years. Copyright notices at the tops of these files list which years
changes have been made. For some files, changes have occurred in many
consecutive years. These files may often have the format of a year
range (e.g., "2006 - 2011"), which indicates that these files have had
copyrightable changes made during each year in the range, inclusive.

