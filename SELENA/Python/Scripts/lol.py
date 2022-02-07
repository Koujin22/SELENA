import json
import sys
sys.path.append(r"C:\Users\world\Documentos\personal\SELENA\SELENA\Python\Resources\PythonModules");
sys.path.append(r"C:\Users\world\Documentos\personal\SELENA\SELENA\Python\Resources\PythonModules\sp_com");
from sp_com import SelenaModule, Errors, Config
import time
import asyncio
import subprocess
import threading
from willump import Willump #if you want to import the class

class LoLManager(SelenaModule):

    lolPath: str = r"C:\Riot Games\League of Legends\LeagueClient.exe"
    wllp: Willump

    cancel_queue_signal: asyncio.Condition
    exp_time: int = -1

    stop_f: bool = False

    def __init__(self, portNumberPub: int, portNumberSub: int, subscribe: str = None, identity: str = None, debug_f: bool = False):
        SelenaModule.__init__(self, portNumberPub, portNumberSub, subscribe, identity, debug_f) 
        self.cancel_queue_signal = asyncio.Condition()

    async def startLobby(self) -> None:    
        await self.wllp.request('post', '/lol-lobby/v2/lobby', data={"queueId":420})

    async def setRoles(self) -> None:
        await self.wllp.request('put', '/lol-lobby/v2/lobby/members/localMember/position-preferences', data={
                
                "firstPreference": "MIDDLE",
                "secondPreference": "TOP"
        })

    async def startQueue(self) -> None:
        await self.wllp.request('post', '/lol-lobby/v2/lobby/matchmaking/search', data={})

    async def endQueue(self) -> None:
        await self.wllp.request('delete', '/lol-lobby/v2/lobby/matchmaking/search', data={})

    async def getInfo(self) -> json:
        response = await self.wllp.request('get', '/lol-matchmaking/v1/search', data={})
        return await response.json()

    async def avoidAutoFill(self):
        while(not self.stop_f):
            
            self.debug("Iniciando chequeo")
            async with self.cancel_queue_signal:
                
                self.debug("Iniciando wait")
                while(self.exp_time < 0):
                    await self.cancel_queue_signal.wait()
                    self.debug("Sali del wait! exp_time: "+str(self.exp_time))
                    if(self.exp_time == -2):
                        return
                self.debug("INiciando sleep con: "+str(self.exp_time))
                await asyncio.sleep(self.exp_time)
                self.debug("Reiniciando")
                await self.endQueue()
                await self.startQueue()
                self.exp_time = -1


    async def end(self):
        await self.wllp.close()

    async def start(self):
        subprocess.Popen(self.lolPath)
        self.wllp = await Willump.start()

    async def processMsg(self, msg: str) -> None:
        if(msg == 'lol_queue_start'):
            await self.startLobby()
            await self.setRoles()
            await self.startQueue()
            time.sleep(5)
            x = await self.getInfo()
            async with self.cancel_queue_signal:
                self.exp_time = float(x['estimatedQueueTime']) if float(x['estimatedQueueTime']) < 120 else 120
                self.cancel_queue_signal.notify_all()
                
        elif(msg == 'lol_queue_quit'):
            await self.endQueue()
        elif(msg == 'lol_quit'):
            self.debug('Not implemented')

async def main():
    if(len(sys.argv) < 2):
        raise Errors.MissingCmdLnArgs



    config: Config = Config(sys.argv[1], sys.argv[2], sys.argv[3:])
    print(str(config), flush=True)
    lolMng = LoLManager(
        config.portNumberPub,
        config.portNumberSub,
        config.subscribe,
        config.identity,
        config.debug_f
    )

    L = await asyncio.gather(
        lolMng.avoidAutoFill(),
        lolMng.runAsync(),
    )

if __name__ == "__main__":
    asyncio.run(main())
        





