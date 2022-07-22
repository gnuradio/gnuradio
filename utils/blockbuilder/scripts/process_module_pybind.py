from jinja2 import Template, FileSystemLoader, DictLoader, Environment
import os
import argparse


def argParse():
    """Parses commandline args."""
    desc='Scrape the doxygen generated xml for docstrings to insert into python bindings'
    parser = argparse.ArgumentParser(description=desc)
    
    parser.add_argument("--blocks", nargs="+")
    parser.add_argument("--imports", nargs="+")
    parser.add_argument("--module")
    parser.add_argument("--output_file")
    parser.add_argument("--build_dir")

    return parser.parse_args()

def main():
    args = argParse()
    # env = Environment(loader = FileSystemLoader(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','templates'))

    paths = []
    paths.append(os.path.join(os.path.dirname(os.path.realpath(__file__)),'..','templates'))
    # paths.append(os.path.dirname(os.path.realpath(args.yaml_file)))
    env = Environment(loader = FileSystemLoader(paths))

    d = {
        "blocks": args.blocks,
        "imports": args.imports,
        "module": args.module
    }

    template = env.get_template('python_bindings.cc.j2')

    rendered = template.render(d)
    with open(args.output_file, 'w') as file:
        print("generating " + args.output_file)
        file.write(rendered)

if __name__ == "__main__":
    main()
