class Config:
    def __init__(self):
        self.arduino_port = "COM4"
        self.B1500_address = "GPIB0::18::INSTR"
        self.LUT_path = 'NFET_LUT.ods'  # Path to the Look-Up Table file

        self.DrainForce = self.ForceSource(
            type='VOLTAGE',
            range=0,
            outputLevel=0.0,
            compliance=1e-3
        )  # Default drain force voltage

        self.SourceForce = self.ForceSource(
            type='VOLTAGE',
            range=0,
            outputLevel=0.0,
            compliance=1e-3
        )  # Default source force voltage

        self.BulkForce = self.ForceSource(
            type='VOLTAGE',  # Type of force source
            range=0,  # Range of the force source
            outputLevel=0.0,  # Default output level for bulk force
            compliance=1e-3  # Compliance current for bulk force
        )

        self.GateForce = self.SweepSource(
            type='VOLTAGE',  # Type of sweep source
            mode='LINEAR_SINGLE',  # Mode of sweep
            range='Auto Ranging',  # Range of the sweep
            start=-0.5,  # Start voltage
            stop=1.8,  # Stop voltage
            steps=116,  # Number of steps in the sweep
            compliance=1e-3  # Compliance current
        )

        self.sweepTimingSet = self.SweepTiming(
            hold = 0.5,
            delay = 0.2,
            step_delay=0.5,
        )


    class SweepSource:
        def __init__(self, type, mode, range, start, stop, steps, compliance):
            self.type = type
            self.mode = mode
            self.range = range
            self.start = start
            self.stop = stop
            self.steps = steps
            self.compliance = compliance

    class SweepTiming:
        def __init__(self, hold, delay, step_delay):
            self.hold = hold
            self.delay = delay
            self.step_delay = step_delay

    class ForceSource:
        def __init__(self, type, range, outputLevel, compliance):
            self.type = type
            self.range = range
            self.outputLevel = outputLevel
            self.compliance = compliance
            
    
