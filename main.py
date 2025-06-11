from pymeasure.instruments.agilent import AgilentB1500
import pandas as pd
from readLUT import get_all_duts
import serial
import time 
from config import Config



class EmptyDataError(Exception):
    """Custom exception for empty data received."""
    pass

config = Config()
arduino = serial.Serial(port=config.arduino_port, baudrate=9600, timeout=.1) # Adjust the port as needed

time.sleep(2)  # Allow time for the Arduino to reset and initialize
def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(1)
    data = arduino.readline()
    if not data:  # If data is empty or None
        raise EmptyDataError(f"No data received for cycle value: {x}")
    return data

duts = get_all_duts(config.LUT_path) #get all DUTs from LUT file


# explicitly define r/w terminations; set sufficiently large timeout in milliseconds or None.
b1500=AgilentB1500(config.B1500_address, read_termination='\r\n', write_termination='\r\n', timeout=600000)
# query SMU config from instrument and initialize all SMU instances
b1500.initialize_all_smus()
# set data output format (required!)
b1500.data_format(21, mode=1) #call after SMUs are initialized to get names for the channels

# choose measurement mode
b1500.meas_mode('STAIRCASE_SWEEP', *b1500.smu_references) #order in smu_references determines order of measurement
for smu in b1500.smu_references:
    smu.enable() #enable SMU
    smu.adc_type = 'HRADC' #set ADC to high-resoultion ADC
    smu.meas_range_current = 0
    smu.meas_op_mode = 'COMPLIANCE_SIDE' # other choices: Current, Voltage, FORCE_SIDE, COMPLIANCE_AND_FORCE_SIDE

# General Instrument Settings
# b1500.adc_averaging = 1
# b1500.adc_auto_zero = True
b1500.adc_setup('HRADC','AUTO',6)
#b1500.adc_setup('HRADC','PLC',1)

#Sweep Settings
b1500.sweep_timing(config.SweepTiming.hold, config.SweepTiming.delay, step_delay=config.SweepTiming.step_delay) #hold,delay
b1500.sweep_auto_abort(False,post='STOP') #disable auto abort, set post measurement output condition to stop value of sweep
# Sweep Source




def idVg(logname, config:Config, mosType=None):
    # settings for individual SMUs

 


    b1500.smu1.staircase_sweep_source(config.GateForce.type, config.GateForce.mode, config.GateForce.range, config.GateForce.start, config.GateForce.stop, config.GateForce.steps, config.GateForce.compliance)
    # Synchronous Sweep Source
    
    #the following are const ports, at your disposal
    b1500.smu2.force(config.DrainForce.type, config.DrainForce.range, config.DrainForce.outputLevel, config.DrainForce.compliance) #force drain voltage

    b1500.smu3.force(config.SourceForce.type, config.SourceForce.range, config.SourceForce.outputLevel, config.SourceForce.compliance) #force source voltage
    b1500.smu4.force(config.BulkForce.type, config.BulkForce.range, config.BulkForce.outputLevel, config.BulkForce.compliance) #force bulk voltage
    #Start Measurement
    b1500.check_errors()
    b1500.clear_buffer()
    b1500.clear_timer()
    b1500.send_trigger()

    # read measurement data all at once
    b1500.check_idle() #wait until measurement is finished
    data = b1500.read_data(config.GateForce.steps) #Factor 2 because of double sweep
    for smu in b1500.smu_references:
        smu.disable() #disable SMU after measurement
    
    #dataframe to csv
    df = pd.DataFrame(data)

    df.to_csv(logname, index=False)


def main():
    for dut_name, (cycle_value, description) in duts.items():
        print(f"DUT Name: {dut_name}")
        print(f"  Cycle Value: {cycle_value}")
        print(f"  Description: {description}")
        write_read(str(cycle_value))  # Send DUT name to Arduino
        idVg(logname=f'{cycle_value}_{description}.csv', config=config)

if __name__ == "__main__":
    main() 