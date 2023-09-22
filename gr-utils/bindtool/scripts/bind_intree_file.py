import warnings
import argparse
from gnuradio.bindtool import BindingGenerator
import sys
import tempfile

parser = argparse.ArgumentParser(description='Bind a GR In-Tree Module')
parser.add_argument('--module', type=str,
                    help='Name of gr module containing file to bind (e.g. fft digital analog)')

parser.add_argument('--output_dir', default=tempfile.gettempdir(),
                    help='Output directory of generated bindings')
parser.add_argument('--prefix', help='Prefix of Installed GNU Radio')

parser.add_argument(
    '--filename', help="File to be parsed")

parser.add_argument(
    '--include', help='Additional Include Dirs, comma separated', default=(), nargs='*')

parser.add_argument(
    '--status', help='Location of output file for general status (used during cmake)', default=None
)
parser.add_argument(
    '--flag_automatic', default='0'
)
parser.add_argument(
    '--flag_pygccxml', default='0'
)

args = parser.parse_args()

prefix = args.prefix
output_dir = args.output_dir
includes = args.include
name = args.module

if name not in ['gr', 'pmt']:
    namespace = ['gr', name]
    prefix_include_root = 'gnuradio/' + name  # pmt, gnuradio/digital, etc.
else:
    namespace = [name]
    if name == 'gr':
        prefix_include_root = 'gnuradio'
    elif name == 'pmt':
        prefix_include_root = 'pmt'

with warnings.catch_warnings():
    warnings.filterwarnings("ignore", category=DeprecationWarning)

    bg = BindingGenerator(args.prefix, namespace,
                          prefix_include_root, output_dir, addl_includes=','.join(args.include), catch_exceptions=False, write_json_output=False, status_output=args.status,
                          flag_automatic=True if args.flag_automatic.lower() in [
                              '1', 'true'] else False,
                          flag_pygccxml=True if args.flag_pygccxml.lower() in ['1', 'true'] else False)
    bg.gen_file_binding(args.filename)
