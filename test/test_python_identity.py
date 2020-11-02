import esl


def test_identity():
    i = esl.simulation.identity([1, 2, 3])
    j = esl.simulation.identity([1, 2, 3])
    assert (i == j)
    assert (i <= j)
    assert (i >= j)

    k = esl.simulation.identity([1, 2, 4])
    assert (i != k)
    assert (i < k)
    assert (not (k < i))

    l = esl.simulation.identity([1, 2])
    assert (i != l)
    assert (l < i)
    assert (not (i < l))
