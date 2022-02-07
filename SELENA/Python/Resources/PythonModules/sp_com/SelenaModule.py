
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
            print(msg, flush=True)

    def __init__(self, portNumberPub: int, portNumberSub: int, subscribe: str = None, identity: str = None, debug_f: bool = False) -> None:
        self.debug_f = debug_f
        self.publisher = Comunicator(portNumberPub, zmq.PUB, subscribe, identity, debug_f)
        self.subscriber = Comunicator(portNumberSub, zmq.SUB, subscribe, identity, debug_f)

    async def runAsync(self) -> None:
        await self.start()
        await self.run()
        await self.end()

    async def run(self) -> None:
        self.publisher.sendMsg("proofOfLife")
        while True:
            
            await asyncio.sleep(0.5)
            try:
                (identity, message) = self.subscriber.recvMsg(block_f=False) 
            except zmq.Again as e:
                #self.debug("No new message")
                continue
            if(message == 'proof_of_life'):
                self.publisher.sendMsg("alive")
            else:
                await self.processMsg(message)
    
    @abstractmethod
    async def start(self) -> None:
        pass

    @abstractmethod
    async def end(self) -> None:
        pass

    @abstractmethod
    async def processMsg(self, msg:str) -> None:
        print("From abstratct",msg)
