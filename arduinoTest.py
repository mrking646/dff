from pymeasure.instruments.agilent import AgilentB1500
import pandas as pd
from readLUT import get_all_duts
import serial
import time 
from config import Config

config = Config()
arduino = serial.Serial(port=config.arduino_port, baudrate=9600, timeout=.1) # Adjust the port as needed
arduino.flushInput()  # Clear the input buffer before starting communication
class EmptyDataError(Exception):
    """Custom exception for empty data received."""
    pass
time.sleep(2)

def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(1)
    data = arduino.readline()
    if not data:  # If data is empty or None
        raise EmptyDataError(f"No data received for cycle value: {x}")
    print(data)
    return data

duts = get_all_duts(config.LUT_path) #get all DUTs from LUT file


for dut_name, (cycle_value, description) in duts.items():
    print(f"DUT Name: {dut_name}")
    print(f"  Cycle Value: {cycle_value}")
    print(f"  Description: {description}")
    write_read(str(cycle_value))  # Send DUT na