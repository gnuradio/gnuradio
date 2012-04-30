#
# Copyright 2011-2012 Free Software Foundation, Inc.
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
import re
import sys
import glob

from volk_arch_defs import archs

remove_after_underscore = re.compile("_.*");
space_remove = re.compile(" ");
leading_space_remove = re.compile("^ *");
replace_arch = re.compile(", const char\* arch");
replace_bracket = re.compile(" {");
replace_volk = re.compile("volk");

def strip_trailing(tostrip, stripstr):
    lindex = tostrip.rfind(stripstr)
    tostrip = tostrip[0:lindex] + tostrip[lindex:len(tostrip)].replace(stripstr, "");
    return tostrip

srcdir = os.path.dirname(os.path.dirname(__file__))
hdr_files = glob.glob(os.path.join(srcdir, "include/volk/*.h"))

datatypes = [];
functions = [];

for line in hdr_files:
    subline = re.search(".*_(a|u)\.h.*", os.path.basename(line))
    if subline:
        subsubline = re.search("(?<=volk_).*", subline.group(0));
        if subsubline:
            dtype = remove_after_underscore.sub("", subsubline.group(0));
            subdtype = re.search("[0-9]+[A-z]+", dtype);
            if subdtype:
                datatypes.append(subdtype.group(0));


datatypes = set(datatypes);

for line in hdr_files:
    for dt in datatypes:
        if dt in line:
            subline = re.search("(volk_" + dt +"_.*(a|u).*\.h)", line);
            if subline:

                subsubline = re.search(".+(?=\.h)", subline.group(0));
                functions.append(subsubline.group(0));

archs_or = "("
for arch in archs:
    archs_or = archs_or + arch.name.upper() + "|";
archs_or = archs_or[0:len(archs_or)-1];
archs_or = archs_or + ")";

taglist = [];
fcountlist = [];
arched_arglist = [];
retlist = [];
my_arglist = [];
my_argtypelist = [];
for func in functions:
    tags = [];
    fcount = [];
    infile_source = open(os.path.join(srcdir, 'include', 'volk', func + ".h"))
    begun_name = 0;
    begun_paren = 0;
    sourcefile = infile_source.readlines();
    infile_source.close();
    for line in sourcefile:
#FIXME: make it work for multiple #if define()s
        archline = re.search("^\#if.*?LV_HAVE_" + archs_or + ".*", line);
        if archline:
            arch = archline.group(0);
            archline = re.findall(archs_or + "(?=( |\n|&))", line);
            if archline:
                archsublist = [];
                for tup in archline:
                    archsublist.append(tup[0]);
                fcount.append(archsublist);
        testline = re.search("static inline.*?" + func, line);
        if (not testline):
            continue
        tagline = re.search(func + "_.+", line);
        if tagline:
            tag = re.search("(?<=" + func + "_)\w+(?= *\()",line);
            if tag:
                tag = re.search("\w+", tag.group(0));
                if tag:
                    tags.append(tag.group(0));


        if begun_name == 0:
            retline = re.search(".+(?=" + func + ")", line);
            if retline:
                ret = retline.group(0);




            subline = re.search(func + ".*", line);
            if subline:
                subsubline = re.search("\(.*?\)", subline.group(0));
                if subsubline:
                    args = subsubline.group(0);

                else:
                    begun_name = 1;
                    subsubline = re.search("\(.*", subline.group(0));
                    if subsubline:
                        args = subsubline.group(0);
                        begun_paren = 1;
        else:
            if begun_paren == 1:
                subline = re.search(".*?\)", line);
                if subline:
                    args = args + subline.group(0);
                    begun_name = 0;
                    begun_paren = 0;
                else:
                    subline = re.search(".*", line);
                    args = args + subline.group(0);
            else:
                subline = re.search("\(.*?\)", line);
                if subline:
                    args = subline.group(0);
                    begun_name = 0;
                else:
                    subline = re.search("\(.*", line);
                    if subline:
                        args = subline.group(0);
                        begun_paren = 1;

    replace = re.compile("static ");
    ret = replace.sub("", ret);
    replace = re.compile("inline ");
    ret = replace.sub("", ret);
    arched_args = args[args.find('(')+1:args.find(')')]

    remove = re.compile('\)|\(|{');
    rargs = remove.sub("", args);
    sargs = rargs.split(',');



    margs = [];
    atypes = [];
    for arg in sargs:
        temp = arg.split(" ");
        margs.append(temp[-1]);
        replace = re.compile(" " + temp[-1]);
        atypes.append(replace.sub("", arg));


    my_args = ""
    arg_types = ""
    for arg in range(0, len(margs) - 1):
        this_arg = leading_space_remove.sub("", margs[arg]);
        my_args = my_args + this_arg + ", ";
        this_type = leading_space_remove.sub("", atypes[arg]);
        arg_types = arg_types + this_type + ", ";

    this_arg = leading_space_remove.sub("", margs[-1]);
    my_args = my_args + this_arg;
    this_type = leading_space_remove.sub("", atypes[-1]);
    arg_types = arg_types + this_type;
    my_argtypelist.append(arg_types);

    if(ret[-1] != ' '):
        ret = ret + ' ';

    arched_arglist.append(arched_args); #!!!!!!!!!!!
    my_arglist.append(my_args) #!!!!!!!!!!!!!!!!!
    retlist.append(ret);
    fcountlist.append(fcount);
    taglist.append(tags);

class kernel_class:
    def __init__(self, index):
        self.name = functions[index]
        self.pname = self.name.replace('volk_', 'p_')
        self.rettype = retlist[index]
        self.arglist_defs = my_argtypelist[index]
        self.arglist_namedefs = arched_arglist[index]
        self.arglist_names = my_arglist[index]
        self._tagdeps = fcountlist[index]
        self._taglist = taglist[index]

    def get_tags(self, archs):
        def is_in(x): return x.lower() in archs
        taglist = list()
        tagdeps = list()
        for i in range(len(self._tagdeps)):
            if all(map(is_in, self._tagdeps[i])):
                taglist.append(self._taglist[i])
                tagdeps.append(self._tagdeps[i])
        return taglist, tagdeps

    def __repr__(self):
        return self.name

kernels = map(kernel_class, range(len(retlist)))

if __name__ == '__main__':
    print kernels
