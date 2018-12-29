

from argparse import Namespace
from os import path
import tempfile
import subprocess

from .test_compiler import *

def test_cpp():
    test_compiler('test_cpp.grc')
    if not os.path.exists('test_cpp/build'):
        os.makedirs('directory')
    return_code = subprocess.Popen('cmake ..', cwd='./build/')
    assert(return_code == 0)
    return_code = subprocess.Popen('make', cwd='./build/')
    assert(return_code == 0)

