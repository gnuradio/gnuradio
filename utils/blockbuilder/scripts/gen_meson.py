# Generate meson.build files for the entire block library

from jinja2 import Template, FileSystemLoader, DictLoader, Environment
import os
import yaml
import argparse
import shutil
import glob

def argParse():
    """Parses commandline args."""
    desc='Scrape the doxygen generated xml for docstrings to insert into python bindings'
    parser = argparse.ArgumentParser(description=desc)
    
    parser.add_argument("blocklib_path")
    parser.add_argument("--clear", action='store_true')

    return parser.parse_args()

def main():
    args = argParse()

    paths = []
    paths.append(os.path.join(os.path.dirname(os.path.realpath(__file__)),'..','templates'))
    env = Environment(loader = FileSystemLoader(paths))


    for root, d_names, f_names in os.walk(args.blocklib_path):       
        yml_files = glob.glob(os.path.join(root,'*.yml'))
        if yml_files:
            existing_meson_filename = os.path.join(root,'meson.build')
            if os.path.exists(existing_meson_filename):
                with open(existing_meson_filename, 'r') as meson_file:
                    if not meson_file.readline().startswith('#autogenerated'):
                        continue

            # If we get to here, there is not a non-auto-generated meson file
            #  so we can overwrite whatever is there
            if args.clear:
                if (os.path.exists(existing_meson_filename)):
                    os.remove(existing_meson_filename)
                    print(f'removing: {root}/{existing_meson_filename}')
                continue

            with open(yml_files[0]) as file:
                d = yaml.load(file, Loader=yaml.FullLoader)
                new_meson_filename = os.path.join(os.path.dirname(yml_files[0]),'meson.build')
                template = env.get_template('blockname.meson.build.j2')
                cufiles = [os.path.basename(x) for x in glob.glob(os.path.join(root,'*.cu'))]
                rendered = template.render(d, cufiles=cufiles)
                with open(new_meson_filename, 'w') as file:
                    print(f'generating: {root}/{new_meson_filename}')
                    file.write(rendered)

    modulesdirs = []
    for x in [x for x in os.listdir(args.blocklib_path)]:
        current_module_path = os.path.join(args.blocklib_path,x)
        if os.path.isdir(current_module_path):
            module_name = os.path.basename(x)
            print(module_name)
            modulesdirs.append(module_name)

            meson_filename = os.path.join(current_module_path,'meson.build')
            pybind_meson_filename = os.path.join(current_module_path,'python',module_name,'meson.build')
            if os.path.exists(meson_filename):
                with open(meson_filename, 'r') as meson_file:
                    if not meson_file.readline().startswith('#autogenerated'):
                        continue

            # If we get to here, there is not a non-auto-generated meson file
            #  so we can overwrite whatever is there
            if args.clear:
                if (os.path.exists(meson_filename)):
                    os.remove(meson_filename)
                    print(f'removing: {meson_filename}')
                if (os.path.exists(pybind_meson_filename)):
                    os.remove(pybind_meson_filename)
                    print(f'removing: {pybind_meson_filename}')
                continue

            # Let's gather up all the block directories in the module and generate the 
            # meson.build for the entire module
            blockdirs = []
            for y in [y for y in os.listdir(current_module_path)]:
                current_block_path = os.path.join(current_module_path, y)
                
                if y not in ['lib','include','test','bench','python','examples'] \
                        and os.path.isdir(current_block_path) \
                        and os.path.exists(os.path.join(current_block_path, os.path.basename(current_block_path) + '.yml')):
                    print('   ' + y)
                    blockdirs.append(os.path.basename(y))

            print(os.path.join(current_module_path,'enums.yml'))
            has_enums = os.path.exists(os.path.join(current_module_path,'enums.yml'))

            template = env.get_template('module.meson.build.j2')
            rendered = template.render(module=module_name, blocks=blockdirs, has_enums=has_enums)
            with open(meson_filename, 'w') as file:
                print(f'generating: {meson_filename}')
                file.write(rendered)

    # Do the python directory
    modulesdirs = []
    for x in [x for x in os.listdir(args.blocklib_path)]:
        current_module_path = os.path.join(args.blocklib_path,x)
        if os.path.isdir(current_module_path):
            module_name = os.path.basename(x)
            print(module_name)
            modulesdirs.append(module_name)

            meson_filename = os.path.join(current_module_path,'python',module_name,'meson.build')
            if os.path.exists(meson_filename):
                with open(meson_filename, 'r') as meson_file:
                    if not meson_file.readline().startswith('#autogenerated'):
                        continue

            # If we get to here, there is not a non-auto-generated meson file
            #  so we can overwrite whatever is there
            if args.clear:
                if (os.path.exists(meson_filename)):
                    os.remove(meson_filename)
                    print(f'removing: {meson_filename}')
                continue
            
            template = env.get_template('module_python.meson.build.j2')
            rendered = template.render(module=module_name)
            with open(meson_filename, 'w') as file:
                print(f'generating: {meson_filename}')
                file.write(rendered)



    return


if __name__ == "__main__":
    main()
