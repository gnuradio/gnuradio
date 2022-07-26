# Custom filters for Jinja2 templates

# cpp, python, grc 
type_lookup = {
    'cf64': ['std::complex<double>', 'complex', 'complex'],
    'cf32': ['std::complex<float>', 'complex', 'complex'],
    'rf64': ['double', 'float', 'real'],
    'rf32': ['float', 'float', 'real'],
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
}

def is_list(value):
    return isinstance(value, list)

# Parses on '/' and returns the second part
def get_linked_value(value):
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
    
    if (vec):
        return f'std::vector<{x}>'
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
        'py_type': py_type,
        'get_linked_value': get_linked_value,
    }