from .utils import Spec, expand, str_

PARAM_SCHEME = expand(
    base_key=str_,   # todo: rename/remove

    id=str_,
    label=str_,
    category=str_,

    dtype=str_,
    default=object,

    options=list,
    option_labels=list,
    option_attributes=Spec(types=dict, required=False, item_scheme=(str_, list)),

    hide=str_,
)
PORT_SCHEME = expand(
    label=str_,
    domain=str_,

    id=str_,
    dtype=str_,
    vlen=(int, str_),

    multiplicity=(int, str_),
    optional=(bool, int, str_),
    hide=(bool, str_),
)
TEMPLATES_SCHEME = expand(
    imports=str_,
    var_make=str_,
    var_value=str_,
    make=str_,
    callbacks=list,
)
CPP_TEMPLATES_SCHEME = expand(
    includes=list,
    declarations=str_,
    make=str_,
    var_make=str_,
    callbacks=list,
    link=list,
    translations=dict,
)
BLOCK_SCHEME = expand(
    id=Spec(types=str_, required=True, item_scheme=None),
    label=str_,
    category=str_,
    flags=(list, str_),

    parameters=Spec(types=list, required=False, item_scheme=PARAM_SCHEME),
    inputs=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    outputs=Spec(types=list, required=False, item_scheme=PORT_SCHEME),

    asserts=(list, str_),
    value=str_,

    templates=Spec(types=dict, required=False, item_scheme=TEMPLATES_SCHEME),
    cpp_templates=Spec(types=dict, required=False, item_scheme=CPP_TEMPLATES_SCHEME),

    documentation=str_,
    grc_source=str_,

    file_format=Spec(types=int, required=True, item_scheme=None),

    block_wrapper_path=str_,  # todo: rename/remove
)
