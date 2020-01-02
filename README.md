<p align="center">
<img src="https://github.com/gnuradio/gnuradio/blob/master/docs/gnuradio.png" width="75%" />
</p>

[![Build](https://shield.lwan.ws/img/p5UKbS/weekly_runner)](https://ci.gnuradio.org/buildbot/#/)
![Version](https://img.shields.io/github/tag/gnuradio/gnuradio.svg)
[![AUR](https://img.shields.io/github/license/gnuradio/gnuradio)](https://github.com/gnuradio/gnuradio/blob/master/COPYING) 
[![Docs](https://img.shields.io/badge/docs-doxygen-orange.svg)](https://www.gnuradio.org/doc/doxygen/)
[![Packaging status](https://repology.org/badge/tiny-repos/gnuradio.svg)](https://repology.org/project/gnuradio/badges)

GNU Radio is a free & open-source software development toolkit that 
provides signal processing blocks to implement software radios. It can 
be used with readily-available, low-cost external RF hardware to create 
software-defined radios, or without hardware in a simulation-like 
environment. It is widely used in hobbyist, academic, and commercial 
environments to support both wireless communications research and real-world 
radio systems.

Please visit the GNU Radio website at https://www.gnuradio.org/ and the 
wiki at https://wiki.gnuradio.org/. Bugs and feature requests are 
tracked on GitHub's [Issue Tracker](https://github.com/gnuradio/gnuradio/issues). 
If you have questions about GNU Radio, please search the **discuss-gnuradio** 
mailing list [archive](https://lists.gnu.org/archive/html/discuss-gnuradio/), 
as many questions have already been asked and answered. Please also 
[subscribe](https://lists.gnu.org/mailman/listinfo/discuss-gnuradio) to 
the mailing list and post your new questions there.


## How to Install GNU Radio

### Prebuilt Binaries

The recommended way to install GNU Radio on most platforms is using available binary package distributions. 

The following command is for Debian, Ubuntu, and derivatives. It will install Release 3.7 with Python2. 

    sudo apt install gnuradio

For other operating systems, see [Installing from Binaries](https://wiki.gnuradio.org/index.php/InstallingGR#From_Binaries)

### PyBOMBS

PyBOMBS is good at building GNU Radio, UHD, and various Out of Tree (OOT) modules from source and then installing into a specified user directory rather than in the system files. PyBOMBS detects the user's Operating System and loads all of the prerequisites in the first stage of the build.

For a quick start, open a terminal window and enter the following commands. This will install Release 3.8 with Python3.

    sudo -H pip3 install PyBOMBS
    pybombs auto-config
    pybombs recipes add-defaults
    pybombs prefix init ~/gnuradio -R gnuradio-default

<b>Wait</b>. The terminal will show the progress.

To run GNU Radio Companion, enter:

        pybombs run gnuradio-companion

Complete PyBOMBS instructions are in the [PyBOMBS README](https://github.com/gnuradio/pybombs#pybombs).

### From Source

Complete instructions for building Gnuradio from source code are detailed in 
[Installing GR From Source](https://wiki.gnuradio.org/index.php/InstallingGR#From_Source). 

## Legal Matters

Some files have been changed many times throughout the years. Copyright 
notices at the top of source files list which years changes have been 
made. For some files, changes have occurred in many consecutive years. 
These files may often have the format of a year range (e.g., "2006 - 2011"), 
which indicates that these files have had copyrightable changes made 
during each year in the range, inclusive.
