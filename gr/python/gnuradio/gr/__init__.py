
import os

try:
    from .gr_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .gr_python import *

# from .gateway_numpy import block, sync_block #, decim_block, interp_block
from .numpy_helpers import get_input_array, get_output_array

# CUDA specific python code will exception out as these objects aren't compiled in
try:
    buffer_cuda_properties.__init__ = buffer_cuda_properties.make
    from .gateway_cupy import block as cupy_block
    from .gateway_cupy import sync_block as cupy_sync_block
except:
    pass

from .gateway_numpy import block as block
from .gateway_numpy import sync_block as sync_block

# For GR 4.0, connect and msg_connect are the same
graph.msg_connect = graph.connect
# Alias top_block as flowgraph
top_block = flowgraph