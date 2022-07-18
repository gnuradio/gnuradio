# Python script to create new module in GR 4.0

import argparse
import os
import shutil

# Argument Parser Setup
parser = argparse.ArgumentParser(description='Create a new module within blocklib using newmod template')
parser.add_argument('create_mod', metavar='mod_name', type=str, help='create new module given provided module name')

args = parser.parse_args()
mod_name = args.create_mod # name of new module to be created

# get current working directory
path = os.getcwd()

path_of_this_file = os.path.dirname(os.path.abspath(__file__))

# copy newmod template
src = os.path.join(path_of_this_file, 'newmod')
dest = os.path.join(path, 'gr4-' + mod_name)

new_mod_dir = shutil.copytree(src, dest)

# change all occurances of newmod to mod_name in the new module directory

# blocklib dir
old_dir = os.path.join(new_mod_dir, 'blocklib', 'newmod')
new_dir = os.path.join(new_mod_dir, 'blocklib', mod_name)
os.rename(old_dir, new_dir)

# include dir
old_dir = os.path.join(new_mod_dir, 'blocklib', mod_name, 'include', 'gnuradio', 'newmod')
new_dir = os.path.join(new_mod_dir, 'blocklib', mod_name, 'include', 'gnuradio', mod_name)
os.rename(old_dir, new_dir)

# python dir
old_dir = os.path.join(new_mod_dir, 'blocklib', mod_name, 'python', 'newmod')
new_dir = os.path.join(new_mod_dir, 'blocklib', mod_name, 'python', mod_name)
os.rename(old_dir, new_dir)

# change all occurances of newmod in files to mod_name

for path, subdirs, files in os.walk(new_mod_dir):
    for name in files:
        file_path = os.path.join(path, name)
        # read original file
        fin = open(file_path, "rt")
        data = fin.read()
        # replace all occurances of newmod with mod_name
        data = data.replace('newmod', mod_name)
        fin.close()
        # overwrite original file
        fin = open(file_path, "wt")
        fin.write(data)
        fin.close()

# add new module path to gnuradio/python/gnuradio/__init__.py


