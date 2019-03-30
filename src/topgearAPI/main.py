from carro import *
from time import *
import serial

car1 = carro()
#ser = serial.Serial('COM6')


while True:
    car1.update()
    packet = car1.pack()
    print(packet)
  #  ser.write(packet.encode('utf-8'))
    sleep(0.4)