import pymatio as pm


def test_version():
    assert pm.get_library_version() == (1, 5, 27)
