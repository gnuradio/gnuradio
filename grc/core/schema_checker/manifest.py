from .utils import Spec, expand

MANIFEST_SCHEME = expand(
    title=Spec(types=str, required=True, item_scheme=None),
    version=str,
    brief=str,
    website=str,
    dependencies=list,
    repo=str,
    copyright_owner=list,
    gr_supported_version=list,
    tags=list,
    license=str,
    description=str,
    author=list,
    icon=str,
    file_format=Spec(types=int, required=True, item_scheme=None),
)
