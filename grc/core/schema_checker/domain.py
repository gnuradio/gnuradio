from .utils import Spec, expand, str_

DOMAIN_CONNECTION = expand(
    type=Spec(types=list, required=True, item_scheme=None),
    connect=str_,
    cpp_connect=str_,
)

DOMAIN_SCHEME = expand(
    id=Spec(types=str_, required=True, item_scheme=None),
    label=str_,
    color=str_,
    multiple_connections_per_input=bool,
    multiple_connections_per_output=bool,

    templates=Spec(types=list, required=False, item_scheme=DOMAIN_CONNECTION)
)