
import sys
from sp_com import SelenaModule, Errors, Config
import time
import asyncio
from willump import Willump #if you want to import the class
import subprocess

class LoLManager(SelenaModule):

    lolPath: str = r"C:\Riot Games\League of Legends\LeagueClient.exe"
    end_f: bool = False
    wllp: Willump

    def __init__(self, portNumberPub: int, portNumberSub: int, subscribe: str = None, identity: str = None, debug_f: bool = False):
        SelenaModule.__init__(self, portNumberPub, portNumberSub, subscribe, identity, debug_f)       

    async def startLobby(self) -> None:
        result = await self.wllp.request('post', '/lol-lobby/v2/lobby', data={"queueId":420})

    async def setRoles(self) -> None:
        result = await self.connection.request('put', '/lol-lobby/v2/lobby/members/localMember/position-preferences', data={
                
                "firstPreference": "MIDDLE",
                "secondPreference": "TOP"
        })

    async def startQueue(self) -> None:
        result = await self.connection.request('post', '/lol-lobby/v2/lobby/matchmaking/search', data={})

    async def getInfo(self):
        result = await self.wllp.request('get', '/lol-matchmaking/v1/search', data={})
        return await result.json()
   
    async def processMsg(self, msg: str) -> None:
        if(msg == 'lol_queue_start'):
            self.debug("starting queue")
            await self.startLobby()
            await self.setRoles()
            await self.startQueue()
            time.sleep(5)
            info = await self.getInfo()
            self.debug(info)
        elif(msg == "lol_queue_quit"):
            self.debug("Not implemented stop queue")
            pass
        elif(msg == 'lol_quit'):
            self.debug("Stopping")
            self.end_f = True

    def start(self):
        subprocess.Popen(self.lolPath)
        self.wllp = asyncio.run(Willump.start())

    def run(self) -> None:
        super().run(self.processMsg)

def main():
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

    lolMng.run()

if __name__ == "__main__":
    main()
        




