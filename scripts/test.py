#python code
import os
import psutil
import serial
from time import sleep

port=serial.Serial('/dev/ttyUSB0',115200)
while True:
    #os.system("stty -F /dev/ttyUSB0 115200 cs8 -cstopb")
    #f=os.popen('cat /proc/cpuinfo | grep "cpu MHz" | tail -1 | cut -c 12- | cut -c -6')
    #speed=f.read()
    #cpu = "064"
    #mem = "192"
    #print speed
    #b=os.popen('free -m | grep buffer | tail -1|cut -c 20-')
    #memory = b.read()
    #umem = memory.split()[0]
    #fmem = memory.split()[1]
    
    #print umem
    #print fmem
    
    #tmem = int(umem) + int(fmem)
    
    #print tmem
    
    #mem = int((float(umem) / float(tmem)) * 255)
    #i=os.popen('top -bn2 | grep "Cpu(s)" |sed "s/.*, *\\([0-9.]*\\)%* id.*/\\1/" |awk \'{print 100 - $1}\'| tail -1')
    #cpuused = i.read()
    #print cpuused
    ''' 
    #print cpu
    cpuzeroes = 3-len(str(cpu))
    for i in range(0,cpuzeroes):
    	cpu = "0" + str(cpu)
    #print cpu
    memzeroes = 3-len(str(mem))
    for i in range(0,memzeroes):
    	mem = "0" + str(mem)
    #print mem
    speed = "START" + speed.strip() + " " + str(cpu) + str(mem)
    '''
    #print speed
    #print "echo '" + speed + "' >> /dev/ttyUSB0"
    #os.system("echo \'" + speed + "\' >> /dev/ttyUSB0")
    
    f=open("/proc/cpuinfo").read()
    speed = (f[f.find("MHz")+7:f.find("\ncache")-2])
    mem = psutil.phymem_usage().percent
    mem=int((mem*255.0)/100.0)
    cpu= psutil.cpu_percent()
    cpu = int((float(cpu)*255.0)/100.0)
   
    for i in xrange(0,8,2):
        if(speed[i+1]!="."):
  	    tempstring=speed[:i+1]+" "+speed[i+1:]
    	    speed = tempstring
    #print speed
    
    speed = 'START%s %03d%03d'%(speed.strip(),cpu,mem)
    port.write(speed)
    sleep(0.1)
