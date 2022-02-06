import sys
sys.path.append(r"C:\Users\world\Documentos\personal\SELENA\SELENA\Python\Resources\PythonModules");
sys.path.append(r"C:\Users\world\Documentos\personal\SELENA\SELENA\Python\Resources\PythonModules\sp_com");
from sp_com import SelenaModule, Errors, Config
import time
import asyncio
import subprocess
from willump import Willump #if you want to import the class

class LoLManager(SelenaModule):

    lolPath: str = r"C:\Riot Games\League of Legends\LeagueClient.exe"
    wllp: Willump

    def __init__(self, portNumberPub: int, portNumberSub: int, subscribe: str = None, identity: str = None, debug_f: bool = False):
        SelenaModule.__init__(self, portNumberPub, portNumberSub, subscribe, identity, debug_f) 

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

    async def getInfo(self):
        response = await self.wllp.request('get', '/lol-matchmaking/v1/search', data={})
        return await response.json()

    async def end(self):
        await self.wllp.close()

    async def start(self):
        self.wllp = await Willump.start()

    async def processMsg(self, msg: str) -> None:
        if(msg == 'lol_queue_start'):
            await self.startLobby()
            await self.setRoles()
            await self.startQueue()
            time.sleep(5)
            x = await self.getInfo()
            print(x)
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

    await lolMng.start()
    await lolMng.run()
    await lolMng.end()

if __name__ == "__main__":
    asyncio.run(main())
        





