import collections

Spec = collections.namedtuple('Spec', 'types required item_scheme')


def expand(**kwargs):
    def expand_spec(spec):
        if not isinstance(spec, Spec):
            types_ = spec if isinstance(spec, tuple) else (spec,)
            spec = Spec(types=types_, required=False, item_scheme=None)
        elif not isinstance(spec.types, tuple):
            spec = Spec(types=(spec.types,), required=spec.required,
                        item_scheme=spec.item_scheme)
        return spec
    return {key: expand_spec(value) for key, value in kwargs.items()}


class Message(collections.namedtuple('Message', 'path type message')):
    fmt = '{path}: {type}: {message}'

    def __str__(self):
        return self.fmt.format(**self._asdict())
