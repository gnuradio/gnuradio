# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from dataclasses import dataclass, field
from enum import auto, Enum
from typing import Optional, Union
from weakref import WeakKeyDictionary

from ...exceptions import UserException

from ..scoped_name import ScopedName
from ..specification import MappedType, WrappedClass, WrappedEnum


# The pre-PEP 585 types.
_LEGACY_TYPES = ('Dict', 'FrozenSet', 'List', 'Set', 'Tuple', 'Type')

# The modules defining standard types.
_ABC_MODULE = 'collections.abc'
_RE_MODULE = 're'
_TYPING_MODULE = 'typing'

# The standard types and their defining modules.
_STANDARD_TYPES = {
    'Annotated':    _TYPING_MODULE,
    'Any':          _TYPING_MODULE,
    'AnyStr':       _TYPING_MODULE,
    'BinaryIO':     _TYPING_MODULE,
    'Callable':     _ABC_MODULE,
    'ClassVar':     _TYPING_MODULE,
    'dict':         None,
    'Final':        _TYPING_MODULE,
    'frozenset':    None,
    'Generator':    _ABC_MODULE,
    'IO':           _TYPING_MODULE,
    'Iterable':     _ABC_MODULE,
    'Iterator':     _ABC_MODULE,
    'list':         None,
    'Literal':      _TYPING_MODULE,
    'Mapping':      _ABC_MODULE,
    'Match':        _RE_MODULE,
    'NamedTuple':   _TYPING_MODULE,
    'NoReturn':     _TYPING_MODULE,
    'Optional':     _TYPING_MODULE,
    'Pattern':      _RE_MODULE,
    'Protocol':     _TYPING_MODULE,
    'Sequence':     _ABC_MODULE,
    'set':          None,
    'Text':         _TYPING_MODULE,
    'TextIO':       _TYPING_MODULE,
    'tuple':        None,
    'type':         None,
    'Union':        _TYPING_MODULE,
}


class NodeType(Enum):
    """ The node types. """

    TYPING = auto()
    CLASS = auto()
    MAPPED_TYPE = auto()
    ENUM = auto()
    OTHER = auto()


class ParseState(Enum):
    """ The different parse states of a type hint. """

    REQUIRED = auto()
    PARSED = auto()


@dataclass
class ManagedTypeHint:
    """ Encapsulate a managed type hint. """

    # The type hint being managed.
    type_hint: str

    # The rendered docstring.
    as_docstring: Optional[str] = None

    # The rendered reST reference.
    as_rest_ref: Optional[str] = None

    # The parse state.
    parse_state: ParseState = ParseState.REQUIRED

    # The root node.
    root: Optional['TypeHintNode'] = None


@dataclass
class TypeHintNode:
    """ Encapsulate a node of a parsed type hint. """

    # The type.
    type: NodeType

    # The list of child nodes.
    children: Optional[list['TypeHintNode']] = None

    # The type-dependent definition.
    definition: Optional[Union[str, MappedType, WrappedClass, WrappedEnum]] = None

    # The PEP 484 definition if it is a TYPING node.
    pep484_definition: Optional[str] = None


class TypeHintManager:
    """ A manager for type hints on behalf of a Specification object. """

    # The map of specification objects and the corresponding manager object.
    _spec_manager_map = WeakKeyDictionary()

    def __new__(cls, spec):
        """ Return the existing manager for a specification or create a new one
        if necessary.
        """

        try:
            manager = cls._spec_manager_map[spec]
        except KeyError:
            manager = object.__new__(cls)

            manager._spec = spec
            manager._managed_type_hints = {}

            cls._spec_manager_map[spec] = manager

        return manager

    def as_docstring(self, type_hint, out, context):
        """ Return the type hint as a docstring. """

        managed_type_hint = self._get_managed_type_hint(type_hint, out)

        # See if it needs rendering.
        if managed_type_hint.as_docstring is None:
            managed_type_hint.as_docstring = self._render(managed_type_hint,
                    out, context=context)

        return managed_type_hint.as_docstring

    def as_rest_ref(self, type_hint, out, context, as_xml=False):
        """ Return the type hint with appropriate reST references. """

        managed_type_hint = self._get_managed_type_hint(type_hint, out)

        # See if it needs rendering.
        if managed_type_hint.as_rest_ref is None:
            managed_type_hint.as_rest_ref = self._render(managed_type_hint,
                    out, context=context, rest_ref=True, as_xml=as_xml)

        return managed_type_hint.as_rest_ref

    def as_type_hint(self, type_hint, out, context, defined):
        """ Return the type hint as a type hint. """

        managed_type_hint = self._get_managed_type_hint(type_hint, out)

        # Note that we always render type hints as they can be different before
        # and after a class or enum is defined in the .pyi file.
        return self._render(managed_type_hint, out, pep484=True,
                context=context, defined=defined)

    def _get_managed_type_hint(self, type_hint, out):
        """ Return the unique (for the specification) managed type hint for a
        type hint.
        """

        try:
            hint_in, hint_out = self._managed_type_hints[type_hint]
        except KeyError:
            hint_in = ManagedTypeHint(type_hint)
            hint_out = ManagedTypeHint(type_hint)
            self._managed_type_hints[type_hint] = (hint_in, hint_out)

        return hint_out if out else hint_in

    def _parse(self, managed_type_hint, out):
        """ Ensure a type hint has been parsed. """

        if managed_type_hint.parse_state is ParseState.REQUIRED:
            managed_type_hint.root = self._parse_node(out,
                    managed_type_hint.type_hint)
            managed_type_hint.parse_state = ParseState.PARSED

    def _parse_node(self, out, text, start=0, end=None):
        """ Return a single node of a parsed type hint. """

        if end is None:
            end = len(text)
            top_level = True
        else:
            top_level = False

        # Find the name and any opening and closing brackets.
        start = self._strip_leading(text, start, end)
        name_start = start

        end = self._strip_trailing(text, start, end)
        name_end = end

        children = None

        i = text[start:end].find('[')
        if i >= 0:
            i += start

            children = []

            # The last character must be a closing bracket.
            if text[end - 1] != ']':
                raise UserException(
                        f"type hint '{text}': ']' expected at position {end}")

            # Find the end of any name.
            name_end = self._strip_trailing(text, name_start, i)

            while True:
                # Skip the opening bracket or comma.
                i += 1

                # Find the next comma, if any.
                depth = 0

                for part_i in range(i, end):
                    if text[part_i] == '[':
                        depth += 1

                    elif text[part_i] == ']' and depth != 0:
                        depth -= 1

                    elif text[part_i] in ',]' and depth == 0:
                        # Recursively parse this part.
                        children.append(self._parse_node(out, text, i, part_i))

                        i = part_i
                        break
                else:
                    break

        # See if we have a name.
        if name_start != name_end:
            # Get the name. */
            name = text[name_start:name_end]

            # Convert pre-PEP 585 types.
            if name in _LEGACY_TYPES:
                name = name.lower()

            # See if it is a standard type.
            try:
                type_module = _STANDARD_TYPES[name]

                if name == 'Union':
                    # If there are no children assume it is because they have
                    # been omitted.
                    if len(children) == 0:
                        return None

                    # Flatten any unions.
                    flattened = []

                    for child in children:
                        if child.type is NodeType.TYPING and child.definition == 'Union':
                            for grandchild in child.children:
                                flattened.append(grandchild)
                        else:
                            flattened.append(child)

                    children = flattened

                pep484_definition = name if type_module is None else type_module + '.' + name

                node = TypeHintNode(NodeType.TYPING, children=children,
                        definition=name, pep484_definition=pep484_definition)

            except KeyError:
                # Search for the type.
                node = self._lookup_type(name, out, children)
        else:
            # At the top level we must have brackets and they must not be empty.
            if top_level and (children is None or len(children) == 0):
                raise UserException(
                        f"type hint '{text}': must have non-empty brackets")

            # Return the representation of brackets.
            node = TypeHintNode(NodeType.TYPING, children=children)

        return node

    def _render(self, managed_type_hint, out, context=None, defined=None,
            pep484=False, rest_ref=False, as_xml=False, context_stack=None):
        """ Return a rendered type hint. """

        self._parse(managed_type_hint, out)

        if managed_type_hint.root is not None:
            if context_stack is None:
                context_stack = []

            # Add any outer context.
            if context is not None:
                context_stack.append(context)

            s = self._render_node(managed_type_hint.root, out, pep484,
                    rest_ref, defined, as_xml, context_stack)

            if context is not None:
                context_stack.pop()
        else:
            s = self._maybe_any_object(managed_type_hint.type_hint, pep484,
                    as_xml)

        return s

    def _render_node(self, node, out, pep484, rest_ref, defined, as_xml,
            context_stack):
        """ Render a single node. """

        if node.type is NodeType.TYPING:
            if node.definition is None:
                s = ''
            elif pep484:
                s = node.pep484_definition
            else:
                s = node.definition

            if node.children is not None:
                s += self._render_children(node, out, pep484, rest_ref,
                        defined, as_xml, context_stack)

            return s

        if node.type is NodeType.CLASS:
            klass = node.definition

            # Get any managed type hint.
            type_hints = klass.type_hints
            if type_hints is not None:
                type_hint = type_hints.hint_out if out else type_hints.hint_in
                if type_hint is not None:
                    managed_type_hint = self._get_managed_type_hint(type_hint,
                            out)

                    # If the type hint isn't in the current context then render
                    # it.
                    if klass not in context_stack:
                        context_stack.append(klass)
                        s = self._render(managed_type_hint, out,
                                pep484=pep484, rest_ref=rest_ref,
                                defined=defined, as_xml=as_xml,
                                context_stack=context_stack)
                        context_stack.pop()

                        return s

            from .formatters import (fmt_class_as_rest_ref,
                    fmt_class_as_type_hint, fmt_scoped_py_name)

            if rest_ref:
                return fmt_class_as_rest_ref(klass)

            if pep484:
                return fmt_class_as_type_hint(self._spec, klass, defined)

            return fmt_scoped_py_name(klass.scope, klass.py_name.name)

        if node.type is NodeType.MAPPED_TYPE:
            mapped_type = node.definition

            # Get any managed type hint.
            type_hints = mapped_type.type_hints
            if type_hints is not None:
                type_hint = type_hints.hint_out if out else type_hints.hint_in
                if type_hint is not None:
                    managed_type_hint = self._get_managed_type_hint(type_hint,
                            out)

                    return self._render(managed_type_hint, out, pep484=pep484,
                            rest_ref=rest_ref, defined=defined, as_xml=as_xml,
                            context_stack=context_stack)

            # This will only happen if the mapped type doesn't have type hints.
            return mapped_type.cpp_name.name

        if node.type is NodeType.ENUM:
            from .formatters import (fmt_enum_as_rest_ref,
                    fmt_enum_as_type_hint, fmt_scoped_py_name)

            enum = node.definition

            if rest_ref:
                return fmt_enum_as_rest_ref(enum)

            if pep484:
                return fmt_enum_as_type_hint(self._spec, enum, defined)

            return fmt_scoped_py_name(enum.scope, enum.py_name.name)

        # We only render children for docstrings.
        if node.children is not None and defined is None:
            return node.definition + self._render_children(node, out, pep484,
                    rest_ref, defined, as_xml, context_stack)

        return self._maybe_any_object(node.definition, pep484, as_xml)

    def _render_children(self, node, out, pep484, rest_ref, defined, as_xml,
            context_stack):
        """ Render the children of a node. """

        children = []

        for child in node.children:
            # For Callable the first argument is in and the rest (ie. the
            # second) is out.
            if node.definition == 'Callable':
                fixed_out = child is not node.children[0]
            else:
                fixed_out = out

            children.append(
                    self._render_node(child, fixed_out, pep484, rest_ref,
                            defined, as_xml, context_stack))

        return '[' + ', '.join(children) + ']'

    def _lookup_enum(self, name, scopes):
        """ Lookup an enum using its C/C++ name. """

        for enum in self._spec.enums:
            if enum.fq_cpp_name is not None and enum.fq_cpp_name.base_name == name and enum.scope in scopes:
                return enum

        return None

    def _lookup_class(self, name, scope):
        """ Lookup a class/struct/union using its C/C++ name. """

        for klass in self._spec.classes:
            if klass.scope is scope and not klass.external and klass.iface_file.fq_cpp_name.base_name == name:
                return klass

        return None

    def _lookup_mapped_type(self, name):
        """ Lookup a mapped type using its C/C++ name. """

        for mapped_type in self._spec.mapped_types:
            if mapped_type.cpp_name is not None and mapped_type.cpp_name.name == name:
                return mapped_type

        return None

    def _lookup_type(self, name, out, children):
        """ Look up a qualified Python type and return the corresponding node.
        """

        # Start searching at the global level.
        scope_klass = None
        scope_mapped_type = None

        # We allow both Python and C++ scope separators.
        scoped_name = ScopedName.parse(name.replace('.', '::'))

        for part_i, part in enumerate(scoped_name):
            is_last_part = ((part_i + 1) == len(scoped_name))

            # See if it's an enum.
            enum = self._lookup_enum(part, (scope_klass, scope_mapped_type))
            if enum is not None:
                if is_last_part:
                    return TypeHintNode(NodeType.ENUM, definition=enum)

                # There is some left so the whole lookup has failed.
                break

            # If we have a mapped type scope then we must be looking for an
            # enum, which we have failed to find.
            if scope_mapped_type is not None:
                break

            if scope_klass is None:
                # We are looking at the global level, so see if it is a mapped
                # type.
                mapped_type = self._lookup_mapped_type(part)
                if mapped_type is not None:
                    # If we have used the whole name then the lookup has
                    # succeeded.
                    if is_last_part:
                        return TypeHintNode(NodeType.MAPPED_TYPE,
                                definition=mapped_type)

                    # Otherwise this is the scope for the next part.
                    scope_mapped_type = mapped_type

            if scope_mapped_type is None:
                # If we get here then it must be a class.
                klass = self._lookup_class(part, scope_klass)
                if klass is None:
                    break

                # If we have used the whole name then the lookup has succeeded.
                if is_last_part:
                    return TypeHintNode(NodeType.CLASS, definition=klass)

                # Otherwise this is the scope for the next part.
                scope_klass = klass

            # If we have run out of name then the lookup has failed.
            if is_last_part:
                break

        # Nothing was found.
        return TypeHintNode(NodeType.OTHER, definition=name, children=children)

    def _maybe_any_object(self, hint, pep484, as_xml):
        """ Return a hint taking into account that it may be any sort of
        object.
        """

        if hint == 'Any':
            return self._any_object(pep484)

        # Don't worry if the voidptr name is qualified in any way.
        if hint.endswith('voidptr'):
            return format_voidptr(self._spec, as_xml)

        return hint

    @staticmethod
    def _any_object(pep484):
        """ Return a hint taking into account that it may be any sort of
        object.
        """

        return 'typing.Any' if pep484 else 'object'

    @staticmethod
    def _strip_leading(text, start, end):
        """ Return the index of the first non-space of a string. """
    
        while start < end and text[start] == ' ':
            start += 1

        return start

    @staticmethod
    def _strip_trailing(text, start, end):
        """ Return the index after the last non-space of a string. """

        while end > start and text[end - 1] == ' ':
            end -= 1

        return end


def format_voidptr(spec, as_xml):
    """ Return the representation of a voidptr in the context of either a type
    hint, XML or a docstring.
    """

    voidptr = spec.sip_module + ('.' if spec.sip_module else '') + 'voidptr'

    if as_xml:
        return f':py:class:`~{voidptr}`'

    return voidptr
