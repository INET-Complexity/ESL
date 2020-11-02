import esl


def test_communicator():
    sender = esl.simulation.identity([1, 2, 3])

    recipient = esl.simulation.identity([1, 2, 2])

    communicator_instance = esl.communicator.communicator()

    assert 3 == 3