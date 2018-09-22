
from grc.core.blocks._flags import Flags


def test_simple_init():
    assert 'test' not in Flags()
    assert 'test' in Flags(' test')
    assert 'test' in Flags('test, foo')
    assert 'test' in Flags({'test', 'foo'})


def test_deprecated():
    assert Flags.DEPRECATED == 'deprecated'
    assert Flags('this is deprecated').deprecated is True


def test_extend():
    f = Flags('a')
    f.set('b')
    assert isinstance(f, Flags)
    f.set(u'b')
    assert isinstance(f, Flags)
    f = Flags(u'a')
    f.set('b')
    assert isinstance(f, Flags)
    f.set(u'b')
    assert isinstance(f, Flags)

    assert str(f) == 'a, b'
