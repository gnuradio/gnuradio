GRAY_CODE = 'gray'
SET_PARTITION_CODE = 'set-partition'
NO_CODE = 'none'

codes = (GRAY_CODE, SET_PARTITION_CODE, NO_CODE)

def invert_code(code):
    c = enumerate(code)
    ic = [(b, a) for (a, b) in c]
    ic.sort()
    return [a for (b, a) in ic]
