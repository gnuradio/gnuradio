import operator

import pytest

from grc.core.utils import expr_utils

id_getter = operator.itemgetter(0)
expr_getter = operator.itemgetter(1)


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
