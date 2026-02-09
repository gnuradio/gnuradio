from grc.core.params.param import Param


class _DummyParent:
    """Minimal parent placeholder for Param."""


def _make_param(dtype: str) -> Param:
    return Param(parent=_DummyParent(), id="test", dtype=dtype, default="")


def test_set_value_trims_whitespace_for_strings():
    param = _make_param("string")
    param.set_value("  tcp  ")
    assert param.get_value() == "tcp"


def test_set_value_trims_mixed_whitespace_variants():
    param = _make_param("string")
    for raw in (" tcp", "tcp ", "  tcp", "tcp  "):
        param.set_value(raw)
        assert param.get_value() == "tcp"


def test_set_value_preserves_multiline_whitespace():
    param = _make_param("_multiline")
    text = "  line 1\n  line 2  "
    param.set_value(text)
    assert param.get_value() == text
