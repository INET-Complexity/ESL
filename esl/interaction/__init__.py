from esl.interaction._interaction import *

from inspect import getframeinfo, stack

#from typing import Callable
#def callback(function: Callable[[], None], description: str, message):
def callback(function, description: str, message):
    """

    :param function:    The function
    :param description:
    :param message:
    :return:
    """
    caller = getframeinfo(stack()[1][0])

    file = caller.filename
    line = caller.lineno

    return callback_t(make_callback_handle(function), description, message, file, line)
