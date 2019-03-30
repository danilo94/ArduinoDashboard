from MemoryHandler import *
from addresses import *
from struct import *
class carro (object):

    def __init__(self):
        self.velocidade=0
        self.gasolina=0
        self.pontos=0
        self.posicao=0
        self.rpm=0
        self.nitro=0
        self.gerenciadorMemoria = MemoryHandler("zsnesw.exe")


    def update(self):
        self.updateVel()
        self.updateGas()
        self.updatePontos()
        self.updatePosicao()
        self.updateRpm()
        self.updateNitro()

    def pack(self):
        data = '1;'+str(self.velocidade)+';'+str(self.gasolina)+';'+str(self.pontos)+';'+str(self.posicao)+';'+str(self.rpm)+';'+str(self.nitro)+';'
        return data

    def updatePosicao(self):
        self.posicao = (int) ((self.gerenciadorMemoria.lerByte(CARROSAFRENTE)) + 1)

    def updateVel(self):
        self.velocidade = (int)(self.gerenciadorMemoria.lerPalavra(SPEEDMETER)/10)
        pass

    def updateGas(self):
        self.gasolina = (int) (100 - (((self.gerenciadorMemoria.lerByte(FUELCONSUMP))*100)/20))
        pass

    def updatePontos(self):
        self.pontos = self.gerenciadorMemoria.lerByte(POINTS)
        pass

    def updateRpm(self):
        self.rpm = 0
        pass

    def updateNitro(self):
        self.nitro = (int) ( self.gerenciadorMemoria.lerByte(0x00C64B06) - 53)
        pass

