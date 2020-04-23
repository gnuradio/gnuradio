from gnuradio.bindtool import BindingGenerator

prefix = '/share/gnuradio/grnext'
output_dir = '/share/tmp/test_pybind'
namespace = ['gr']
module_dir = '/share/gnuradio/grnext/src/gnuradio/gnuradio-runtime/include/gnuradio'
prefix_include_root = 'gnuradio'  #pmt, gnuradio/digital, etc.

prefix = '/share/gnuradio/grnext'
output_dir = '/share/tmp/test_pybind'
namespace = ['gr','digital']
module_dir = '/share/gnuradio/grnext/src/gnuradio/gr-digital/include'
prefix_include_root = 'gnuradio/digital'  #pmt, gnuradio/digital, etc.

prefix = '/share/gnuradio/grnext'
output_dir = '/share/tmp/test_pybind'
namespace = ['gr','fft']
module_dir = '/share/gnuradio/grnext/src/gnuradio/gr-fft/include'
prefix_include_root = 'gnuradio/fft'  #pmt, gnuradio/digital, etc.

import warnings
with warnings.catch_warnings():
    warnings.filterwarnings("ignore", category=DeprecationWarning)
    bg = BindingGenerator(prefix, namespace, prefix_include_root, output_dir)
    bg.gen_bindings(module_dir)