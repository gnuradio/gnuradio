"""
Customizations of sphinx for gnuradio use.
"""

from sphinx.ext.autodoc import py_ext_sig_re
from sphinx.ext.autodoc import ClassDocumenter, FunctionDocumenter, members_option
from sphinx.ext.autodoc import bool_option, members_set_option, identity
from sphinx.ext.autodoc import ALL

# A dictionary of the number of lines to delete from the beginning of docstrings
lines_to_delete = {}

def setup(sp):
    # Fix line-breaks in signature.
    sp.connect('autodoc-process-signature', fix_signature)
    sp.connect('autodoc-process-docstring', remove_lines)
    # Add node to autodocument signal-processing blocks.
    sp.add_autodocumenter(OldBlockDocumenter)
    sp.add_autodocumenter(BlockDocumenter)
    sp.add_autodocumenter(PyBlockDocumenter)

def remove_lines(app, what, name, obj, options, lines):
    del_lines = lines_to_delete.get(name, 0)
    # Don't delete any lines if this is called again.
    lines_to_delete[name] = 0
    lines[:] = lines[del_lines:]

def fix_signature(app, what, name, obj, options, signature, return_annotation):
    """
    SWIG produces signature at the top of docstrings of the form
    'blah(int arg1, float arg2) -> return_type'
    and if the string is long it breaks it over multiple lines.

    Sphinx gets confused if it is broken over multiple lines.
    fix_signature and remove_lines get around this problem.
    """
    if return_annotation is not None:
        return

    if hasattr(obj, '__doc__'):
        docs = obj.__doc__
    else:
        docs = None
    if not docs:
        return None
    doclines = docs.split('\n')
    del_lines = remove_linebreaks_in_signature(doclines)
    # match first line of docstring against signature RE
    match = py_ext_sig_re.match(doclines[0])
    if not match:
        return None
    exmod, path, base, args, retann = match.groups()
    # ok, now jump over remaining empty lines and set the remaining
    # lines as the new doclines
    i = 1
    while i < len(doclines) and not doclines[i].strip():
        i += 1
    lines_to_delete[name] = i - 1 + del_lines
    # format args
    signature = "({0})".format(args)
    return signature, retann

def remove_linebreaks_in_signature(lines):
    alllines = '\n'.join(lines)
    alllines = alllines.lstrip()
    bits = alllines.split('->')
    if len(bits) == 1:
        return 0
    after = '->'.join(bits[1:])
    after_lines = after.split('\n')
    ending = None
    remainder = []
    for line in after_lines:
        if line and ending is None:
            ending = line
        elif ending is not None:
            remainder.append(line)
    first_line = ' '.join([a.strip() for a in bits[0].split('\n') if a.strip()]) + ' -> ' + ending.strip()
    match = py_ext_sig_re.match(first_line)
    # If it is a signature, make the change to lines.
    if match:
        new_lines = [first_line] + remainder
        lines[:] = new_lines
        return len(bits[0].split('\n'))
    else:
        return 0

# These methods are not displayed in the documentation of blocks to
# avoid redundancy.
common_block_members =[
    'check_topology',
    'detail',
    'history',
    'input_signature',
    'name',
    'nitems_read',
    'nitems_written',
    'nthreads',
    'output_multiple',
    'output_signature',
    'relative_rate',
    'set_detail',
    'set_nthreads',
    'start',
    'stop',
    'thisown',
    'to_basic_block',
    'unique_id',
    'make',
    'alias',
    'is_set_max_noutput_items',
    'max_noutput_items',
    'max_output_buffer',
    'message_ports_in',
    'message_ports_out',
    'min_output_buffer',
    'pc_input_buffers_full',
    'pc_input_buffers_full_var',
    'pc_noutput_items',
    'pc_noutput_items_var',
    'pc_nproduced',
    'pc_nproduced_var',
    'pc_output_buffers_full',
    'pc_output_buffers_full_var',
    'pc_work_time',
    'pc_work_time_var',
    'processor_affinity',
    'set_block_alias',
    'set_max_noutput_items',
    'unset_max_noutput_items',
    'set_max_output_buffer',
    'set_min_output_buffer',
    'set_processor_affinity',
    'symbol_name',
    'unset_processor_affinity',
    'disconnect_all',
    'index',
    'length',
    'lock',
    'primitive_connect',
    'primitive_disconnect',
    'primitive_message_port_register_hier_in',
    'primitive_message_port_register_hier_out',
    'primitive_msg_connect',
    'primitive_msg_disconnect',
    'to_hier_block2',
    'type',
    'unlock',
]

class OldBlockDocumenter(FunctionDocumenter):
    """
    Specialized Documenter subclass for gnuradio blocks.

    It merges together the documentation for the generator function (e.g. blocks.head)
    with the wrapped sptr (e.g. gr::blocks::head::sptr) to keep the documentation
    tidier.
    """
    objtype = 'oldblock'
    directivetype = 'function'
    # Don't want to use this for generic functions for give low priority.
    priority = -10

    def __init__(self, *args, **kwargs):
        super(OldBlockDocumenter, self).__init__(*args, **kwargs)
        # Get class name
        bits = self.name.split('.')
        if len(bits) != 3 or bits[0] != 'gnuradio':
            raise ValueError("expected name to be of form gnuradio.x.y but it is {0}".format(self.name))
        sptr_name = 'gnuradio.{0}.{0}_{1}_sptr'.format(bits[1], bits[2])
        # Create a Class Documenter to create documentation for the classes members.
        self.classdoccer = ClassDocumenter(self.directive, sptr_name, indent=self.content_indent)
        self.classdoccer.doc_as_attr = False
        self.classdoccer.real_modname = self.classdoccer.get_real_modname()
        self.classdoccer.options.members = ALL
        self.classdoccer.options.exclude_members = common_block_members
        self.classdoccer.parse_name()
        self.classdoccer.import_object()

    def document_members(self, *args, **kwargs):
        return self.classdoccer.document_members(*args, **kwargs)

class BlockDocumenter(FunctionDocumenter):
    """
    Specialized Documenter subclass for new style gnuradio blocks.

    It merges together the documentation for the generator function (e.g. wavelet.squash_ff)
    with the wrapped sptr (e.g. wavelet.squash_ff_sptr) to keep the documentation
    tidier.
    """
    objtype = 'block'
    directivetype = 'function'
    # Don't want to use this for generic functions for give low priority.
    priority = -10

    def __init__(self, *args, **kwargs):
        super(BlockDocumenter, self).__init__(*args, **kwargs)
        # Get class name
        sptr_name = self.name + '_sptr'
        # Create a Class Documenter to create documentation for the classes members.
        self.classdoccer = ClassDocumenter(self.directive, sptr_name, indent=self.content_indent)
        self.classdoccer.doc_as_attr = False
        self.classdoccer.real_modname = self.classdoccer.get_real_modname()
        self.classdoccer.options.members = ALL
        self.classdoccer.options.exclude_members = common_block_members
        self.classdoccer.parse_name()
        self.classdoccer.import_object()

    def document_members(self, *args, **kwargs):
        return self.classdoccer.document_members(*args, **kwargs)

class PyBlockDocumenter(ClassDocumenter):
    """
    Specialized Documenter subclass for hierarchical python gnuradio blocks.
    """
    objtype = 'pyblock'
    directivetype = 'class'

    def __init__(self, *args, **kwargs):
        super(PyBlockDocumenter, self).__init__(*args, **kwargs)
        self.options.members = ALL
        self.options.exclude_members = common_block_members
