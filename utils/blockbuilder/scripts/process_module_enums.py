from jinja2 import Template, FileSystemLoader, DictLoader, Environment
import os
import argparse
import yaml
import shutil

def argParse():
    """Parses commandline args."""
    desc=''
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("--module")
    parser.add_argument("--yaml_file")
    parser.add_argument("--output_h")
    parser.add_argument("--output_pybind")
    parser.add_argument("--build_dir")

    return parser.parse_args()

def main():
    args = argParse()

    # env = Environment(loader = FileSystemLoader(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','templates'))
    paths = []
    paths.append(os.path.join(os.path.dirname(os.path.realpath(__file__)),'..','templates'))
    paths.append(os.path.dirname(os.path.realpath(args.yaml_file)))
    env = Environment(loader = FileSystemLoader(paths))

    with open(args.yaml_file) as file:
        d = yaml.load(file, Loader=yaml.FullLoader)

        enums_h = os.path.join(args.build_dir, 'blocklib', args.module, os.path.basename(args.output_h))
        enums_h_includedir = os.path.join(args.build_dir, 'blocklib', args.module, 'include', 'gnuradio', args.module, os.path.basename(args.output_h))
        template = env.get_template('module_enums.h.j2')

        rendered = template.render(enums=d, module=args.module)
        with open(enums_h, 'w') as file:
            print("generating " + enums_h)
            file.write(rendered)

        # Copy to the include dir
        shutil.copyfile(enums_h, enums_h_includedir)                

        if args.output_pybind:
            filename = os.path.join(args.build_dir, 'blocklib', args.module, os.path.basename(args.output_pybind))
            template = env.get_template('module_enums_pybind.cc.j2')

            rendered = template.render(enums=d, module=args.module)
            with open(filename, 'w') as file:
                print("generating " + filename)
                file.write(rendered)

if __name__ == "__main__":
    main()
