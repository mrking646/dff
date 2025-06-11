1. run "pip install odfpy" in cmd
2. write your measurement conditions in config.py
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
            mode='STAIRCASE',  # Mode of sweep
            range='Auto Ranging',  # Range of the sweep
            start=-0.5,  # Start voltage
            stop=1.8,  # Stop voltage
            steps=116,  # Number of steps in the sweep
            compliance=1e-3  # Compliance current
        )

3. in main.py, change the sequence of SMU as same as the physical sequence you connected:

    b1500.smu1.staircase_sweep_source(config.GateForce.type, config.GateForce.mode, config.GateForce.range, config.GateForce.start, config.GateForce.stop, config.GateForce.steps, config.GateForce.compliance)
    # Synchronous Sweep Source
    
    #the following are const ports, at your disposal
    b1500.smu2.force(config.DrainForce.type, config.DrainForce.range, config.DrainForce.outputLevel, config.DrainForce.compliance) #force drain voltage

    b1500.smu3.force(config.SourceForce.type, config.SourceForce.range, config.SourceForce.outputLevel, config.SourceForce.compliance) #force source voltage
    b1500.smu4.force(config.BulkForce.type, config.BulkForce.range, config.BulkForce.outputLevel, config.BulkForce.compliance) #force bulk voltage
    #Start Measurement

    
4. run "python main.py" in cmd, you will see new datalog file showed up, if no, debug.