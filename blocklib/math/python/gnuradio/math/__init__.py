
import os
import sys

try:
    from .math_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .math_python import *

# sys.path.extend([f'./{name}' for name in os.listdir(".") if os.path.isdir(name)])
from . import numpy