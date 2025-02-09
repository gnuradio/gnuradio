# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ..argument_parser import ArgumentParser
from ..exceptions import handle_exception
from ..module import module


def main():
    """ Create the source, interface and documentation of a sip module. """

    # Parse the command line.
    parser = ArgumentParser("Generate a sip extension module.")

    parser.add_argument('--abi-version', help="the ABI version",
            metavar="MAJOR[.MINOR]")

    parser.add_argument('--project', help="the PyPI project name",
            metavar="NAME")

    parser.add_argument('--sdist', action='store_true', default=False,
            help="generate an sdist file")

    parser.add_argument('--setup-cfg',
            help="the name of the setup.cfg file to use", metavar="FILE")

    parser.add_argument('--sip-h', action='store_true', default=False,
            help="generate a sip.h file")

    parser.add_argument('--sip-rst', action='store_true', default=False,
            help="generate a sip.rst file")

    parser.add_argument('--target-dir', help="generate files in DIR",
            metavar="DIR")

    parser.add_argument(dest='sip_modules', nargs=1,
            help="the fully qualified name of the sip module",
            metavar="module")

    args = parser.parse_args()

    try:
        module(sip_module=args.sip_modules[0], abi_version=args.abi_version,
                project=args.project, sdist=args.sdist,
                setup_cfg=args.setup_cfg, sip_h=args.sip_h,
                sip_rst=args.sip_rst, target_dir=args.target_dir)
    except Exception as e:
        handle_exception(e)

    return 0


if __name__ == '__main__':
    import sys

    sys.exit(main())
