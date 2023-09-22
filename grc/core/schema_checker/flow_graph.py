from .utils import Spec, expand

OPTIONS_SCHEME = expand(
    parameters=Spec(types=dict, required=False, item_scheme=(str, str)),
    states=Spec(types=dict, required=False, item_scheme=(str, str)),
)

BLOCK_SCHEME = expand(
    name=str,
    id=str,
    **OPTIONS_SCHEME
)

FLOW_GRAPH_SCHEME = expand(
    options=Spec(types=dict, required=False, item_scheme=OPTIONS_SCHEME),
    blocks=Spec(types=dict, required=False, item_scheme=BLOCK_SCHEME),
    connections=list,

    metadata=Spec(types=dict, required=True, item_scheme=expand(
        file_format=Spec(types=int, required=True, item_scheme=None),
        grc_version=Spec(types=str, required=False, item_scheme=None),
    ))

)
