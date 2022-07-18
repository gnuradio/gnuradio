from jinja2 import Template, FileSystemLoader, DictLoader, Environment
import os
import yaml
import argparse
import shutil

def is_list(value):
    return isinstance(value, list)

def argParse():
    """Parses commandline args."""
    desc='Scrape the doxygen generated xml for docstrings to insert into python bindings'
    parser = argparse.ArgumentParser(description=desc)
    
    parser.add_argument("--yaml_file")
    parser.add_argument("--output-cc")
    parser.add_argument("--output-hh")
    parser.add_argument("--build_dir")

    return parser.parse_args()

def main():
    args = argParse()
    # env = Environment(loader = FileSystemLoader(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','templates'))

    # Set FileSystemLoader path to be dir of yml file and dir of this script

    blockdir = os.path.dirname(os.path.realpath(args.yaml_file))
    # print("blockdir is " + blockdir)


    paths = []
    paths.append(os.path.join(os.path.dirname(os.path.realpath(__file__)),'..','templates'))
    paths.append(os.path.dirname(os.path.realpath(args.yaml_file)))
    env = Environment(loader = FileSystemLoader(paths))
    env.filters['is_list'] = is_list
    
    blockname = os.path.basename(os.path.dirname(os.path.realpath(args.yaml_file)))
    

    with open(args.yaml_file) as file:
        d = yaml.load(file, Loader=yaml.FullLoader)
        # Does this block specify a templated version
        templated = 0
        if ('typekeys' in d and len(d['typekeys']) > 0):
            templated = len(d['typekeys'])

        if (args.output_hh ):
            blockname_h = os.path.join(args.build_dir, 'blocklib', d['module'], blockname, os.path.basename(args.output_hh))
            if templated >= 1:
                template = env.get_template('blockname_pyshell_templated.h.j2')
            else:
                template = env.get_template('blockname_pyshell.h.j2')

            rendered = template.render(d)
            with open(blockname_h, 'w') as file:
                print("generating " + blockname_h)
                file.write(rendered)

        if args.output_cc:
            blockname_cc = os.path.join(args.build_dir, 'blocklib', d['module'], blockname, os.path.basename(args.output_cc))
            template = env.get_template('blockname_pyshell.cc.j2')
            rendered = template.render(d)
            with open(blockname_cc, 'w') as file:
                print("generating " + blockname_cc)
                file.write(rendered)


if __name__ == "__main__":
    main()
