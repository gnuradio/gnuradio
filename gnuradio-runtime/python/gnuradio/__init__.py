"""
GNU Radio is a free & open-source software development toolkit that provides signal processing blocks to implement software radios. It can be used with readily-available low-cost external RF hardware to create software-defined radios, or without hardware in a simulation-like environment. It is widely used in hobbyist, academic and commercial environments to support both wireless communications research and real-world radio systems.

GNU Radio applications are primarily written using the Python programming language, while the supplied performance-critical signal-processing path is implemented in C++ using processor floating-point extensions, where available. Thus, the developer is able to implement real-time, high-throughput radio systems in a simple-to-use, rapid-application-development environment.

While not primarily a simulation tool, GNU Radio does support development of signal processing algorithms using pre-recorded or generated data, avoiding the need for actual RF hardware.

GNU Radio is licensed under the GNU General Public License (GPL) version 3. All of the code is copyright of the Free Software Foundation.
"""

# This file makes gnuradio a package
# The docstring will be associated with the top level of the package.

import os

# Check if the gnuradio package is installed or whether we're attempting to import it from
# the build directory.
path_ending = os.path.join('gnuradio-runtime', 'python', 'gnuradio', '__init__.py')
path = os.path.abspath(__file__)
if path.endswith('.pyc'):
    path = path[:-1]

if path.endswith(path_ending):
    # We importing it from build directory.
    build_path = os.path.join(path[:-len(path_ending)])

    # Place these directories on __path__ so that their contents are
    # part of the gnuradio package.
    __path__.append(os.path.join(build_path, 'gr-utils', 'src', 'python'))
    __path__.append(os.path.join(build_path, 'gr-blocks', 'python'))
    __path__.append(os.path.join(build_path, 'gr-digital', 'python'))
    __path__.append(os.path.join(build_path, 'gr-filter', 'python'))
    __path__.append(os.path.join(build_path, 'gr-fft', 'python'))
    __path__.append(os.path.join(build_path, 'gr-analog', 'python'))
    __path__.append(os.path.join(build_path, 'gr-dtv', 'python'))
    __path__.append(os.path.join(build_path, 'gr-trellis', 'python'))
    __path__.append(os.path.join(build_path, 'gr-wavelet', 'python'))
    __path__.append(os.path.join(build_path, 'gr-audio', 'python'))
    __path__.append(os.path.join(build_path, 'gr-qtgui', 'python'))
    __path__.append(os.path.join(build_path, 'gr-wxgui', 'python'))
    __path__.append(os.path.join(build_path, 'gr-atsc', 'python'))
    __path__.append(os.path.join(build_path, 'gr-noaa', 'python'))
    __path__.append(os.path.join(build_path, 'gr-pager', 'python'))
    __path__.append(os.path.join(build_path, 'gr-video-sdl', 'python'))
    __path__.append(os.path.join(build_path, 'gr-vocoder', 'python'))
    __path__.append(os.path.join(build_path, 'gr-fcd', 'python'))
    __path__.append(os.path.join(build_path, 'gr-comedi', 'python'))
    __path__.append(os.path.join(build_path, 'gr-channels', 'python'))
    __path__.append(os.path.join(build_path, 'gr-fec', 'python'))
    __path__.append(os.path.join(build_path, 'gr-utils', 'python'))
    __path__.append(os.path.join(build_path, 'gr-uhd', 'python'))
    __path__.append(os.path.join(build_path, 'gr-zeromq', 'python'))
