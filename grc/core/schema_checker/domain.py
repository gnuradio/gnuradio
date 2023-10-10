from .utils import Spec, expand
from .block import PARAM_SCHEME

DOMAIN_CONNECTION = expand(
    type=Spec(types=list, required=True, item_scheme=None),
    connect=str,
    cpp_connect=str,
    parameters=Spec(types=list, required=False, item_scheme=PARAM_SCHEME),
)

DOMAIN_SCHEME = expand(
    id=Spec(types=str, required=True, item_scheme=None),
    label=str,
    color=str,
    multiple_connections_per_input=bool,
    multiple_connections_per_output=bool,

    templates=Spec(types=list, required=False, item_scheme=DOMAIN_CONNECTION)
)
