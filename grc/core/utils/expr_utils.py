"""
Copyright 2008-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import ast
import string


def expr_replace(expr, replace_dict):
    """
    Search for vars in the expression and add the prepend.

    Args:
        expr: an expression string
        replace_dict: a dict of find:replace

    Returns:
        a new expression with the prepend
    """
    expr_splits = _expr_split(expr, var_chars=VAR_CHARS + '.')
    for i, es in enumerate(expr_splits):
        if es in list(replace_dict.keys()):
            expr_splits[i] = replace_dict[es]
    return ''.join(expr_splits)


def get_variable_dependencies(expr, vars):
    """
    Return a set of variables used in this expression.

    Args:
        expr: an expression string
        vars: a list of variable names

    Returns:
        a subset of vars used in the expression
    """
    expr_toks = _expr_split(expr)
    return set(v for v in vars if v in expr_toks)


def sort_objects(objects, get_id, get_expr):
    """
    Sort a list of objects according to their expressions.

    Args:
        objects: the list of objects to sort
        get_id: the function to extract an id from the object
        get_expr: the function to extract an expression from the object

    Returns:
        a list of sorted objects
    """
    id2obj = {get_id(obj): obj for obj in objects}
    # Map obj id to expression code
    id2expr = {get_id(obj): get_expr(obj) for obj in objects}
    # Sort according to dependency
    sorted_ids = _sort_variables(id2expr)
    # Return list of sorted objects
    return [id2obj[id] for id in sorted_ids]


def dependencies(expr, names=None):
    node = ast.parse(expr, mode='eval')
    used_ids = frozenset(
        [n.id for n in ast.walk(node) if isinstance(n, ast.Name)])
    return used_ids & names if names else used_ids


def sort_objects2(objects, id_getter, expr_getter, check_circular=True):
    known_ids = {id_getter(obj) for obj in objects}

    def dependent_ids(obj):
        deps = dependencies(expr_getter(obj))
        return [id_ if id_ in deps else '' for id_ in known_ids]

    objects = sorted(objects, key=dependent_ids)

    if check_circular:  # walk var defines step by step
        defined_ids = set()  # variables defined so far
        for obj in objects:
            deps = dependencies(expr_getter(obj), known_ids)
            if not defined_ids.issuperset(deps):  # can't have an undefined dep
                raise RuntimeError(obj, deps, defined_ids)
            defined_ids.add(id_getter(obj))  # define this one

    return objects


VAR_CHARS = string.ascii_letters + string.digits + '_'


class _graph(object):
    """
    Simple graph structure held in a dictionary.
    """

    def __init__(self):
        self._graph = dict()

    def __str__(self):
        return str(self._graph)

    def add_node(self, node_key):
        if node_key in self._graph:
            return
        self._graph[node_key] = set()

    def remove_node(self, node_key):
        if node_key not in self._graph:
            return
        for edges in self._graph.values():
            if node_key in edges:
                edges.remove(node_key)
        self._graph.pop(node_key)

    def add_edge(self, src_node_key, dest_node_key):
        self._graph[src_node_key].add(dest_node_key)

    def remove_edge(self, src_node_key, dest_node_key):
        self._graph[src_node_key].remove(dest_node_key)

    def get_nodes(self):
        return list(self._graph.keys())

    def get_edges(self, node_key):
        return self._graph[node_key]


def _expr_split(expr, var_chars=VAR_CHARS):
    """
    Split up an expression by non alphanumeric characters, including underscore.
    Leave strings in-tact.
    #TODO ignore escaped quotes, use raw strings.

    Args:
        expr: an expression string

    Returns:
        a list of string tokens that form expr
    """
    toks = list()
    tok = ''
    quote = ''
    for char in expr:
        if quote or char in var_chars:
            if char == quote:
                quote = ''
            tok += char
        elif char in ("'", '"'):
            toks.append(tok)
            tok = char
            quote = char
        else:
            toks.append(tok)
            toks.append(char)
            tok = ''
    toks.append(tok)
    return [t for t in toks if t]


def _get_graph(exprs):
    """
    Get a graph representing the variable dependencies

    Args:
        exprs: a mapping of variable name to expression

    Returns:
        a graph of variable deps
    """
    vars = list(exprs.keys())
    # Get dependencies for each expression, load into graph
    var_graph = _graph()
    for var in vars:
        var_graph.add_node(var)
    for var, expr in exprs.items():
        for dep in get_variable_dependencies(expr, vars):
            if dep != var:
                var_graph.add_edge(dep, var)
    return var_graph


def _sort_variables(exprs):
    """
    Get a list of variables in order of dependencies.

    Args:
        exprs: a mapping of variable name to expression

    Returns:
        a list of variable names
    @throws Exception circular dependencies
    """
    var_graph = _get_graph(exprs)
    sorted_vars = list()
    # Determine dependency order
    while var_graph.get_nodes():
        # Get a list of nodes with no edges
        indep_vars = [var for var in var_graph.get_nodes()
                      if not var_graph.get_edges(var)]
        if not indep_vars:
            raise Exception('circular dependency caught in sort_variables')
        # Add the indep vars to the end of the list
        sorted_vars.extend(sorted(indep_vars))
        # Remove each edge-less node from the graph
        for var in indep_vars:
            var_graph.remove_node(var)
    return reversed(sorted_vars)
