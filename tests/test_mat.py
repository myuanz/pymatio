import pytest
import _pymatio as pm


def test_version():
    assert pm.get_library_version() == (1, 5, 24,)

if __name__ == '__main__':
    pytest.main()
