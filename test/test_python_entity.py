import esl.identity
import esl.entity


def test_entity():
    i = esl.identity.identity([1, 2, 3])
    e = esl.entity.entity(i)

    assert(e.identifier == i)
    j = esl.identity.identity([1, 2, 4])
    f = esl.entity.entity(j)
    assert(e != f)

