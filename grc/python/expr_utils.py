"""
Copyright 2008-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import string
VAR_CHARS = string.letters + string.digits + '_'

class graph(object):
	"""
	Simple graph structure held in a dictionary.
	"""

	def __init__(self): self._graph = dict()

	def __str__(self): return str(self._graph)

	def add_node(self, node_key):
		if self._graph.has_key(node_key): return
		self._graph[node_key] = set()

	def remove_node(self, node_key):
		if not self._graph.has_key(node_key): return
		for edges in self._graph.values():
			if node_key in edges: edges.remove(node_key)
		self._graph.pop(node_key)

	def add_edge(self, src_node_key, dest_node_key):
		self._graph[src_node_key].add(dest_node_key)

	def remove_edge(self, src_node_key, dest_node_key):
		self._graph[src_node_key].remove(dest_node_key)

	def get_nodes(self): return self._graph.keys()

	def get_edges(self, node_key): return self._graph[node_key]

def expr_split(expr):
	"""
	Split up an expression by non alphanumeric characters, including underscore.
	Leave strings in-tact.
	#TODO ignore escaped quotes, use raw strings.
	@param expr an expression string
	@return a list of string tokens that form expr
	"""
	toks = list()
	tok = ''
	quote = ''
	for char in expr:
		if quote or char in VAR_CHARS:
			if char == quote: quote = ''
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
	return filter(lambda t: t, toks)

def expr_replace(expr, replace_dict):
	"""
	Search for vars in the expression and add the prepend.
	@param expr an expression string
	@param replace_dict a dict of find:replace
	@return a new expression with the prepend
	"""
	expr_splits = expr_split(expr)
	for i, es in enumerate(expr_splits):
		if es in replace_dict.keys():
			expr_splits[i] = replace_dict[es]
	return ''.join(expr_splits)

def get_variable_dependencies(expr, vars):
	"""
	Return a set of variables used in this expression.
	@param expr an expression string
	@param vars a list of variable names
	@return a subset of vars used in the expression
	"""
	expr_toks = expr_split(expr)
	return set(filter(lambda v: v in expr_toks, vars))

def get_graph(exprs):
	"""
	Get a graph representing the variable dependencies
	@param exprs a mapping of variable name to expression
	@return a graph of variable deps
	"""
	vars = exprs.keys()
	#get dependencies for each expression, load into graph
	var_graph = graph()
	for var in vars: var_graph.add_node(var)
	for var, expr in exprs.iteritems():
		for dep in get_variable_dependencies(expr, vars):
			if dep != var: var_graph.add_edge(dep, var)
	return var_graph

def sort_variables(exprs):
	"""
	Get a list of variables in order of dependencies.
	@param exprs a mapping of variable name to expression
	@return a list of variable names
	@throws Exception circular dependencies
	"""
	var_graph = get_graph(exprs)
	sorted_vars = list()
	#determine dependency order
	while var_graph.get_nodes():
		#get a list of nodes with no edges
		indep_vars = filter(lambda var: not var_graph.get_edges(var), var_graph.get_nodes())
		if not indep_vars: raise Exception('circular dependency caught in sort_variables')
		#add the indep vars to the end of the list
		sorted_vars.extend(sorted(indep_vars))
		#remove each edge-less node from the graph
		for var in indep_vars: var_graph.remove_node(var)
	return reversed(sorted_vars)

def sort_objects(objects, get_id, get_expr):
	"""
	Sort a list of objects according to their expressions.
	@param objects the list of objects to sort
	@param get_id the function to extract an id from the object
	@param get_expr the function to extract an expression from the object
	@return a list of sorted objects
	"""
	id2obj = dict([(get_id(obj), obj) for obj in objects])
	#map obj id to expression code
	id2expr = dict([(get_id(obj), get_expr(obj)) for obj in objects])
	#sort according to dependency
	sorted_ids = sort_variables(id2expr)
	#return list of sorted objects
	return [id2obj[id] for id in sorted_ids]

if __name__ == '__main__':
	for i in sort_variables({'x':'1', 'y':'x+1', 'a':'x+y', 'b':'y+1', 'c':'a+b+x+y'}): print i
