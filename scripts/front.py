#!/usr/bin/python

import os
import psutil
import serial
from time import sleep

port=serial.Serial('/dev/ttyUSB0',115200)
while True:
    f=open("/proc/cpuinfo").read()
    speed = (f[f.find("MHz")+7:f.find("\ncache")-2])
    mem = psutil.phymem_usage().percent
    cpu = psutil.cpu_percent()
   
    #print 'SPEED %s'%(speed)
    #print 'CPU %d'%(cpu)
    #print 'MEM %d'%(mem)
    port.write('SPEED %s\n'%(speed))
    port.write('CPU %d\n'%(cpu))
    port.write('MEM %d\n'%(mem))
    sleep(0.1)
