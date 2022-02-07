from Errors import BadConfig, RecevingFromTypePUB, AsyncFromSyncType, SendingFromTypeSUB
import zmq
from zmq import Socket, Context
from abc import ABC, abstractmethod
from threading import Thread
from pymitter import EventEmitter
from typing import Callable

class Comunicator():

    context: Context
    socket: Socket
    turnToSend: bool
    ee: EventEmitter

    def __init__(self, portNumber: int, socketType: int, subscribe: str = None, identity: str = None, debug_f: bool = False) -> None:
        
        context = zmq.Context()

        self.context = context
        self.portNumber = portNumber
        self.socketType = socketType
        self.subscribe = subscribe
        self.debug_f = debug_f
        self.identity = identity
        
        self.ee = EventEmitter()

        if(socketType == zmq.SUB and subscribe is None):
            raise BadConfig

        if((socketType == zmq.DEALER or socketType == zmq.PUB )and identity is None):
            raise BadConfig

        if(socketType == zmq.REP):
            self.turnToSend = False
            self.ee.on("respond", self.sendMsg)
        elif(socketType == zmq.REQ):
            self.turnToSend = True
        else:
            self.turnToSend = None

        self.connectToServer()
 
    def debug(self, output: str) -> None:
        if(self.debug_f):
            print(output)

    def connectToServer(self) -> Socket:
        socket = self.context.socket(self.socketType)
        
        if(self.socketType == zmq.SUB and self.subscribe is not None):
            socket.setsockopt_string(zmq.SUBSCRIBE, self.subscribe)
            
        elif((self.socketType == zmq.DEALER or self.socketType == zmq.PUB)and self.identity is not None):
            self.debug("seteo string identity con "+self.identity)
            socket.setsockopt_string(zmq.IDENTITY, self.identity)

        socket.connect("tcp://localhost:"+self.portNumber)
        self.socket = socket
        return socket

    def recvMsg(self, block_f: bool = True) -> list[str]:
        if(self.socketType == zmq.PUB):
            raise RecevingFromTypePUB
        if(self.turnToSend is not None):
            if(self.turnToSend):
                raise AsyncFromSyncType

        if(block_f):
            self.debug("Waiting for reply.")
            message:list[bytes] = self.socket.recv_multipart()
            self.debug(f"Received msg {message}")
            
            if(self.turnToSend is not None):
                self.turnToSend = True
            decodedMsg:list[str] = map(lambda msg : msg.decode(), message)
            return decodedMsg
        else:
            #self.debug("Checking if new message")
            message:list[bytes] = self.socket.recv_multipart(flags=zmq.NOBLOCK)
            if(self.turnToSend is not None):
                self.turnToSend = True
            self.debug(f"New message! {message}")
            decodedMsg:list[str] = map(lambda msg : msg.decode(), message)
            return decodedMsg
            
    def sendMsg(self, msg: str) -> None:
        if(self.socketType == zmq.SUB):
            raise SendingFromTypeSUB
        if(self.turnToSend is not None):
            if(not self.turnToSend):
                raise AsyncFromSyncType
                
        self.debug(f"Sending msg {msg}")
        if(self.socketType == zmq.PUB):
            self.socket.send_string(self.identity, zmq.SNDMORE)
        self.socket.send_string(msg)
        if(self.turnToSend is not None):
            self.turnToSend = False

    def addMsgListener(self, event: str, handle: Callable) -> None:
        self.ee.on(event, handle)

    def emitEvent(self, event:str, *args, **kwargs) -> None:
        self.ee.emit(event, args, kwargs)

