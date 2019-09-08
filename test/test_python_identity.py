import esl.identity


def test_identity():
    i = esl.identity.identity([1, 2, 3])
    j = esl.identity.identity([1, 2, 3])
    assert (i == j)
    assert (i <= j)
    assert (i >= j)

    k = esl.identity.identity([1, 2, 4])
    assert (i != k)
    assert (i < k)
    assert (not (k < i))

    l = esl.identity.identity([1, 2])
    assert (i != l)
    assert (l < i)
    assert (not (i < l))
