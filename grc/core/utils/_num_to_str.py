# -*- coding: utf-8 -*-
"""${FILE_NAME}"""
from grc.core import Constants

__author__ = "Sebastian Koslowski"
__email__ = "sebastian.koslowski@gmail.com"
__copyright__ = "Copyright 2016, Sebastian Koslowski"


def num_to_str(num):
    """ Display logic for numbers """
    def eng_notation(value, fmt='g'):
        """Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n"""
        template = '{:' + fmt + '}{}'
        magnitude = abs(value)
        for exp, symbol in zip(range(9, -15-1, -3), 'GMk munpf'):
            factor = 10 ** exp
            if magnitude >= factor:
                return template.format(value / factor, symbol.strip())
        return template.format(value, '')

    if isinstance(num, Constants.COMPLEX_TYPES):
        num = complex(num)  # Cast to python complex
        if num == 0:
            return '0'
        output = eng_notation(num.real) if num.real else ''
        output += eng_notation(num.imag, '+g' if output else 'g') + 'j' if num.imag else ''
        return output
    else:
        return str(num)
