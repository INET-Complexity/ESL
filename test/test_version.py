import pytest

import version


def test_version():
    if not hasattr(version, "version"):
        pytest.fail("module missing: {}".format(version))
