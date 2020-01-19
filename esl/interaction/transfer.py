
from esl.interaction.message import Message

class Transfer(Message):
    #identity<esl::law::owner<law::property>> transferor
    #identity<esl::law::owner<law::property>> transferee;
    #economics::accounting::inventory_filter<law::property> transferred;

    def __init__(self, transferor, transferee, transferred):
        pass