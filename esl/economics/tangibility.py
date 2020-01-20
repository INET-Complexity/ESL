from esl.geography.location import Location


class Tangible(object):
    """
    A tangible object exists in the real world, and hence has a location
    """
    place: Location


class Intangible(object):
    pass
