import esl


def test_entity():
    i = esl.simulation.identity([1, 2, 3])
    e = esl.simulation.entity(i)

    assert(e.identifier == i)
    j = esl.simulation.identity([1, 2, 4])
    f = esl.simulation.entity(j)
    assert(e != f)

