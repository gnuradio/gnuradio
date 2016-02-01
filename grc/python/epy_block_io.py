
import inspect
import collections

from gnuradio import gr
import pmt


TYPE_MAP = {
    'complex64': 'complex', 'complex': 'complex',
    'float32': 'float', 'float': 'float',
    'int32': 'int', 'uint32': 'int',
    'int16': 'short', 'uint16': 'short',
    'int8': 'byte', 'uint8': 'byte',
}

BlockIO = collections.namedtuple('BlockIO', 'name cls params sinks sources doc')


def _ports(sigs, msgs):
    ports = list()
    for i, dtype in enumerate(sigs):
        port_type = TYPE_MAP.get(dtype.name, None)
        if not port_type:
            raise ValueError("Can't map {0:!r} to GRC port type".format(dtype))
        ports.append((str(i), port_type))
    for msg_key in msgs:
        if msg_key == 'system':
            continue
        ports.append((msg_key, 'message'))
    return ports


def _blk_class(source_code):
    ns = {}
    try:
        exec source_code in ns
    except Exception as e:
        raise ValueError("Can't interpret source code: " + str(e))
    for var in ns.itervalues():
        if inspect.isclass(var)and issubclass(var, gr.gateway.gateway_block):
            break
    else:
        raise ValueError('No python block class found in code')
    return var


def extract(cls):
    if not inspect.isclass(cls):
        cls = _blk_class(cls)

    spec = inspect.getargspec(cls.__init__)
    defaults = map(repr, spec.defaults or ())
    doc = cls.__doc__ or cls.__init__.__doc__ or ''
    cls_name = cls.__name__

    if len(defaults) + 1 != len(spec.args):
        raise ValueError("Need all default values")

    try:
        instance = cls()
    except Exception as e:
        raise RuntimeError("Can't create an instance of your block: " + str(e))

    name = instance.name()
    params = list(zip(spec.args[1:], defaults))

    sinks = _ports(instance.in_sig(),
                      pmt.to_python(instance.message_ports_in()))
    sources = _ports(instance.out_sig(),
                        pmt.to_python(instance.message_ports_out()))

    return BlockIO(name, cls_name, params, sinks, sources, doc)


if __name__ == '__main__':
    blk_code = """
import numpy as np
from gnuradio import gr
import pmt

class blk(gr.sync_block):
    def __init__(self, param1=None, param2=None):
        "Test Docu"
        gr.sync_block.__init__(
            self,
            name='Embedded Python Block',
            in_sig = (np.float32,),
            out_sig = (np.float32,np.complex64,),
        )
        self.message_port_register_in(pmt.intern('msg_in'))
        self.message_port_register_out(pmt.intern('msg_out'))

    def work(self, inputs_items, output_items):
        return 10
    """
    print extract(blk_code)
