
from abc import ABC, abstractmethod
from ast import Subscript
from re import sub
import asyncio
from typing import Callable
from sp_com import Comunicator
import zmq
import asyncio

class SelenaModule(ABC):

    publisher: Comunicator
    subscriber: Comunicator
    debug_f: bool = False
        
    def debug(self, msg: str) -> None:
        if(self.debug_f):
            print(msg)

    def __init__(self, portNumberPub: int, portNumberSub: int, subscribe: str = None, identity: str = None, debug_f: bool = False) -> None:
        self.debug_f = debug_f
        self.publisher = Comunicator(portNumberPub, zmq.PUB, subscribe, identity, debug_f)
        self.subscriber = Comunicator(portNumberSub, zmq.SUB, subscribe, identity, debug_f)

    async def run(self) -> None:
        while True:
            (identity, message) = self.subscriber.recvMsg() 
            await self.processMsg(message)
    
    @abstractmethod
    async def processMsg(self, msg:str) -> None:
        print("From abstratct",msg)
