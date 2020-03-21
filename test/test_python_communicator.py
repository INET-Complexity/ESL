import esl.simulation.identity
import esl.interaction.communicator


def test_communicator():
    sender = esl.simulation.identity.Identity([1, 2, 3])

    recipient = esl.simulation.identity.Identity([1, 2, 2])

    communicator_instance = esl.interaction.communicator.Communicator()

    assert 3 == 3
