""""""

import functools
import grc.converter.cheetah_converter as parser


def test_basic():
    c = parser.Converter(names={'abc'})
    for convert in (c.convert_simple, c.convert_hard, c.to_python):
        assert 'abc' == convert('$abc')
        assert 'abc' == convert('$abc()')
        assert 'abc' == convert('$(abc)')
        assert 'abc' == convert('$(abc())')
        assert 'abc' == convert('${abc}')
        assert 'abc' == convert('${abc()}')

    assert c.stats['simple'] == 2 * 6
    assert c.stats['hard'] == 1 * 6


def test_simple():
    convert = parser.Converter(names={'abc': {'def'}})
    assert 'abc' == convert.convert_simple('$abc')
    assert 'abc.def' == convert.convert_simple('$abc.def')
    assert 'abc.def' == convert.convert_simple('$(abc.def)')
    assert 'abc.def' == convert.convert_simple('${abc.def}')
    try:
        convert.convert_simple('$abc.not_a_sub_key')
    except NameError:
        assert True
    else:
        assert False


def test_conditional():
    convert = parser.Converter(names={'abc'})
    assert '(asb_asd_ if abc > 0 else __not__)' == convert.convert_inline_conditional(
        '#if $abc > 0 then asb_$asd_ else __not__')


def test_simple_format_string():
    convert = functools.partial(parser.Converter(names={'abc'}).convert_simple, spec=parser.FormatString)
    assert '{abc}' == convert('$abc')
    assert '{abc:eval}' == convert('$abc()')
    assert '{abc}' == convert('$(abc)')
    assert '{abc:eval}' == convert('$(abc())')
    assert '{abc}' == convert('${abc}')
    assert '{abc:eval}' == convert('${abc()}')


def test_hard_format_string():
    names = {'abc': {'ff'}, 'param1': {}, 'param2': {}}
    convert = functools.partial(parser.Converter(names).convert_hard, spec=parser.FormatString)
    assert 'make_a_cool_block_{abc.ff}({param1}, {param2})' == \
           convert('make_a_cool_block_${abc.ff}($param1, $param2)')


converter = parser.Converter(names={'abc'})
c2p = converter.to_python


def test_opts():
    assert 'abc abc abc' == c2p('$abc $(abc) ${abc}')
    assert 'abc abc.abc abc' == c2p('$abc $abc.abc ${abc}')
    assert 'abc abc[''].abc abc' == c2p('$abc $abc[''].abc() ${abc}')


def test_nested():
    assert 'abc(abc) abc + abc abc[abc]' == c2p('$abc($abc) $(abc + $abc) ${abc[$abc]}')
    assert '(abc_abc_)' == c2p('(abc_$(abc)_)')


def test_nested2():
    class Other(parser.Python):
        nested_start = '{'
        nested_end = '}'
    assert 'abc({abc})' == converter.convert('$abc($abc)', spec=Other)


def test_nested3():
    class Other(parser.Python):
        start = '{'
        end = '}'
    assert '{abc(abc)}' == converter.convert('$abc($abc)', spec=Other)


def test_with_string():
    assert 'abc "$(abc)" abc' == c2p('$abc "$(abc)" ${abc}')
    assert 'abc \'$(abc)\' abc' == c2p('$abc \'$(abc)\' ${abc}')
    assert 'abc "\'\'$(abc)" abc' == c2p('$abc "\'\'$(abc)" ${abc}')


def test_if():
    result = converter.to_mako("""
        #if $abc > 0
            test
        #else if $abc < 0
            test
        #else
            bla
        #end if
    """)

    expected = """
        % if abc > 0:
            test
        % elif abc < 0:
            test
        % else:
            bla
        % endif
    """
    assert result == expected


def test_hash_end():
    result = converter.to_mako('$abc#slurp')
    assert result == '${abc}\\'


def test_slurp_if():
    result = converter.to_mako("""
        $abc#slurp
        #if $abc
    """)

    expected = """
        ${abc}
        % if abc:
    """
    assert result == expected

