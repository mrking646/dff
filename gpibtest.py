import pyvisa

rm = pyvisa.ResourceManager()
resources = rm.list_resources()
print("Available resources:", resources)

# Try opening the GPIB device explicitly by using the GPIB address
inst = rm.open_resource('GPIB0::18::INSTR')  # Change to the correct address if necessary
print(inst.query("*IDN?"))  # Example command to query the device
