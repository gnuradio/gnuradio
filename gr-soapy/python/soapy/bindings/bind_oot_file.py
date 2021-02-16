import warnings
import argparse
import os
from gnuradio.bindtool import BindingGenerator
import pathlib
import sys

parser = argparse.ArgumentParser(description='Bind a GR Out of Tree Block')
parser.add_argument('--module', type=str,
                    help='Name of gr module containing file to bind (e.g. fft digital analog)')

parser.add_argument('--output_dir', default='/tmp',
                    help='Output directory of generated bindings')
parser.add_argument('--prefix', help='Prefix of Installed GNU Radio')
parser.add_argument('--src', help='Directory of gnuradio source tree',
                    default=os.path.dirname(os.path.abspath(__file__))+'/../../..')

parser.add_argument(
    '--filename', help="File to be parsed")

parser.add_argument(
    '--defines', help='Set additional defines for precompiler',default=(), nargs='*')
parser.add_argument(
    '--include', help='Additional Include Dirs, separated', default=(), nargs='*')

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
defines = tuple(','.join(args.defines).split(','))
includes = ','.join(args.include)
name = args.module

namespace = ['gr', name]
prefix_include_root = name


with warnings.catch_warnings():
    warnings.filterwarnings("ignore", category=DeprecationWarning)

    bg = BindingGenerator(prefix, namespace,
                          prefix_include_root, output_dir, define_symbols=defines, addl_includes=includes,
                          catch_exceptions=False, write_json_output=False, status_output=args.status,
                          flag_automatic=True if args.flag_automatic.lower() in [
                              '1', 'true'] else False,
                          flag_pygccxml=True if args.flag_pygccxml.lower() in ['1', 'true'] else False)
    bg.gen_file_binding(args.filename)
