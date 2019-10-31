#!/usr/bin/env python3

from time import sleep
import serial
from sys import argv as args


try:
     port = args[1]
except:
     port = "/dev/ttyUSB0"

try:
     baud = args[2]
except:
     baud = 115200

ser = serial.Serial(port, baud) # Establish the connection on a specific port
# ser.ReadIntervalTimeout=0
while True:
     try:
          dicks = ser.readline().decode('utf-8')
     except:
          dicks = ser.readline()
          
     print(dicks,end='') # Read the newest output from the Arduino
     #sleep(.1) # Delay for one tenth of a second
    #  ser.
