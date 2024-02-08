import tkinter as tk
from tkinter import *
import serial.tools.list_ports
import serial
import functools
import time



ports = serial.tools.list_ports.comports()
serialObj = serial.Serial()

speed = 0
RPM = 0
Celecius = [0,0,0,0,0]
Acceleration = [0,0,0]
Rotation = [0,0,0]

root = tk.Tk()
root.geometry("800x480")
root.title("arduino experimental communicator")
root.config(bg='grey')
topWrapper = Frame(root)
topWrapper.pack(fill=tk.BOTH, expand=True)
comselect = Frame(topWrapper,bg='white')
comselect.pack(side=RIGHT,fill=tk.BOTH, anchor=E)
rightconsole = Frame(topWrapper,bg='grey')
rightconsole.pack(side=LEFT,fill=tk.BOTH, expand=True)
serialWrapper = Frame(root,bg='white')
serialWrapper.pack(side=BOTTOM,fill=tk.X, expand=True, anchor=SE)

serialMonitor = Canvas(serialWrapper, height=200,width=780,bg='white')
serialMonitor.grid(row=0,column=0,rowspan=100)


def selectComPort(index):
    if serialObj.is_open:
        serialObj.close()
    currentPort= str(ports[index])
    selectedPort= str(currentPort.split(' ')[0])
    print(selectedPort)
    serialObj.port = selectedPort
    serialObj.baudrate = 9600
    serialObj.open()

def checkSerialPort():
    if serialObj.isOpen() and serialObj.in_waiting:
        recentPacketString = serialObj.readline().decode('utf').rstrip('\n')
        Label(dataFrame,text=recentPacketString, bg='white').pack(anchor=W)
        print(recentPacketString)
        if recentPacketString.find("speed:")!=-1:
            speed=float(recentPacketString[recentPacketString.find(":")+1:len(recentPacketString)])
            speeddisplay = Label(rightconsole, text=speed, height=1, width=5, bg='grey', fg='white', font=('Arial','20')).grid(row=0,column=2)
        
        checkValue = recentPacketString[-len(recentPacketString)]
        if checkValue == "A":
            ReadGyro(recentPacketString, Acceleration)
        elif checkValue == "R":
            ReadGyro(recentPacketString, Rotation)
        elif checkValue == "S":
            RPM = float(recentPacketString[recentPacketString.find(":")+1:])
            RPMdisplay = Label(rightconsole, text=RPM, height=1, width=5, bg='grey', fg='white', font=('Arial','20')).grid(row=1,column=2)
        elif checkValue.isdigit():
            if (float(recentPacketString[recentPacketString.find(":")+1:])) :
                Celecius[int(checkValue)] = float(recentPacketString[recentPacketString.find(":")+1:])

            
def sendSerial():
    if serialObj.isOpen():
        message=sender.get()
        message=(message + "\n").encode()
        serialObj.write(message)

def ReadGyro(message,destination):
    X = message.find('X')
    Y = message.find('Y')
    Z = message.find("Z")
    destination[0] = float(message[X+1:Y])
    destination[1] = float(message[Y+1:Z])
    destination[2] = float(message[Z+1:])

for comPort in ports:
    comButton = Button(comselect, text=comPort, height=1, width= 40, command = functools.partial(selectComPort, index=ports.index(comPort))).grid(row=ports.index(comPort),column=0)
speedLable = Label(rightconsole, text="current speed:", height=1, width=12, bg='grey', fg='white', font=('Arial','15')).grid(row=0,column=1)
speeddisplay = Label(rightconsole, text=speed, height=1, width=5, bg='grey', fg='white', font=('Arial','20')).grid(row=0,column=2)
RPMLable = Label(rightconsole, text="current RPM:", height=1, width=12, bg='grey', fg='white', font=('Arial','15')).grid(row=1,column=1)
RPMdisplay = Label(rightconsole, text=RPM, height=1, width=5, bg='grey', fg='white', font=('Arial','20')).grid(row=1,column=2)
sender = Entry(rightconsole, width=20)
sender.grid(row=3,column=1)
senderBtn = Button(rightconsole, text='send command', command=functools.partial(sendSerial)).grid(row=3, column=2)
vsb= Scrollbar(serialWrapper, orient='vertical', command=serialMonitor.yview)
vsb.grid(row=0,column=1,rowspan=100,sticky='ns')
serialMonitor.config(yscrollcommand= vsb.set)
dataFrame = Frame(serialMonitor,bg='white')
serialMonitor.create_window((10,0),window=dataFrame,anchor='nw')


while True:
    root.update()
    checkSerialPort()
    serialMonitor.config(scrollregion=serialMonitor.bbox('all'))