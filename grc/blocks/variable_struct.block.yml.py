#!/usr/bin/env python

MAX_NUM_FIELDS = 20

HEADER = """\
id: variable_struct
label: Struct Variable
flags: [ show_id ]

parameters:
"""

TEMPLATES = """\

templates:
    imports: "def struct(data): return type('Struct', (object,), data)()"
    var_make: |-
        self.${{id}} = ${{id}} = struct({{
            % for i in range({0}):
            <%
                field = context.get('field' + str(i))
                value = context.get('value' + str(i))
            %>
            % if len(str(field)) > 2:
            ${{field}}: ${{value}},
            % endif
            % endfor
        }})
    var_value: |-
        struct({{
            % for i in range({0}):
            <%
                field = context.get('field' + str(i))
            %>
            % if len(str(field)) > 2:
            ${{field}}: ${{field}},
            % endif
            % endfor
        }})
"""

FIELD0 = """\
-   id: field0
    label: Field 0
    category: Fields
    dtype: string
    default: field0
    hide: part
"""

FIELDS = """\
-   id: field{0}
    label: Field {0}
    category: Fields
    dtype: string
    hide: part
"""

VALUES = """\
-   id: value{0}
    label: ${{field{0}}}
    dtype: raw
    default: '0'
    hide: ${{ 'none' if field{0} else 'all' }}
"""

ASSERTS = """\
- ${{ (str(field{0}) or "a")[0].isalpha() }}
- ${{ (str(field{0}) or "a").isalnum() }}
"""

FOOTER = """\

documentation: |-
    This is a simple struct/record like variable.

    Attribute/field names can be specified in the tab 'Fields'.
    For each non-empty field a parameter with type raw is shown.
    Value access via the dot operator, e.g. "variable_struct_0.field0"

file_format: 1
"""


def make_yml(num_fields):
    return ''.join((
        HEADER.format(num_fields),
        FIELD0, ''.join(FIELDS.format(i) for i in range(1, num_fields)),
        ''.join(VALUES.format(i) for i in range(num_fields)),
        'value: ${value}\n\nasserts:\n',
        ''.join(ASSERTS.format(i) for i in range(num_fields)),
        ''.join(TEMPLATES.format(num_fields)),
        FOOTER
    ))


if __name__ == '__main__':
    import sys
    try:
        filename = sys.argv[1]
    except IndexError:
        filename = __file__[:-3]

    data = make_yml(MAX_NUM_FIELDS)

    with open(filename, 'wb') as fp:
        fp.write(data.encode())
