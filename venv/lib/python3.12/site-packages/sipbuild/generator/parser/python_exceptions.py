# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


# The builtin Python exceptions across all supported versions of Python.
PYTHON_EXCEPTIONS = (
    'BaseException',
    'Exception',
    'StopIteration',
    'GeneratorExit',
    'ArithmeticError',
    'LookupError',
    'StandardError',                # Python v2.

    'AssertionError',
    'AttributeError',
    'BufferError',
    'EOFError',
    'FloatingPointError',
    'OSError',
    'ImportError',
    'IndexError',
    'KeyError',
    'KeyboardInterrupt',
    'MemoryError',
    'NameError',
    'OverflowError',
    'RuntimeError',
    'NotImplementedError',
    'SyntaxError',
    'IndentationError',
    'TabError',
    'ReferenceError',
    'SystemError',
    'SystemExit',
    'TypeError',
    'UnboundLocalError',
    'UnicodeError',
    'UnicodeEncodeError',
    'UnicodeDecodeError',
    'UnicodeTranslateError',
    'ValueError',
    'ZeroDivisionError',
    'EnvironmentError',             # Python v2.
    'IOError',                      # Python v2.
    'WindowsError',                 # Python v2.
    'VMSError',                     # Python v2.

    'BlockingIOError',
    'BrokenPipeError',
    'ChildProcessError',
    'ConnectionError',
    'ConnectionAbortedError',
    'ConnectionRefusedError',
    'ConnectionResetError',
    'FileExistsError',
    'FileNotFoundError',
    'InterruptedError',
    'IsADirectoryError',
    'NotADirectoryError',
    'PermissionError',
    'ProcessLookupError',
    'TimeoutError',

    'Warning',
    'UserWarning',
    'DeprecationWarning',
    'PendingDeprecationWarning',
    'SyntaxWarning',
    'RuntimeWarning',
    'FutureWarning',
    'ImportWarning',
    'UnicodeWarning',
    'BytesWarning',
    'ResourceWarning',
)
