# from https://hg.python.org/cpython/file/default/Lib/collections/__init__.py

from collections import MutableMapping


class ChainMap(MutableMapping):
    """ A ChainMap groups multiple dicts (or other mappings) together
    to create a single, updateable view.

    The underlying mappings are stored in a list.  That list is public and can
    be accessed or updated using the *maps* attribute.  There is no other
    state.

    Lookups search the underlying mappings successively until a key is found.
    In contrast, writes, updates, and deletions only operate on the first
    mapping.

    """

    def __init__(self, *maps):
        """Initialize a ChainMap by setting *maps* to the given mappings.
        If no mappings are provided, a single empty dictionary is used.

        """
        self.maps = list(maps) or [{}]          # always at least one map

    def __missing__(self, key):
        raise KeyError(key)

    def __getitem__(self, key):
        for mapping in self.maps:
            try:
                # can't use 'key in mapping' with defaultdict
                return mapping[key]
            except KeyError:
                pass
        # support subclasses that define __missing__
        return self.__missing__(key)

    def get(self, key, default=None):
        return self[key] if key in self else default

    def __len__(self):
        # reuses stored hash values if possible
        return len(set().union(*self.maps))

    def __iter__(self):
        return iter(set().union(*self.maps))

    def __contains__(self, key):
        return any(key in m for m in self.maps)

    def __bool__(self):
        return any(self.maps)

    def __repr__(self):
        return '{0.__class__.__name__}({1})'.format(
            self, ', '.join(map(repr, self.maps)))

    @classmethod
    def fromkeys(cls, iterable, *args):
        """Create a ChainMap with a single dict created from the iterable."""
        return cls(dict.fromkeys(iterable, *args))

    def copy(self):
        """New ChainMap or subclass with a new copy of maps[0] and refs to maps[1:]"""
        return self.__class__(self.maps[0].copy(), *self.maps[1:])

    __copy__ = copy

    def new_child(self, m=None):                # like Django's Context.push()
        """New ChainMap with a new map followed by all previous maps.
        If no map is provided, an empty dict is used.
        """
        if m is None:
            m = {}
        return self.__class__(m, *self.maps)

    @property
    def parents(self):                          # like Django's Context.pop()
        """New ChainMap from maps[1:]."""
        return self.__class__(*self.maps[1:])

    def __setitem__(self, key, value):
        self.maps[0][key] = value

    def __delitem__(self, key):
        try:
            del self.maps[0][key]
        except KeyError:
            raise KeyError(
                'Key not found in the first mapping: {!r}'.format(key))

    def popitem(self):
        """Remove and return an item pair from maps[0]. Raise KeyError is maps[0] is empty."""
        try:
            return self.maps[0].popitem()
        except KeyError:
            raise KeyError('No keys found in the first mapping.')

    def pop(self, key, *args):
        """Remove *key* from maps[0] and return its value. Raise KeyError if *key* not in maps[0]."""
        try:
            return self.maps[0].pop(key, *args)
        except KeyError:
            raise KeyError(
                'Key not found in the first mapping: {!r}'.format(key))

    def clear(self):
        """Clear maps[0], leaving maps[1:] intact."""
        self.maps[0].clear()
