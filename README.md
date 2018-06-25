<p align="center">
<img src="https://github.com/gnuradio/gnuradio/blob/master/docs/gnuradio.png" width="75%" />
</p>

[![Build](https://shield.lwan.ws/img/p5UKbS/weekly_runner)](https://ci.gnuradio.org/buildbot/#/)
![Version](https://img.shields.io/github/tag/gnuradio/gnuradio.svg)
[![AUR](https://img.shields.io/aur/license/yaourt.svg)](https://github.com/gnuradio/gnuradio/blob/master/COPYING) 
[![Docs](https://img.shields.io/badge/docs-doxygen-orange.svg)](https://gnuradio.org/doc/doxygen/)

GNU Radio is a free & open-source software development toolkit that 
provides signal processing blocks to implement software radios. It can 
be used with readily-available, low-cost external RF hardware to create 
software-defined radios, or without hardware in a simulation-like 
environment. It is widely used in hobbyist, academic, and commercial 
environments to support both wireless communications research and real-world 
radio systems.

Please visit the GNU Radio website at https://gnuradio.org/ and the 
wiki at https://wiki.gnuradio.org/. Bugs and feature requests are 
tracked on GitHub's [Issue Tracker](https://github.com/gnuradio/gnuradio/issues). 
If you have questions about GNU Radio, please search the **discuss-gnuradio** 
mailing list [archive](https://lists.gnu.org/archive/html/discuss-gnuradio/), 
as many questions have already been asked and answered. Please also 
[subscribe](https://lists.gnu.org/mailman/listinfo/discuss-gnuradio) to 
the mailing list and post your new questions there.


## How to Build GNU Radio

### PyBOMBS
PyBOMBS (Python Build Overlay Managed Bundle System) is the recommended 
method for building and installing GNU Radio. Please see 
https://github.com/gnuradio/pybombs for detailed instructions. Abbreviated 
instructions are duplicated below.

1. Install PyBOMBS:
    ```
    $ [sudo] pip install PyBOMBS
    ```
    or
    ```
    $ git clone https://github.com/gnuradio/pybombs.git
    $ cd pybombs
    $ sudo python setup.py install
    ```

2. Add PyBOMBS recipes:
    ```
    $ pybombs recipes add gr-recipes git+https://github.com/gnuradio/gr-recipes.git  
    $ pybombs recipes add gr-etcetera git+https://github.com/gnuradio/gr-etcetera.git
    ```

3. Configure an installation prefix:
    ```
    $ pybombs prefix init ~/prefix/default/
    ```

4. Install GNU Radio:
    ```
    $ pybombs install gnuradio
    ```

5. Run GNU Radio Companion from your new prefix:
    ```
    $ source ~/prefix/default/setup_env.sh
    $ gnuradio-companion
    ```
    or execute it without changing the current environment
    ```
    $ pybombs run gnuradio-companion
    ```

### Manual Source Build
Complete build instructions are detailed in the 
[GNU Radio Build Guide](https://gnuradio.org/doc/doxygen/build_guide.html). 
Abbreviated instructions are duplicated below.

1. Ensure that you have satisfied the external dependencies, see 
[GNU Radio Dependencies](https://gnuradio.org/doc/doxygen/build_guide.html).

2. Checkout the latest code:
    ```
    $ git clone https://github.com/gnuradio/gnuradio.git
    ```

3. Build with CMake:
    ```
    $ cd gnuradio
    $ mkdir build
    $ cd build
    $ cmake [OPTIONS] ../
    $ make
    $ make test
    $ sudo make install
    ```
    Useful `[OPTIONS]` include setting the install prefix 
    `-DCMAKE_INSTALL_PREFIX=<directory to install to>` and the build type 
    `-DCMAKE_BUILD_TYPE=<type>`. Currently, GNU Radio has a `"Debug"` type 
    that builds with `-g -O2` which is useful for debugging the software, 
    and a `"Release"` type that builds with `-O3`, which is the default.


## Legal Matters

Some files have been changed many times throughout the years. Copyright 
notices at the top of source files list which years changes have been 
made. For some files, changes have occurred in many consecutive years. 
These files may often have the format of a year range (e.g., "2006 - 2011"), 
which indicates that these files have had copyrightable changes made 
during each year in the range, inclusive.
