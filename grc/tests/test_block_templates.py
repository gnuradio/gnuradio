import pytest

from grc.core.blocks._templates import MakoTemplates
from grc.core.errors import TemplateError


class Block(object):
    namespace_templates = {}

    templates = MakoTemplates(None)

    def __init__(self, **kwargs):
        self.namespace_templates.update(kwargs)


def test_simple():
    t = MakoTemplates(_bind_to=Block(num='123'), test='abc${num}')
    assert t['test'] == 'abc${num}'
    assert t.render('test') == 'abc123'
    assert 'abc${num}' in t._template_cache


def test_instance():
    block = Block(num='123')
    block.templates['test'] = 'abc${num}'
    assert block.templates.render('test') == 'abc123'
    assert block.templates is block.__dict__['templates']


def test_list():
    templates = ['abc${num}', '${2 * num}c']
    t = MakoTemplates(_bind_to=Block(num='123'), test=templates)
    assert t['test'] == templates
    assert t.render('test') == ['abc123', '123123c']
    assert set(templates) == set(t._template_cache.keys())


def test_parse_error():
    with pytest.raises(TemplateError):
        MakoTemplates(_bind_to=Block(num='123'), test='abc${num NOT CLOSING').render('test')


def test_parse_error2():
    with pytest.raises(TemplateError):
        MakoTemplates(_bind_to=Block(num='123'), test='abc${ WRONG_VAR }').render('test')
