import tkinter as tk
from tkinter import *
import serial.tools.list_ports
import serial
import functools
import math
import csv
import time


def RecordData():
    global recordNumb
    # print(recordNumb/recordInterval)
    if (recordNumb/recordInterval).is_integer():
        LogRecord(recordNumb/recordInterval)
    RecordCache.append([round(time.time()-Initialtime,3),RPM,Celecius[0],Celecius[1],Celecius[2],Celecius[3],Celecius[4],Acceleration[0],Acceleration[1],Acceleration[2],rotation[0],rotation[1],rotation[2]])
    recordNumb = recordNumb + 1

def LogRecord(lognumber):
    str(int(lognumber))
    filename = "recorded_data" + str(int(lognumber)) + ".csv"
    with open(filename, 'w') as new_file:
        csv_writer = csv.writer(new_file)
        csv_writer.writerow(['Time','RPM','T0','T1','T2','T3','T4','Acceleration-X','Acceleration-Y','Acceleration-Z','Rotation-X','Rotation-Y','Rotation-Z'])
        for sample in RecordCache:
            csv_writer.writerow(sample)
    RecordCache.clear()

#Display window functions
def OpenOptions():
    global Options
    if Options:
        optionsWrapper.place(relx = 2, rely = 2)
        Options = False

    else:
        optionsWrapper.place(relx = ColumnC, rely = 0.5, anchor = CENTER)
        Options = True

def RecordSwitch():
    global Record, recordButton, Initialtime, recordNumb
    if Record:
        recordButton.config(text="Not recording")
        Record = False
        LogRecord(recordNumb/recordInterval+1)
        recordNumb = 0

    else:
        recordButton.config(text="Currently recording")
        Initialtime = time.time()
        Record = True


def LoadRPM():
    global RPM
    RPM2.delete('all')
    RPMP = ( 1 - (RPM/RPMmax)) * RPMCH
    RPM2.create_rectangle(
    0, RPMP, RPMCW+1, RPMCH+1,
    outline="#fb0",
    fill="#fb0")
    RPM2.create_text(RPMCW/2, RPMP + 20, text= str(RPM), fill=fg, font=('Helvetica 30'))

def LoadSpeed():
    global RPM
    speed = "{:.2f}".format(RPM * 2 * 3.14159265 * radius * 60 / 1000)
    s1 = str(speed).split(".")
    speed1.config(text = s1[0])
    speed2.config(text = '.' + s1[1])

def Coffin():
    s = 2.9
    y = -68
    points = [GyroCW/2-12*s,GyroCH/2-27*s + y,GyroCW/2+12*s,GyroCH/2-27*s + y,GyroCW/2+25*s,GyroCH/2 + y,GyroCW/2+12*s,GyroCH/2+73*s + y,GyroCW/2-12*s,GyroCH/2+73*s + y,GyroCW/2-25*s,GyroCH/2 + y]
    Gyro1.create_polygon(points, outline = "blue", fill = fg, width = 2)
    Gyro1.create_text(GyroCW/2, GyroCH/2-21*s + y, text= "FRONT", fill=bg, font=('Helvetica 10'))

def LoadAcceleration(dA):
    global AAWE, AANS, Acceleration
    ErrorMargin = 0.01
    s = 6
    y = -155
    if  dA[accforward] >= -ErrorMargin and dA[accforward] <= ErrorMargin:
        Gyro1.delete(AANS[0],AANS[1],AANS[2])
        AANS = [        
            Gyro1.create_polygon([0,0], outline = bg, fill = bg, width = 0),
            Gyro1.create_text(0 , 0, text= ""),
            Gyro1.create_text(0 , 0, text= "")
        ]
    elif  dA[accforward] < -ErrorMargin: # Top
        if  Acceleration[accforward] >= -ErrorMargin:
            Gyro1.delete(AANS[0],AANS[2])
            points = [GyroCW/2, GyroCH/2 - 10 * s + y, GyroCW/2 + 10*s, GyroCH/2 + y, GyroCW/2 - 10*s, GyroCH/2 + y]
            AANS[0] = Gyro1.create_polygon(points, outline = bg, fill = fg, width = 0)
            AANS[2] = Gyro1.create_text(GyroCW/2 + 30 , GyroCH/2 + (y - 15), text= "g", fill=bg, font=('Helvetica 18'))
        Gyro1.delete(AANS[1])
        AANS[1] = Gyro1.create_text(GyroCW/2 , GyroCH/2 + (y - 20), text= -Acceleration[0], fill=bg, font=('Helvetica 17'))
    elif  dA[accforward] > ErrorMargin: # Bottom
        if  Acceleration[accforward] <= ErrorMargin:
            Gyro1.delete(AANS[0],AANS[2])
            points = [GyroCW/2, GyroCH/2 + 10 * s - y, GyroCW/2 + 10*s, GyroCH/2 - y, GyroCW/2 - 10*s, GyroCH/2 - y]
            AANS[0] = Gyro1.create_polygon(points, outline = bg, fill = fg, width = 0)
            AANS[2] = Gyro1.create_text(GyroCW/2 + 30 , GyroCH/2 - (y - 10), text= "g", fill=bg, font=('Helvetica 18'))
        Gyro1.delete(AANS[1])
        AANS[1] = Gyro1.create_text(GyroCW/2 , GyroCH/2 - (y - 20), text= Acceleration[0], fill=bg, font=('Helvetica 17'))
    s=30
    y=-90
    x=-82
    if  dA[accforward] >= -ErrorMargin and dA[accforward] <= ErrorMargin:
        Gyro1.delete(AAWE[0],AAWE[1],AAWE[2])
        AAWE = [        
            Gyro1.create_polygon([0,0], outline = bg, fill = bg, width = 0),
            Gyro1.create_text(0 , 0, text= ""),
            Gyro1.create_text(0 , 0, text= "")
        ]
    elif  dA[accforward] < -ErrorMargin:   # Left
        if  Acceleration[accforward] >= -ErrorMargin:
            Gyro1.delete(AAWE[0],AAWE[2])
            points = [GyroCW/2 + x, GyroCH/2 + 3*s + y, GyroCW/2 + x, GyroCH/2 + 6*s + y, GyroCW/2 - 2*s + x, GyroCH/2 + 3*s + y]
            AAWE[0] = Gyro1.create_polygon(points, outline = fg, fill = fg, width = 0)
            AAWE[2] = Gyro1.create_text(GyroCW/2 + (x - 12) , GyroCH/2 + 40, text = "g", fill=bg, font=('Helvetica 18'))
        Gyro1.delete(AAWE[1])
        AAWE[1] = Gyro1.create_text(GyroCW/2 + (x - 23) , GyroCH/2 + 20, text= -Acceleration[0], fill=bg, font=('Helvetica 16'))
    elif  dA[accforward] > ErrorMargin: # Right
        if  Acceleration[accforward] <= ErrorMargin:
            Gyro1.delete(AAWE[0],AAWE[2])
            points = [GyroCW/2 - x, GyroCH/2 + 3*s + y, GyroCW/2 - x, GyroCH/2 + 6*s + y, GyroCW/2 + 2*s - x, GyroCH/2 + 3*s + y]
            AAWE[0] = Gyro1.create_polygon(points, outline = fg, fill = fg, width = 0)
            AAWE[2] = Gyro1.create_text(GyroCW/2 - (x - 12) , GyroCH/2 + 40, text = "g", fill=bg, font=('Helvetica 18'))
        Gyro1.delete(AAWE[1])
        AAWE[1] = Gyro1.create_text(GyroCW/2 - (x - 23) , GyroCH/2 + 20, text= Acceleration[0], fill=bg, font=('Helvetica 16'))

    Acceleration = dA

def Loadrotation(dR):
    global RA, rotation
    ErrorMargin = 0.1
    s=30
    y=-90
    x=-82
    if  dR[rotYaw] >= -ErrorMargin and dR[rotYaw] <= ErrorMargin:
        Gyro1.delete(RA[0],RA[1],RA[2])
        RA = [        
            Gyro1.create_polygon([0,0], outline = bg, fill = bg, width = 0),
            Gyro1.create_text(0 , 0, text= ""),
            Gyro1.create_text(0 , 0, text= "")
        ]
    elif  dR[rotYaw] < -ErrorMargin:
        if  rotation[rotYaw] >= -ErrorMargin:
            Gyro1.delete(RA[0],RA[2])
            points = [GyroCW/2 + x, GyroCH/2 + y, GyroCW/2 + x, GyroCH/2 + 3*s + y, GyroCW/2 - 2*s + x, GyroCH/2 + 3*s + y]
            RA[0] = Gyro1.create_polygon(points, outline = fg, fill = fg, width = 0),
            RA[2] = Gyro1.create_text(GyroCW/2 + (x - 25) , GyroCH/2 - 10, text= "deg/s", fill=bg, font=('Helvetica 14'))
        Gyro1.delete(RA[1])
        RA[1] = Gyro1.create_text(GyroCW/2 + (x - 22) , GyroCH/2 - 30, text= -dR[rotYaw], fill=bg, font=('Helvetica 14'))
    elif  dR[rotYaw] > ErrorMargin:
        if  rotation[rotYaw] <= ErrorMargin:
            Gyro1.delete(RA[0],RA[2])
            points = [GyroCW/2 - x, GyroCH/2 + y, GyroCW/2 - x, GyroCH/2 + 3*s + y, GyroCW/2 + 2*s - x, GyroCH/2 + 3*s + y]
            RA[0] = Gyro1.create_polygon(points, outline = fg, fill = fg, width = 0),
            RA[2] = Gyro1.create_text(GyroCW/2 - (x - 25) , GyroCH/2 - 10, text= "deg/s", fill=bg, font=('Helvetica 14'))
        Gyro1.delete(RA[1])
        RA[1] = Gyro1.create_text(GyroCW/2 - (x - 22) , GyroCH/2 - 30, text=  dR[rotYaw], fill=bg, font=('Helvetica 14'))

    rotation = dR
    if Record:
        RecordData()

def Cross(colour):
        Gyro1.create_line(GyroCW/2, GyroCH/2 - 75, GyroCW/2, GyroCH/2 + 90, fill = colour, width = 3)
        x = 50
        y = -50
        Gyro1.create_line(GyroCW/2 - x, GyroCH/2 + y, GyroCW/2 + x, GyroCH/2 + y, fill = colour, width = 3)

def selectColour(value,value2 = 0):
    global Tmax
    if value >= Tmax or value2 >= Tmax:
        return "red"
    else:
        return "green"

def LoadTemp():
    global T01, T23, T4, Tmax, ts
    Gyro1.delete(T01,T23,T4)
    T01 = Gyro1.create_text(GyroCW/2 - 30, GyroCH/2 - 10, text= math.floor(max(Celecius[0],Celecius[1])), fill=selectColour(Celecius[0],Celecius[1]), font=('Helvetica 20'))
    T23 = Gyro1.create_text(GyroCW/2 + 30, GyroCH/2 - 10, text= math.floor(max(Celecius[2],Celecius[3])), fill=selectColour(Celecius[2],Celecius[3]), font=('Helvetica 20'))
    T4 = Gyro1.create_text(GyroCW/2 , GyroCH/2 - 95, text= Celecius[4], fill= selectColour(Celecius[4]), font=('Helvetica 20'))

    if max(Celecius) >= Tmax:
        if ts == "green":
            Cross("red")
            ts = "red"
    elif max(Celecius) <= Tmax:
        if ts == "red":
            Cross("green")
            ts = "green"

#Com port functions
def sendSerial():
    if serialObj.isOpen():
        message=sender.get()
        message=(message + "\n").encode()
        serialObj.write(message)

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
        # Label(dataFrame,text=recentPacketString, bg='white').pack(anchor=W)
        if recentPacketString[0]:
            checkValue = recentPacketString[0]
            if checkValue == "A":
                LoadAcceleration(ReadGyro(recentPacketString, Acceleration,"acceleration"))
            elif checkValue == "R":
                Loadrotation(ReadGyro(recentPacketString, rotation,"rotation"))
            elif checkValue == "S":
                global RPM
                RPM = float(recentPacketString[recentPacketString.find(":")+1:])
                LoadRPM()
                LoadSpeed()
            elif checkValue == "E":
                if recentPacketString[0+1] == "R":
                    ReadGyro(recentPacketString, Rerror)
                    print(Rerror)
                elif recentPacketString[0+1] == "A":
                    ReadGyro(recentPacketString, Aerror)
                    print(Aerror)
            elif checkValue.isdigit():
                # if (float(recentPacketString[recentPacketString.find(":")+1:])) :
                try:
                    Celecius[int(checkValue)] = float(recentPacketString[recentPacketString.find(":")+1:])
                except:
                    print("Faulty reading")
                if int(checkValue) == 0:
                    LoadTemp()

def convertRotation(message,index):
    return round((float(message) - Rerror[index]) / 3.14159265 * 180, 1)

def convertAcceleration(message,index):
    return round((float(message) - Aerror[index])/ 9.81, 2)

def ReadGyro(message,destination,type = " "):
    d = [0,0,0]
    X = message.find('X')
    Y = message.find('Y')
    Z = message.find("Z")
    if type == "rotation":
        d[0] = convertRotation(message[X+1:Y],0)
        d[1] = convertRotation(message[Y+1:Z],1)
        d[2] = convertRotation(message[Z+1:],2)
        return d
    elif type == "acceleration":
        d[0] = convertAcceleration(message[X+1:Y],0)
        d[1] = convertAcceleration(message[Y+1:Z],1)
        d[2] = convertAcceleration(message[Z+1:],2)
        return d
    destination[0] = float(message[X+1:Y])
    destination[1] = float(message[Y+1:Z])
    destination[2] = float(message[Z+1:])

#Com port variables
ports = serial.tools.list_ports.comports()
serialObj = serial.Serial()

#Display Window settings variables
bg = "black" # back ground colour
fg = "white" # for ground colour
ts = "green"
ColumnL = 0.15 # left column
ColumnC = 0.45 # center column
ColumnR = 0.81 # Right column
Options = FALSE


RPMCH = 300 # RPM canvas height
RPMCW = 100 # RPM canvas width
GyroCH = 450 # Gyro canvas height
GyroCW = 320 # Gyro canvas width

root = tk.Tk()
root.geometry("800x480")
root.title("arduino experimental window")
root.config(bg=bg)

#Vehcal settings and veriables
radius = 0.07 # radius of wheel
RPMmax = 3000
Tmax = 25.00
rotYaw = 2 # 0 is roll, 1 is pitch, 2 is yaw
accforward = 0
accsideways = 1
Rerror = [-0.04, 0.08, 0.09]
Aerror = [0, 0, 0]
recordInterval = 50

file = open('dryup.txt', 'r')
f = file.readlines()
for line in f:
    values = line.split('=')
    a = values[-len(values)].rstrip()
    x = values[len(values)-1]

    if a == "radius":
        radius = float(x)
    elif a == "RPMmax":
        RPM = int(x)
    elif a == "Tmax":
        Tmax = float(x)
    elif a == "rotYaw":
        rotYaw = int(x)
    elif a == "accforward":
        accforward = int(x)
    elif a == "rotYaw":
        accsideways = int(x)
    elif a == "Rerror":
        storage = x.lstrip(" [").rstrip("]\n").split(",")
        for index, item in enumerate(storage):
            Rerror[index] = float(item)
    elif a == "recordInterval":
        recordInterval = int(x)
file.close()

Celecius = [0,0,0,0,0] #sensor 1, 2, 3, 4 & Gyro Temperature
Acceleration = [0.0,0.0,0.0] #Acceleration forward & backward, sideways, downward
rotation = [0.0,0.0,0.0] #Rotation [roll, pitch, yal]
RPM = 0
Initialtime = time.time()
RecordCache = []
Record = False
recordNumb = 1



#Builds display:
#ColumnC
speed1 = Label(root, text = 0, height=1, width=2, bg=bg, fg=fg, font=('Arial','160'))
speed1.place(relx = ColumnC, rely = 0.4, anchor = CENTER)
speed2 = Label(root, text = 0, height=1, width=3, bg=bg, fg=fg, font=('Arial','30'))
speed2.place(relx = ColumnC, rely = 0.62, anchor = CENTER)
speed3 = Label(root, text = "km/h", height=1, width=12, bg=bg, fg=fg, font=('Arial','35')).place(relx = ColumnC, rely= 0.73, anchor = CENTER)

#ColumnL
RPM1 = Label(root, text = "RPM", height=1, width=12, bg=bg, fg=fg, font=('Arial','35')).place(relx = ColumnL, rely = 0.1, anchor = CENTER)
RPM2 = Canvas(
    root,
    height = RPMCH,
    width = RPMCW,
    bg = bg
    )
RPM2.place(relx = ColumnL, rely = 0.47, anchor = CENTER)
OptionB = Button(root, text='Options', height=1, width=6, bg=fg, fg=bg, font=('Arial','20'), command=functools.partial(OpenOptions)).place(relx = ColumnL, rely = 0.86, anchor = CENTER)

#ColumnR
Gyro1 = Canvas(
    root,
    height = GyroCH,
    width = GyroCW,
    bg = bg,
    bd = 0,
    highlightthickness = 0
    )
Gyro1.place(relx = ColumnR, rely = 0.5, anchor=CENTER)
Coffin()
Cross("green")
T01 = Gyro1.create_text(GyroCW/2 - 30, GyroCH/2 - 10, text= math.floor((Celecius[0] + Celecius[1])/2), fill="green", font=('Helvetica 20'))
T23 = Gyro1.create_text(GyroCW/2 + 30, GyroCH/2 - 10, text= math.floor((Celecius[2] + Celecius[3])/2), fill="green", font=('Helvetica 20'))
T4 = Gyro1.create_text(GyroCW/2 , GyroCH/2 - 95, text= Celecius[4], fill="green", font=('Helvetica 20'))
RA = [        
    Gyro1.create_polygon([0,0], outline = bg, fill = bg, width = 0),
    Gyro1.create_text(0 , 0, text= ""),
    Gyro1.create_text(0 , 0, text= "")
    ] #Rotation Arrows
AANS = [
    Gyro1.create_polygon([0,0], outline = bg, fill = bg, width = 0),
    Gyro1.create_text(0 , 0, text= ""),
    Gyro1.create_text(0 , 0, text= "")
    ] #Acceleration Arrows Verticle
AAWE= [
    Gyro1.create_polygon([0,0], outline = bg, fill = bg, width = 0),
    Gyro1.create_text(0 , 0, text= ""),
    Gyro1.create_text(0 , 0, text= "")
]

#Comport selecter
optionsWrapper = Frame(root)
portWrapper = Frame(optionsWrapper)
sendWrapper = Frame(optionsWrapper)
recordWrapper = Frame(optionsWrapper)
recordWrapper.grid(row=0,column=0)
recordButton = Button(recordWrapper, text="Not Recording", command = RecordSwitch)
recordButton.grid(row=1, column=0)
sender = Entry(sendWrapper, width=20)
portWrapper.grid(row=2, column=0)
for comPort in ports:
    comButton = Button(portWrapper, text=comPort, height=1, width= 40, command = functools.partial(selectComPort, index=ports.index(comPort))).grid(row=ports.index(comPort),column=0)
sendWrapper.grid(row=1,column=0)
sender.grid(row=0,column=0)
senderBtn = Button(sendWrapper, text='send command', command=functools.partial(sendSerial)).grid(row=0, column=1)


while True:
    checkSerialPort()
    root.update()