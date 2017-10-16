
from grc.core.blocks._flags import Flags


def test_simple():
    assert 'test' in Flags('_test_')


def test_deprecated():
    assert Flags.DEPRECATED == 'deprecated'
    assert Flags('this is deprecated').deprecated is True


def test_extend():
    f = Flags('a')
    f += 'b'
    assert isinstance(f, Flags)
    f += u'b'
    assert isinstance(f, Flags)
    f = Flags(u'a')
    f += 'b'
    assert isinstance(f, Flags)
    f += u'b'
    assert isinstance(f, Flags)

    assert str(f) == 'abb'
