import esl.version
import pytest


def test_version_built():
    if not hasattr(esl.version, "version"):
        pytest.fail("module missing: {}".format(version))
    
def test_semantic_version():
    text = esl.version.version()
    assert text is not None and len(text) >= 5 and text.count('.') >= 2 