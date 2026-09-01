import operator
import os
import subprocess
import sys

import pytest

from grc.core.utils import expr_utils

id_getter = operator.itemgetter(0)
expr_getter = operator.itemgetter(1)

REPO_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..'))


def test_simple():
    objects = [
        ['c', '2 * a + b'],
        ['a', '1'],
        ['b', '2 * a + unknown * d'],
        ['d', '5'],
    ]

    expected = [
        ['d', '5'],
        ['a', '1'],
        ['b', '2 * a + unknown * d'],
        ['c', '2 * a + b'],
    ]

    out = expr_utils.sort_objects(objects, id_getter, expr_getter)

    assert out == expected


def test_circular():
    test = [
        ['c', '2 * a + b'],
        ['a', '1'],
        ['b', '2 * c + unknown'],
    ]

    # Should fail due to circular dependency
    with pytest.raises(Exception):
        expr_utils.sort_objects(test, id_getter, expr_getter)


def test_a_variable_never_precedes_what_it_depends_on():
    objects = [
        ['d', 'b + c'],
        ['b', 'a + 1'],
        ['c', 'a + 2'],
        ['a', '1'],
    ]

    order = [o[0] for o in
             expr_utils.sort_objects(objects, id_getter, expr_getter)]

    assert order.index('a') < order.index('b')
    assert order.index('a') < order.index('c')
    assert order.index('b') < order.index('d')
    assert order.index('c') < order.index('d')


CHILD = """\
import operator, sys
sys.path.insert(0, %r)
from grc.core.utils import expr_utils
objs = [['a', '1'], ['b', 'a + 1'], ['c', 'a + 2'], ['d', 'b + c']]
out = expr_utils.sort_objects(objs, operator.itemgetter(0), operator.itemgetter(1))
print('ORDER=' + ','.join(o[0] for o in out))
"""


def test_order_does_not_depend_on_the_hash_seed():
    """The order has to be a property of the flowgraph, not of the run.

    An ordering keyed on set iteration would vary here: str hashing is salted
    per interpreter (PEP 456), so the iteration order of a set of variable
    names moves between processes. Each seed gets its own interpreter, because
    one process only ever sees one salt.
    """
    orders = set()
    for seed in ('0', '1', '2', '3', 'random', 'random'):
        env = dict(os.environ, PYTHONHASHSEED=seed)
        proc = subprocess.run([sys.executable, '-c', CHILD % REPO_ROOT],
                              capture_output=True, text=True, env=env)
        assert proc.returncode == 0, proc.stderr
        tagged = [ln for ln in proc.stdout.splitlines()
                  if ln.startswith('ORDER=')]
        assert len(tagged) == 1, proc.stdout
        orders.add(tagged[0])

    assert len(orders) == 1, 'order changed between interpreters: %s' % orders
