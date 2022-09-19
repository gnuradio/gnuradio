"""
GNU Radio is a free & open-source software development toolkit that provides signal processing blocks to implement software radios. It can be used with readily-available low-cost external RF hardware to create software-defined radios, or without hardware in a simulation-like environment. It is widely used in hobbyist, academic and commercial environments to support both wireless communications research and real-world radio systems.

GNU Radio applications are primarily written using the Python programming language, while the supplied performance-critical signal-processing path is implemented in C++ using processor floating-point extensions, where available. Thus, the developer is able to implement real-time, high-throughput radio systems in a simple-to-use, rapid-application-development environment.

While not primarily a simulation tool, GNU Radio does support development of signal processing algorithms using pre-recorded or generated data, avoiding the need for actual RF hardware.

GNU Radio is licensed under the GNU General Public License (GPL) version 3. All of the code is copyright of the Free Software Foundation.
"""

# This file makes gnuradio a package
# The docstring will be associated with the top level of the package.

import os

from pkgutil import extend_path

# Allow OOT modules to be installed elsewhere on the PYTHONPATH
# For example, gnuradio might be installed to /usr/lib/python3.8/site-packages/gnuradio
#  But an OOT might be installed to /some/other/directory/gnuradio/myoot
#  As long as /some/other/directory/ is in PYTHONPATH, this init file should find it
#  and `from gnuradio import myoot` will work as expected
__path__ = extend_path(__path__, __name__)

# python3.8 and up need to have the dll search path set
# https://docs.python.org/3/whatsnew/3.8.html#bpo-36085-whatsnew
if os.name == 'nt' and hasattr(os, 'add_dll_directory'):
    root_dir = __file__
    for i in range(5):  # limit search depth
        root_dir = os.path.dirname(root_dir)
        bin_dir = os.path.join(root_dir, 'bin')
        if os.path.exists(bin_dir):
            try:
                os.add_dll_directory(bin_dir)
            except Exception as ex:
                print('add_dll_directory(%s): %s' % (bin_dir, ex))
            break

# Check if the gnuradio package is installed or whether we're attempting to import it from
# the build directory.
path_ending = os.path.join('python', 'gnuradio', '__init__.py')
path = os.path.abspath(__file__)
if path.endswith('.pyc'):
    path = path[:-1]

if path.endswith(path_ending):
    # We importing it from build directory.
    build_path = os.path.join(path[:-len(path_ending)])

    # Place these directories on __path__ so that their contents are
    # part of the gnuradio package.
    __path__.append(os.path.join(build_path, 'gr', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'schedulers', 'nbt', 'python', 'gnuradio'))

    ###KERNEL###
    __path__.append(os.path.join(build_path, 'kernel', 'analog', 'python'))
    __path__.append(os.path.join(build_path, 'kernel', 'digital', 'python'))
    __path__.append(os.path.join(build_path, 'kernel', 'fft', 'python'))
    __path__.append(os.path.join(build_path, 'kernel', 'filter', 'python'))
    __path__.append(os.path.join(build_path, 'kernel', 'math', 'python'))
    

    ###BLOCKLIB###
    __path__.append(os.path.join(build_path, 'blocklib', 'analog', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'blocks', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'digital', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'fec', 'python', 'gnuradio'))    
    __path__.append(os.path.join(build_path, 'blocklib', 'fft', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'fileio', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'filter', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'math', 'python', 'gnuradio'))   
    __path__.append(os.path.join(build_path, 'blocklib', 'qtgui', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'soapy', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'streamops', 'python', 'gnuradio'))
    __path__.append(os.path.join(build_path, 'blocklib', 'zeromq', 'python', 'gnuradio'))
