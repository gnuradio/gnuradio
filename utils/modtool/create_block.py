# Python script to create new block within a module

import argparse
import fnmatch
import os
import shutil
import sys
from datetime import datetime
from pathlib import Path

# Argument Parser Setup
parser = argparse.ArgumentParser(description='Create a new block within specified module using newblock template')
parser.add_argument('create_block', metavar='block_name', type=str, help='name of new block')
parser.add_argument('--nocpu', action='store_true', help='indicate whether cpu files will NOT be generated')
parser.add_argument('--cuda', action='store_true', help='indicate whether cuda files will be generated')
parser.add_argument('--templated', action='store_true', help='indicate whether files will be templated')
parser.add_argument('--author', default="Block Author", help="Author of the block")
parser.add_argument('--intree', action='store_true', help='indicate whether block is in tree in which case, call the script from the top src level')

args = parser.parse_args()
block_name = args.create_block # name of new block to be created
# mod_name = args.mod_name # name of module where new block will be created
# Infer the mod name from the current directory


nocpu_arg = args.nocpu # boolean for cpu files
cuda_arg = args.cuda # boolean for cuda files
templated_arg = args.templated # boolean for templated

# get current working directory
current_dir_name = os.getcwd()
mod_name = os.path.basename(current_dir_name)
if mod_name.startswith('ns-') or mod_name.startswith('gr4-'):
    mod_name = current_dir_name.split('-')[1]

path_of_this_file = os.path.dirname(os.path.abspath(__file__))

Path(os.path.join(current_dir_name, 'meson.build')).touch()

# # check if module exists, if not return an error
# mod_path = os.path.join(path, mod_name)
# mod_exists = os.path.isdir(mod_path)
# if not mod_exists:
#     sys.exit('Error: module ' + mod_name + ' does not exist')

# copy newblock template
src = os.path.join(path_of_this_file, 'newblock')
if (args.intree):
    dest = os.path.join(block_name)
else:
    dest = os.path.join('blocklib',mod_name, block_name)
new_block_dir = shutil.copytree(src,dest)

# if cpu is not specified, delete all cpu files
if nocpu_arg:
    files = os.listdir(new_block_dir)
    for file in files:
        if 'cpu' in file:
            os.remove(os.path.join(new_block_dir, file))

# if cuda is not specified, delete all cuda files
if not cuda_arg:
    files = os.listdir(new_block_dir)
    for file in files:
        if 'cuda' in file:
            os.remove(os.path.join(new_block_dir, file))

# if templated is specified, remove non-templated files and rename files
# else, remove templated files
if templated_arg:
    files = os.listdir(new_block_dir)
    for file in files:
        if '_t.' not in file:
            if file == 'meson.build' or file == '.gitignore': # leave meson.build as is
                continue
            else:
                os.remove(os.path.join(new_block_dir, file))
    files = os.listdir(new_block_dir)
    for file in files:
        if '_t.' in file:
            s = file
            index = s.index('.')
            s = s[0 : index-2] + s[index :]
            os.rename(os.path.join(new_block_dir, file), os.path.join(new_block_dir, s))
else:
    files = os.listdir(new_block_dir)
    for file in files:
        if '_t.' in file:
            os.remove(os.path.join(new_block_dir, file))

# rename all files from newblock to block_name
files = os.listdir(new_block_dir)
for file in files:
    if 'newblock' in file:
        new_file_name = file.replace('newblock', block_name)
        os.rename(os.path.join(new_block_dir, file), os.path.join(new_block_dir, new_file_name))

# change all occurences of newblock and newmod in files to block_name and mod_name respectively
files = os.listdir(new_block_dir)
for name in files:
    file_path = os.path.join(new_block_dir, name)
    # read original file
    fin = open(file_path, "rt")
    data = fin.read()
    # replace all occurances of newmod with mod_name and newblock with block_name
    data = data.replace('newmod', mod_name)
    data = data.replace('newblock', block_name)
    data = data.replace('<COPYRIGHT_YEAR>', str(datetime.now().year))
    data = data.replace('<COPYRIGHT_AUTHOR>', args.author)
    fin.close()
    # overwrite original file
    fin = open(file_path, "wt")
    fin.write(data)
    fin.close()