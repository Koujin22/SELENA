import sys
from .Errors import OptionNotFound
class Config:

    portNumberPub: int
    portNumberSub: int
    subscribe: str = None
    identity: str = None
    debug_f: bool = False

    def __init__(self, portNumberPub:int,  portNumberSub:int, argv: list[str]):
        self.portNumberPub = portNumberPub
        self.portNumberSub = portNumberSub
        print(argv)
        for arg in argv:
            option, value = arg.split('=')
            if(option == '-s'):
                self.subscribe = value
            elif (option == '-i'):
                self.identity = value
            elif (option == '-d'):
                self.debug_f = bool(value)
            elif (option == '-n'):
                self.identity = value
                self.subscribe = value
            else:
                raise OptionNotFound

        pass

    def __str__(self):
        return F"Config:{{portNumberPub: {self.portNumberPub}, portNumberSub: {self.portNumberSub}, subscribe: {self.subscribe}, identity: {self.identity}, debug_f: {self.debug_f} }}"