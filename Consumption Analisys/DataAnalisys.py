
import numpy as np
import matplotlib.pyplot as plt
import datetime
import pandas as pd

#Open file with data and store the lines into an array
file = open("RESIDENCIAL_C_HORA.CSV", 'r')
text = file.readlines()
numLines = len(text)
print("Number of Lines:",numLines)

#Generate arrays for receiving the data
Pressure = []
Flow = []
DateStamp = []
TimeStamp = []
Serial = []
timeTmp =[]
dateLeakage = []
#Create variable for counting consumption during night
numLeakage = 0
print("Split")

#Split file read lines storing into corresponding variables
for i in range(0, numLines -1):
    tmp = text[i].split(',')
    time = datetime.datetime.strptime(tmp[0],"%Y-%m-%d %H:%M:%S")
    DateStamp.append(time.strftime("%Y-%m-%d"))
    TimeStamp.append(time.strftime("%H"))
    Flow.append(float(tmp[1]))

statusLeakage = 0
first = True

#Search for continous consumption
for i in range(0, numLines -1):
    time = int(TimeStamp[i])
    if  0 <= time <= 6:
        #If time between 0h and 6h 
        if time == 0:
        #Idenfify as new day and reset intervals with leakage counter
            statusLeakage = 0
        if Flow[i] > 0:
        #In case of consumption increase the counter
            statusLeakage = statusLeakage+1
        else:
        #In case of no consumption reset the counter
            statusLeakage = 0
        if statusLeakage == 6:
        #If reach 6h of continous consumption, store the data when it happened
            print(DateStamp[i])
            dateLeakage.append(DateStamp[i])
            numLeakage = numLeakage + 1
            print("ACHOU")
        
#Stores in a file the days with leakage occurence probably
print("Generate File")
with open ("RESIDENCIAL_C_VAZAMENTOS.csv","+a") as file :
    for i in range(0, numLeakage ):
        to_write = str( dateLeakage[i] + "\n")
        file.write(to_write)

           
print("END")