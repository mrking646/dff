



import pandas as pd
from typing import Dict, List
# Load the ODS file
file_path = 'NFET_LUT.ods'  # Update this path with your local file path
df = pd.read_excel(file_path, engine='odf')

# Dictionaries for mapping width, length, and FET type
width_dict = {'A': '0p08', 'B': '0p17', 'C': '0p3', 'D': '0p5', 'E': '0p8', 'F': '1', 'G': '1p5', 'H': '2'}
length_dict = {'a': '0p02', 'b': '0p024', 'c': '0p04', 'd': '0p07', 'e': '0p1', 'f': '0p5', 'g': '1', 'h': '8'}
fet_type_dict = { '1': 'nfet', '2': 'hvtnfet', '3': 'lvtnfet', '4': 'slvtnfet', '5': 'pfet ', '6': 'hvtpfet', '7': 'lvtpfet', '8': 'slvtpfet'}



def get_all_duts(LUT_path) -> Dict[str, List]:
# Initialize an empty dictionary to store the DUT data
    dut_data = {}

    # Iterate through the columns, skipping every other column for DUT names
    for col in df.columns[::2]:  # Select every other column for DUT names
        for i, dut_name in enumerate(df[col]):
            if dut_name != 'Blank':  # Skip 'Blank' entries
                # Parse the DUT name (first letter for width, second for length, third for fet type)
                ref1 = dut_name[0]
                ref2 = dut_name[1]
                ref3 = dut_name[2]
                
                # Fetch the width, length, and fet type from dictionaries
                width = width_dict.get(ref1, 'Unknown')
                length = length_dict.get(ref2, 'Unknown')
                fet_type = fet_type_dict.get(ref3, 'Unknown')

                # Get the cycle value from the next column
                cycle_value = df.iloc[i, df.columns.get_loc(col) + 1]
                
                # Build description string
                description = f"W_{width}_L_{length}_{fet_type}"
                
                # Add the DUT data to the dictionary
                dut_data[cycle_value] = [dut_name, description]
    return dut_data


