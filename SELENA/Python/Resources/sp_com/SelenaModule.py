
from abc import ABC, abstractmethod
from ast import Subscript
from re import sub
import asyncio
from typing import Callable
from sp_com import Comunicator
import zmq

class SelenaModule(ABC):

    publisher: Comunicator
    subscriber: Comunicator

    def __init__(self, portNumberPub: int, portNumberSub: int, subscribe: str = None, identity: str = None, debug_f: bool = False) -> None:
        self.publisher = Comunicator(portNumberPub, zmq.PUB, subscribe, identity, debug_f)
        self.subscriber = Comunicator(portNumberSub, zmq.SUB, subscribe, identity, debug_f)

    
    @abstractmethod
    def run(self, processMsg: Callable = lambda _ : {}) -> None:
        self.start()
        while True:
            serverMsg = self.subscriber.recvMsg()
            identity, message = serverMsg.split()
            asyncio.run(processMsg(identity, message))

    @abstractmethod
    def start(self) -> None:
        pass
