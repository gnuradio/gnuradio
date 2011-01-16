#! /usr/bin/env python

import sys
import re
import string
from xml.dom import minidom
from volk_regexp import *
from make_cpuid_x86_c import make_cpuid_x86_c
from make_cpuid_h import make_cpuid_h
from make_proccpu_sim import make_proccpu_sim
from make_set_simd import make_set_simd
from make_cpuid_generic_c import make_cpuid_generic_c
from make_cpuid_powerpc_c import make_cpuid_powerpc_c
from make_registry import make_registry
from make_h import make_h
from make_init_h import make_init_h
from make_config_fixed import make_config_fixed
from make_config_in import make_config_in
from make_c import make_c
from make_runtime_c import make_runtime_c
from make_init_c import make_init_c
from make_runtime import make_runtime
from make_typedefs import make_typedefs
from make_mktables import make_mktables
from make_environment_init_c import make_environment_init_c
from make_environment_init_h import make_environment_init_h

outfile_set_simd = open("../../config/lv_set_simd_flags.m4", "w");
outfile_reg = open("volk_registry.h", "w");
outfile_h = open("volk.h", "w");
outfile_c = open("../../lib/volk.c", "w");
outfile_runtime = open("volk_runtime.h", "w");
outfile_runtime_c = open("../../lib/volk_runtime.c", "w");
outfile_typedefs = open("volk_typedefs.h", "w");
outfile_init_h = open("../../lib/volk_init.h", "w");
outfile_init_c = open("../../lib/volk_init.c", "w");
outfile_cpu_h = open("volk_cpu.h", "w");
outfile_cpu_x86_c = open("../../lib/volk_cpu_x86.c", "w");
outfile_cpu_generic_c = open("../../lib/volk_cpu_generic.c", "w");
outfile_cpu_powerpc_c = open("../../lib/volk_cpu_powerpc.c", "w");
outfile_proccpu_sim = open("../../lib/volk_proccpu_sim.c", "w");
outfile_config_in = open("../../volk_config.h.in", "w");
outfile_config_fixed = open("volk_config_fixed.h", "w");
outfile_mktables = open("../../lib/volk_mktables.c", "w");
outfile_environment_c = open("../../lib/volk_environment_init.c", "w");
outfile_environment_h = open("volk_environment_init.h", "w");
infile = open("Makefile.am", "r");


mfile = infile.readlines();

datatypes = [];
functions = [];



for line in mfile:
    subline = re.search(".*(a16).*", line);
    if subline:
        subsubline = re.search("(?<=volk_).*", subline.group(0));
        if subsubline:
            dtype = remove_after_underscore.sub("", subsubline.group(0));
            subdtype = re.search("[0-9]+[A-z]+", dtype);
            if subdtype:
                datatypes.append(subdtype.group(0));
                

datatypes = set(datatypes);

for line in mfile:
    for dt in datatypes:
        if dt in line:
            subline = re.search("(volk_" + dt +"_.*(a16).*\.h)", line);
            if subline:
                
                subsubline = re.search(".+(?=\.h)", subline.group(0));
                functions.append(subsubline.group(0));

archs = [];
afile = minidom.parse("archs.xml");
filearchs = afile.getElementsByTagName("arch");
for filearch in filearchs:
    archs.append(str(filearch.attributes["name"].value));
                 
for arch in archs:
    a_var = re.search("^\$", arch);
    if a_var:
        archs.remove(arch);



archs_or = "("
for arch in archs:
    archs_or = archs_or + string.upper(arch) + "|";
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
    infile_source = open(func + ".h");
    begun_name = 0;
    begun_paren = 0;
    sourcefile = infile_source.readlines();
    infile_source.close();
    for line in sourcefile:

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
    replace = re.compile("\)");
    arched_args = replace.sub(", const char* arch) {", args);
    
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

outfile_mktables.write(make_mktables(functions));
outfile_mktables.close();


outfile_cpu_h.write(make_cpuid_h(filearchs));
outfile_cpu_h.close();

outfile_cpu_x86_c.write(make_cpuid_x86_c(filearchs));
outfile_cpu_x86_c.close();

outfile_proccpu_sim.write(make_proccpu_sim(filearchs));
outfile_proccpu_sim.close();

outfile_set_simd.write(make_set_simd(filearchs));
outfile_set_simd.close();

outfile_cpu_generic_c.write(make_cpuid_generic_c(filearchs)); 
outfile_cpu_generic_c.close();

outfile_cpu_powerpc_c.write(make_cpuid_powerpc_c(filearchs));
outfile_cpu_powerpc_c.close();

outfile_config_in.write(make_config_in(filearchs));
outfile_config_in.close();
    
outfile_reg.write(make_registry(filearchs, functions, fcountlist));
outfile_reg.close();

outfile_h.write(make_h(functions, arched_arglist, retlist));
outfile_h.close();  

outfile_init_h.write(make_init_h(functions, arched_arglist, retlist));
outfile_init_h.close();

outfile_config_fixed.write(make_config_fixed(filearchs));
outfile_config_fixed.close();

outfile_c.write( make_c(functions, taglist, arched_arglist, retlist, my_arglist, fcountlist));
outfile_c.close();

outfile_runtime_c.write(make_runtime_c(functions, taglist, arched_arglist, retlist, my_arglist, fcountlist));
outfile_runtime_c.close();

outfile_init_c.write(make_init_c(functions, filearchs));
outfile_init_c.close();

outfile_runtime.write(make_runtime(functions));
outfile_runtime.close();

outfile_typedefs.write(make_typedefs(functions, retlist, my_argtypelist));
outfile_typedefs.close();

outfile_environment_c.write(make_environment_init_c(filearchs));
outfile_environment_c.close();

outfile_environment_h.write(make_environment_init_h());
outfile_environment_h.close();
