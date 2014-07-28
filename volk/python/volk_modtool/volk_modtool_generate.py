#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import os
import glob
import sys
import re
import glob
import shutil
import exceptions
from sets import Set

class volk_modtool:
    def __init__(self, cfg):
        self.volk = re.compile('volk');
        self.remove_after_underscore = re.compile("_.*");
        self.volk_run_tests = re.compile('^\s*VOLK_RUN_TESTS.*\n', re.MULTILINE);
        self.volk_profile = re.compile('^\s*(VOLK_PROFILE|VOLK_PUPPET_PROFILE).*\n', re.MULTILINE);
        self.my_dict = cfg;
        self.lastline = re.compile('\s*char path\[1024\];.*');
        self.badassert = re.compile('^\s*assert\(toked\[0\] == "volk_.*\n', re.MULTILINE);
        self.goodassert = '    assert(toked[0] == "volk");\n'
        self.baderase = re.compile('^\s*toked.erase\(toked.begin\(\)\);.*\n', re.MULTILINE);
        self.gooderase = '    toked.erase(toked.begin());\n    toked.erase(toked.begin());\n';

    def get_basename(self, base=None):
        if not base:
            base = self.my_dict['base']
        candidate = base.split('/')[-1];
        if len(candidate.split('_')) == 1:
            return '';
        else:
            return candidate.split('_')[-1];

    def get_current_kernels(self, base=None):
        if not base:
            base = self.my_dict['base']
            name = self.get_basename();
        else:
            name = self.get_basename(base);
        if name == '':
            hdr_files = glob.glob(os.path.join(base, "kernels/volk/*.h"));
            begins = re.compile("(?<=volk_).*")
        else:
            hdr_files = glob.glob(os.path.join(base, "kernels/volk_" + name + "/*.h"));
            begins = re.compile("(?<=volk_" + name + "_).*")

        datatypes = [];
        functions = [];


        for line in hdr_files:

            subline = re.search(".*\.h.*", os.path.basename(line))
            if subline:
                subsubline = begins.search(subline.group(0));
                if subsubline:
                    dtype = self.remove_after_underscore.sub("", subsubline.group(0));
                    subdtype = re.search("[0-9]+[A-z]+", dtype);
                    if subdtype:
                        datatypes.append(subdtype.group(0));


        datatypes = set(datatypes);

        for line in hdr_files:
            for dt in datatypes:
                if dt in line:
                    #subline = re.search("(?<=volk_)" + dt + ".*(?=\.h)", line);
                    subline = re.search(begins.pattern[:-2] + dt + ".*(?=\.h)", line);
                    if subline:
                        functions.append(subline.group(0));

        return set(functions);

    def make_module_skeleton(self):

        dest = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'])
        if os.path.exists(dest):
            raise exceptions.IOError("Destination %s already exits!"%(dest));

        if not os.path.exists(os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'kernels/volk_' + self.my_dict['name'])):
            os.makedirs(os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'kernels/volk_' + self.my_dict['name']))

        current_kernel_names = self.get_current_kernels();

        for root, dirnames, filenames in os.walk(self.my_dict['base']):
            for name in filenames:
                t_table = map(lambda a: re.search(a, name), current_kernel_names);
                t_table = set(t_table);
                if t_table == set([None]):
                    infile = os.path.join(root, name);
                    instring = open(infile, 'r').read();
                    outstring = re.sub(self.volk, 'volk_' + self.my_dict['name'], instring);
                    newname = re.sub(self.volk, 'volk_' + self.my_dict['name'], name);
                    relpath = os.path.relpath(infile, self.my_dict['base']);
                    newrelpath = re.sub(self.volk, 'volk_' + self.my_dict['name'], relpath);
                    dest = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], os.path.dirname(newrelpath), newname);

                    if not os.path.exists(os.path.dirname(dest)):
                        os.makedirs(os.path.dirname(dest))
                    open(dest, 'w+').write(outstring);


        infile = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'lib/testqa.cc');
        instring = open(infile, 'r').read();
        outstring = re.sub(self.volk_run_tests, '', instring);
        open(infile, 'w+').write(outstring);

        infile = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'apps/volk_' + self.my_dict['name'] + '_profile.cc');
        instring = open(infile, 'r').read();
        outstring = re.sub(self.volk_profile, '', instring);
        open(infile, 'w+').write(outstring);

        infile = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'lib/qa_utils.cc');
        instring = open(infile, 'r').read();
        outstring = re.sub(self.badassert, self.goodassert, instring);
        outstring = re.sub(self.baderase, self.gooderase, outstring);
        open(infile, 'w+').write(outstring);

    def write_default_cfg(self, cfg):
        outfile = open(os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'volk_modtool.cfg'), 'wb');
        cfg.write(outfile);
        outfile.close();


    def convert_kernel(self, oldvolk, name, base, inpath, top):
        infile = os.path.join(inpath, 'kernels/' + top[:-1] + '/' + top + name + '.h');
        instring = open(infile, 'r').read();
        outstring = re.sub(oldvolk, 'volk_' + self.my_dict['name'], instring);
        newname = 'volk_' + self.my_dict['name'] + '_' + name + '.h';
        relpath = os.path.relpath(infile, base);
        newrelpath = re.sub(oldvolk, 'volk_' + self.my_dict['name'], relpath);
        dest = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], os.path.dirname(newrelpath), newname);

        if not os.path.exists(os.path.dirname(dest)):
            os.makedirs(os.path.dirname(dest))
        open(dest, 'w+').write(outstring);

        # copy orc proto-kernels if they exist
        for orcfile in glob.glob(inpath + '/orc/' + top + name + '*.orc'):
            if os.path.isfile(orcfile):
                instring = open(orcfile, 'r').read();
                outstring = re.sub(oldvolk, 'volk_' + self.my_dict['name'], instring);
                newname = 'volk_' + self.my_dict['name'] + '_' + name + '.orc';
                relpath = os.path.relpath(orcfile, base);
                newrelpath = re.sub(oldvolk, 'volk_' + self.my_dict['name'], relpath);
                dest = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], os.path.dirname(newrelpath), newname);
                if not os.path.exists(os.path.dirname(dest)):
                    os.makedirs(os.path.dirname(dest));
                open(dest, 'w+').write(outstring)


    def remove_kernel(self, name):
        basename = self.my_dict['name'];
        if len(basename) > 0:
            top = 'volk_' + basename + '_';
        else:
            top = 'volk_'
        base = os.path.join(self.my_dict['destination'], top[:-1]) ;

        if not name in self.get_current_kernels():

            raise exceptions.IOError("Requested kernel %s is not in module %s"%(name,base));



        inpath = os.path.abspath(base);


        kernel = re.compile(name)
        search_kernels = Set([kernel])
        profile = re.compile('^\s*VOLK_PROFILE')
        puppet = re.compile('^\s*VOLK_PUPPET')
        src_dest = os.path.join(inpath, 'apps/', top[:-1] + '_profile.cc');
        infile = open(src_dest);
        otherlines = infile.readlines();
        open(src_dest, 'w+').write('');

        for otherline in otherlines:
            write_okay = True;
            if kernel.search(otherline):
                write_okay = False;
                if puppet.match(otherline):
                    args = re.search("(?<=VOLK_PUPPET_PROFILE).*", otherline)
                    m_func = args.group(0).split(',')[0];
                    func = re.search('(?<=' + top + ').*', m_func);
                    search_kernels.add(re.compile(func.group(0)));
            if write_okay:
                open(src_dest, 'a').write(otherline);


        src_dest = os.path.join(inpath, 'lib/testqa.cc')
        infile = open(src_dest);
        otherlines = infile.readlines();
        open(src_dest, 'w+').write('');

        for otherline in otherlines:
            write_okay = True;

            for kernel in search_kernels:
                if kernel.search(otherline):
                    write_okay = False;

            if write_okay:
                open(src_dest, 'a').write(otherline);

        for kernel in search_kernels:
            infile = os.path.join(inpath, 'kernels/' + top[:-1] + '/' + top + kernel.pattern + '.h');
            print "Removing kernel %s"%(kernel.pattern)
            if os.path.exists(infile):
                os.remove(infile);
        # remove the orc proto-kernels if they exist. There are no puppets here
        # so just need to glob for files matching kernel name
        print glob.glob(inpath + '/orc/' + top + name + '*.orc');
        for orcfile in glob.glob(inpath + '/orc/' + top + name + '*.orc'):
            print orcfile
            if(os.path.exists(orcfile)):
                os.remove(orcfile);

    def import_kernel(self, name, base):
        if not (base):
            base = self.my_dict['base'];
            basename = self.getbasename();
        else:
            basename = self.get_basename(base);
        if not name in self.get_current_kernels(base):
            raise exceptions.IOError("Requested kernel %s is not in module %s"%(name,base));

        inpath = os.path.abspath(base);
        if len(basename) > 0:
            top = 'volk_' + basename + '_';
        else:
            top = 'volk_'
        oldvolk = re.compile(top[:-1]);

        self.convert_kernel(oldvolk, name, base, inpath, top);

        kernel = re.compile(name)
        search_kernels = Set([kernel])

        profile = re.compile('^\s*VOLK_PROFILE')
        puppet = re.compile('^\s*VOLK_PUPPET')
        infile = open(os.path.join(inpath, 'apps/', oldvolk.pattern + '_profile.cc'));
        otherinfile = open(os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'apps/volk_' + self.my_dict['name'] + '_profile.cc'));
        dest = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'apps/volk_' + self.my_dict['name'] + '_profile.cc');
        lines = infile.readlines();
        otherlines = otherinfile.readlines();
        open(dest, 'w+').write('');
        insert = False;
        inserted = False
        for otherline in otherlines:

            if self.lastline.match(otherline):
                insert = True;
            if insert and not inserted:
                inserted = True;
                for line in lines:
                    if kernel.search(line):
                        if profile.match(line):
                            outline = re.sub(oldvolk, 'volk_' + self.my_dict['name'], line);
                            open(dest, 'a').write(outline);
                        elif puppet.match(line):
                            outline = re.sub(oldvolk, 'volk_' + self.my_dict['name'], line);
                            open(dest, 'a').write(outline);
                            args = re.search("(?<=VOLK_PUPPET_PROFILE).*", line)
                            m_func = args.group(0).split(',')[0];
                            func = re.search('(?<=' + top + ').*', m_func);
                            search_kernels.add(re.compile(func.group(0)));
                            self.convert_kernel(oldvolk, func.group(0), base, inpath, top);
            write_okay = True;
            for kernel in search_kernels:
                if kernel.search(otherline):
                    write_okay = False
            if write_okay:
                open(dest, 'a').write(otherline);

        for kernel in search_kernels:
            print "Adding kernel %s from module %s"%(kernel.pattern,base)

        infile = open(os.path.join(inpath, 'lib/testqa.cc'));
        otherinfile = open(os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'lib/testqa.cc'));
        dest = os.path.join(self.my_dict['destination'], 'volk_' + self.my_dict['name'], 'lib/testqa.cc');
        lines = infile.readlines();
        otherlines = otherinfile.readlines();
        open(dest, 'w+').write('');
        inserted = False;
        insert = False
        for otherline in otherlines:

            if (re.match('\s*', otherline) == None or re.match('\s*#.*', otherline) == None):

                insert = True;
            if insert and not inserted:
                inserted = True;
                for line in lines:
                    for kernel in search_kernels:
                        if kernel.search(line):
                            if self.volk_run_tests.match(line):
                                outline = re.sub(oldvolk, 'volk_' + self.my_dict['name'], line);
                                open(dest, 'a').write(outline);
            write_okay = True;
            for kernel in search_kernels:
                if kernel.search(otherline):
                    write_okay = False
            if write_okay:
                open(dest, 'a').write(otherline);





