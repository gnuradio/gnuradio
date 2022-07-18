
import os

try:
    from .analog_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .analog_python import *


# from .fm_deemph_hier import *