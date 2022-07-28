# Custom filters for Jinja2 templates

# cpp, python, grc 
type_lookup = {
    'cf64': ['std::complex<double>', 'complex', 'complex'],
    'cf32': ['std::complex<float>', 'complex', 'complex'],
    'rf64': ['double', 'float', 'float'],
    'rf32': ['float', 'float', 'float'],
    'ri64': ['int64_t', 'int', 'int'],
    'ri32': ['int32_t', 'int', 'int'],
    'ri16': ['int16_t', 'int', 'short'],
    'ri8': ['int8_t', 'int', 'byte'],
    'ru64': ['uint64_t', 'int', 'int'],
    'ru32': ['uint32_t', 'int', 'int'],
    'ru16': ['uint16_t', 'int', 'short'],
    'ru8': ['uint8_t', 'int', 'byte'],
    'size': ['size_t', 'int', 'int'],
    'string': ['std::string', 'str', 'string'],
    'bool': ['bool', 'bool', 'bool'],
    'untyped': ['','','untyped']
}

def is_list(value):
    return isinstance(value, list)

def is_numeric(value):
    return type(value) is int or type(value) is float or type(value) is complex

# Parses on '/' and returns the second part
def get_linked_value(value):
    newvalue = value
    if '/' in value:
        list1 = value.split('/')
        newvalue = list1[1]
    return newvalue

def get_linked_value_with_args(value):
    newvalue = value
    if '/' in value:
        list1 = value.split('/')
        category = list1[0]
        newvalue = list1[1]
        if category == 'parameters':
            newvalue = 'args.' + newvalue
    return newvalue

def cpp_type(input, vec=False):
    if is_list(input):
        input = input[0]
    if input in type_lookup:
        x = type_lookup[input][0]
    else:
        x = get_linked_value(input)
        if input.startswith('parameters/'):
            x = 'args.' + x

    if (vec):
        return f'std::vector<{x}>'
    return x

def grc_type(input, vec=False, ref=False):
    if is_list(input):
        input = input[0]
    try:
        if is_numeric(eval(input)):
            return input
    except:
        pass
    
    if input.startswith('enums/'):
        return 'enum'
    
    if input in type_lookup:
        x = type_lookup[input][2]
    else:
        x = get_linked_value(input)
        if (ref):
            x = f'${{{x}}}'
        
    if (vec):
        return f'{x}_vector'

    return x

def py_type(input):
    if input in type_lookup:
        return type_lookup[input][1]
    else:
        return input

def custom_filters():
    return {
        'is_list': is_list,
        'cpp_type': cpp_type,
        'grc_type': grc_type,
        'py_type': py_type,
        'get_linked_value': get_linked_value,
        'get_linked_value_with_args': get_linked_value_with_args,
    }
