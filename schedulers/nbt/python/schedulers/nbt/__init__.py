
import os

try:
    from .scheduler_nbt_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .scheduler_nbt_python import *
