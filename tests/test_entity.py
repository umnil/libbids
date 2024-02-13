from typing import Optional

from libbids.clibbids import Entity  # type: ignore


def test_constructor_with_name_and_optional_key_and_int_value() -> None:
    name = "Entity"
    key: Optional[str] = "KEY"
    value: int = 42
    padding: int = 2

    entity = Entity(name, key, value, padding)

    assert entity.id == "KEY-42"


def test_constructor_with_name_and_optional_key_and_string_value() -> None:
    name = "Entity"
    key: Optional[str] = "KEY"
    value: str = "VALUE"
    padding: int = 2

    entity = Entity(name, key, value, padding)

    assert entity.id == "KEY-VALUE"


def test_constructor_with_name_and_optional_key_and_invalid_string_value() -> None:
    name: str = "Entity"
    key: Optional[str] = "KEY"
    value: str = "INVALID"
    padding: int = 2

    entity: Entity = Entity(name, key, value, padding)

    assert entity.id == "KEY-INVALID"
    assert entity.padding == 2
