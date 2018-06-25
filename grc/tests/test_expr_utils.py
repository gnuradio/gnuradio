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
        ['a', '1'],
        ['d', '5'],
        ['b', '2 * a + unknown * d'],
        ['c', '2 * a + b'],
    ]

    out = expr_utils.sort_objects2(objects, id_getter, expr_getter)

    assert out == expected


def test_other():
    test = [
        ['c', '2 * a + b'],
        ['a', '1'],
        ['b', '2 * c + unknown'],
    ]

    expr_utils.sort_objects2(test, id_getter, expr_getter, check_circular=False)

    with pytest.raises(RuntimeError):
        expr_utils.sort_objects2(test, id_getter, expr_getter)
