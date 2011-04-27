import re
import string

remove_after_underscore = re.compile("_.*");
space_remove = re.compile(" ");
leading_space_remove = re.compile("^ *");
replace_arch = re.compile(", const char\* arch");
replace_bracket = re.compile(" {");
replace_volk = re.compile("volk");

def strip_trailing(tostrip, stripstr):
    lindex = tostrip.rfind(stripstr)
    tostrip = tostrip[0:lindex] + string.replace(tostrip[lindex:len(tostrip)], stripstr, "");
    return tostrip
