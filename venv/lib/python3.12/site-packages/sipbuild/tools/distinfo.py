# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ..argument_parser import ArgumentParser
from ..distinfo import distinfo
from ..exceptions import handle_exception


def main():
    """ Create and populate a .dist-info directory. """

    # Parse the command line.
    parser = ArgumentParser("Create and populate a .dist-info directory.")

    parser.add_argument('--console-script', dest='console_scripts',
            action='append', help="the entry point of a console script",
            metavar='ENTRY-POINT')

    parser.add_argument('--generator',
            help="the name of the program generating the directory",
            metavar="NAME")

    parser.add_argument('--generator-version',
            help="the version of the program generating the directory",
            metavar="VERSION")

    parser.add_argument('--gui-script', dest='gui_scripts', action='append',
            help="the entry point of a GUI script", metavar='ENTRY-POINT')

    parser.add_argument('--inventory', required=True,
            help="the file containing the names of the files in the project",
            metavar="FILE")

    parser.add_argument('--metadata', dest='metadata_overrides',
            action='append',
            help="a name/value to override any pyproject.toml metadata",
            metavar='NAME[=VALUE]')

    parser.add_argument('--prefix', help="the installation prefix directory",
            metavar="DIR")

    parser.add_argument('--project-root', required=True,
            help="the directory containing pyproject.toml", metavar="DIR")

    parser.add_argument('--requires-dist', dest='requires_dists',
            action='append', help="additional Requires-Dist", metavar="EXPR")

    parser.add_argument('--wheel-tag',
            help="the tag if a wheel is being created", metavar="TAG")

    parser.add_argument(dest='names', nargs=1,
            help="the name of the .dist-info directory", metavar='directory')

    args = parser.parse_args()

    try:
        distinfo(name=args.names[0], console_scripts=args.console_scripts,
                gui_scripts=args.gui_scripts, generator=args.generator,
                generator_version=args.generator_version,
                inventory=args.inventory,
                metadata_overrides=args.metadata_overrides, prefix=args.prefix,
                project_root=args.project_root,
                requires_dists=args.requires_dists, wheel_tag=args.wheel_tag)
    except Exception as e:
        handle_exception(e)

    return 0


if __name__ == '__main__':
    import sys

    sys.exit(main())
