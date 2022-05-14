# TrackAir Data Logger
# 
"""
Receive data collected from Arduino Opla using the Serial port.
Input (str): A string of numbers separated by comma.
Output (csv): A csv data file of all the sample collected in a given sample count.  
"""

import serial
import datetime

today_date = datetime.date.today()

arduino_port = "/dev/cu.usbmodem21301" #Serial port of Arduino
baud = 115200 # Arduino Opla runs at 115200 baud
fileName = "/Users/hongjunwu/Documents/GitHub/trackair-opla/trackair-opla/demo_data_collection.csv" #name of the CSV file generated

ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)
file = open(fileName, "a")
print("Created file")

samples = 100 # Number of samples to collect.
print_labels = False
line = 0 #start at 0 because our header is 0 (not real data)
while line <= samples:
    if print_labels:
        if line==0:
            print("Printing Column Headers")
        else:
            print("Line " + str(line) + ": writing...")

    if (line == 0):
        data = "Temperature, Humidity, Air_Pressure, Light_Intensity, Toxic_Chemical_Level, TVOC_Level, eCO2_Level, H2_Level, Ethanol_Level"
    else:
        getData=str(ser.readline())
        data=getData[2:-3][:-2]
    
    print(data)

    file = open(fileName, "a")
    file.write(data + "\n") #write data with a newline
    line = line+1

print("Data collection complete!")
file.close()

