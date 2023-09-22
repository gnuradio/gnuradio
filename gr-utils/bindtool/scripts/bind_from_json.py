import argparse
import os
from gnuradio.bindtool import BindingGenerator
import pathlib

parser = argparse.ArgumentParser(
    description='Bind a GR header file from the generated json')
parser.add_argument('pathnames', type=str, nargs='+',
                    help='Json files to bind')

args = parser.parse_args()

bg = BindingGenerator()

for p in args.pathnames:
    bg.bind_from_json(p)
