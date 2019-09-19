import esl.identity
import esl.communicator


def test_communicator():
    sender = esl.identity.identity([1, 2, 3])

    recipient = esl.identity.identity([1, 2, 2])

    communicator_instance = esl.communicator.communicator()

    assert 3 == 3