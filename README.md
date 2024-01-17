## How to run the Qt version of GRC

- Make sure PyQt5 (including QtSvg and QtWebEngine) is installed.
- `pip install QDarkStyle qtpy`
- `pip install pytest-qt pyautogui` (only required for testing)
- checkout the `feature-grc-qt` branch
- build and install like usual (see instructions on the master branch)
- Run `gnuradio-companion --qt`


<p align="center">
<img src="https://github.com/gnuradio/gnuradio/blob/main/docs/gnuradio.png" width="75%" />
</p>

[![Make Test](https://github.com/gnuradio/gnuradio/actions/workflows/make-test.yml/badge.svg?branch=main)](https://github.com/gnuradio/gnuradio/actions/workflows/make-test.yml)
![Version](https://img.shields.io/github/tag/gnuradio/gnuradio.svg)
[![AUR](https://img.shields.io/github/license/gnuradio/gnuradio)](https://github.com/gnuradio/gnuradio/blob/main/COPYING)
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

The following command is for Debian, Ubuntu, and derivatives. Consult your distribution information to obtain the version of GNU Radio which is included.

    sudo apt install gnuradio

For other operating systems and versions, see [Quick Start](https://wiki.gnuradio.org/index.php/InstallingGR#Quick_Start)

### Ubuntu PPA Installation

For Ubuntu, the latest builds (both released and pulled from master branch) are maintained as PPAs (Personal Package Archives) on [launchpad.net](https://launchpad.net/~gnuradio). **Be sure to uninstall any previously installed versions of gnuradio first.** See [UnInstallGR](https://wiki.gnuradio.org/index.php?title=UnInstallGR#From_Distribution_package_manager_or_PPA).


### Other Installation Methods

Platform-specific guides and Cross-platform guides are described in [Other Installation Methods](https://wiki.gnuradio.org/index.php/InstallingGR#Other_Installation_Methods).

### From Source

Complete instructions for building GNU Radio from source code are detailed in 
[Installing From Source](https://wiki.gnuradio.org/index.php?title=LinuxInstall#From_Source). 

### PyBOMBS

**We are no longer recommending** [PyBOMBS](https://github.com/gnuradio/pybombs#pybombs) **to install modern versions of GNU Radio.**

## Legal Matters

Some files have been changed many times throughout the years. Copyright 
notices at the top of source files list which years changes have been 
made. For some files, changes have occurred in many consecutive years. 
These files may often have the format of a year range (e.g., "2006 - 2011"), 
which indicates that these files have had copyrightable changes made 
during each year in the range, inclusive.
