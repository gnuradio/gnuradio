

import inspect
import collections


TYPE_MAP = {
    'complex64': 'complex', 'complex': 'complex',
    'float32': 'float', 'float': 'float',
    'int32': 'int', 'uint32': 'int',
    'int16': 'short', 'uint16': 'short',
    'int8': 'byte', 'uint8': 'byte',
}

BlockIO = collections.namedtuple(
    'BlockIO', 'name cls params sinks sources doc callbacks')


def _ports(sigs, msgs):
    ports = list()
    for i, dtype in enumerate(sigs):
        port_type = TYPE_MAP.get(dtype.base.name, None)
        if not port_type:
            raise ValueError("Can't map {0!r} to GRC port type".format(dtype))
        vlen = dtype.shape[0] if len(dtype.shape) > 0 else 1
        ports.append((str(i), port_type, vlen))
    for msg_key in msgs:
        if msg_key == 'system':
            continue
        ports.append((msg_key, 'message', 1))
    return ports


def _find_block_class(source_code, cls):
    ns = {}
    try:
        exec(source_code, ns)
    except Exception as e:
        raise ValueError("Can't interpret source code: " + str(e))
    for var in ns.values():
        if inspect.isclass(var) and issubclass(var, cls):
            return var
    raise ValueError('No python block class found in code')


def extract(cls):
    try:
        from gnuradio import gr
        import pmt
    except ImportError:
        raise EnvironmentError("Can't import GNU Radio")

    if not inspect.isclass(cls):
        cls = _find_block_class(cls, gr.gateway.gateway_block)

    spec = inspect.getfullargspec(cls.__init__)
    init_args = spec.args[1:]
    defaults = [repr(arg) for arg in (spec.defaults or ())]
    doc = cls.__doc__ or cls.__init__.__doc__ or ''
    cls_name = cls.__name__

    if len(defaults) + 1 != len(spec.args):
        raise ValueError("Need all __init__ arguments to have default values")

    try:
        instance = cls()
    except Exception as e:
        raise RuntimeError("Can't create an instance of your block: " + str(e))

    name = instance.name()

    params = list(zip(init_args, defaults))

    def settable(attr):
        try:
            # check for a property with setter
            return callable(getattr(cls, attr).fset)
        except AttributeError:
            return attr in instance.__dict__  # not dir() - only the instance attribs

    callbacks = [attr for attr in dir(
        instance) if attr in init_args and settable(attr)]

    sinks = _ports(instance.in_sig(),
                   pmt.to_python(instance.message_ports_in()))
    sources = _ports(instance.out_sig(),
                     pmt.to_python(instance.message_ports_out()))

    return BlockIO(name, cls_name, params, sinks, sources, doc, callbacks)


if __name__ == '__main__':
    blk_code = """
import numpy as np
from gnuradio import gr
import pmt

class blk(gr.sync_block):
    def __init__(self, param1=None, param2=None, param3=None):
        "Test Docu"
        gr.sync_block.__init__(
            self,
            name='Embedded Python Block',
            in_sig = (np.float32,),
            out_sig = (np.float32,np.complex64,),
        )
        self.message_port_register_in(pmt.intern('msg_in'))
        self.message_port_register_out(pmt.intern('msg_out'))
        self.param1 = param1
        self._param2 = param2
        self._param3 = param3

    @property
    def param2(self):
        return self._param2

    @property
    def param3(self):
        return self._param3

    @param3.setter
    def param3(self, value):
        self._param3 = value

    def work(self, inputs_items, output_items):
        return 10
    """
    from pprint import pprint
    pprint(dict(extract(blk_code)._asdict()))
