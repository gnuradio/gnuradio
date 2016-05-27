#!/usr/bin/env python

MAX_NUM_FIELDS = 20

HEADER = """\
<block>
  <name>Struct Variable</name>
  <key>variable_struct</key>
  <import>def struct(data): return type('Struct', (object,), data)()</import>
  <var_make>self.$id = $id = struct({{#slurp
#for $i in range({0}):
#set $field = $getVar('field' + str(i))
#if len(str($field)) > 2
$field: $getVar('value' + str(i)), #slurp
#end if
#end for
}})</var_make>
  <var_value>struct({{#slurp
#for $i in range({0}):
#set $field = $getVar('field' + str(i))
#if len(str($field)) > 2
$field: $getVar('value' + str(i)), #slurp
#end if
#end for
}})</var_value>
  <make></make>
"""

FIELD0 = """\
  <param>
    <name>Field 0</name>
    <key>field0</key>
    <value>field0</value>
    <type>string</type>
    <hide>part</hide>
    <tab>Fields</tab>
  </param>
"""

FIELDS = """\
  <param>
    <name>Field {0}</name>
    <key>field{0}</key>
    <value></value>
    <type>string</type>
    <hide>part</hide>
    <tab>Fields</tab>
  </param>
"""

VALUES = """\
  <param>
    <name>$field{0}()</name>
    <key>value{0}</key>
    <value>0</value>
    <type>raw</type>
    <hide>#if $field{0}() then 'none' else 'all'#</hide>
  </param>
"""

CHECKS = """\
  <check>($str($field{0}) or "a")[0].isalpha()</check>
  <check>($str($field{0}) or "a").isalnum()</check>
"""

FOOTER = """\
  <doc>This is a simple struct/record like variable.

Attribute/field names can be specified in the tab 'Fields'.
For each non-empty field a parameter with type raw is shown.
Value access via the dot operator, e.g. "variable_struct_0.field0"
  </doc>
</block>
"""


def make_xml(num_fields):
    return ''.join((
        HEADER.format(num_fields),
        FIELD0, ''.join(FIELDS.format(i) for i in range(1, num_fields)),
        ''.join(VALUES.format(i) for i in range(num_fields)),
        ''.join(CHECKS.format(i) for i in range(num_fields)),
        FOOTER
    ))


if __name__ == '__main__':
    import sys
    try:
        filename = sys.argv[1]
    except IndexError:
        filename = __file__[:-3]

    data = make_xml(MAX_NUM_FIELDS)

    with open(filename, 'w') as fp:
        fp.write(data.encode())
