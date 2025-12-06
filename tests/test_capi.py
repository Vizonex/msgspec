import pytest
from msgspec._core import Factory, Field  # noqa: F401

from msgspec import field

_testcapi = pytest.importorskip("msgspec._testcapi")


def test_factory_type():
    t = _testcapi.factory_type()
    assert str(t) == "<class 'msgspec._core.Factory'>"


def test_factory_check():
    f = Factory(str)
    assert _testcapi.factory_check(f)
    not_a_factory = "NOT IT"
    assert not _testcapi.factory_check(not_a_factory)

def test_factory_new():
    f = _testcapi.factory_new(str)
    assert isinstance(f, Factory)

    # Should raise if item is not callable...
    with pytest.raises(TypeError):
        _testcapi.factory_new("NOT CALLABLE")



def test_factory_create():
    f = Factory(list)
    assert _testcapi.factory_create(f) == []

def test_field_type():
    t = _testcapi.field_type()
    # signature should match
    assert str(t) == "<class 'msgspec._core.Field'>"

def test_field_check():
    f = field(1)
    assert _testcapi.field_check(f)
    assert not _testcapi.field_check("NOT A FIELD")

def test_field_new():
    f = _testcapi.feild_new("name", None, None)
    assert isinstance(f, Field)

def test_field_get_default():
    f = field(default="X")
    assert _testcapi.field_get_default(f) == "X"

# TODO (Vizonex)
#   - field_get_default (Exception Case)
#   - field_get_facotry
#   - field_get_name
#   - field_new (Exception Case of trying both Factory and Default)
