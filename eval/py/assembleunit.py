#!/usr/bin/env python3
import json
import struct
import sys
from pathlib import Path

def parse_json_to_binary(json_filename, binary_filename):
    # Open the JSON file and load its content
    with open(json_filename, 'r') as file:
        data = json.load(file)

    data = data[int(sys.argv[2])]

    initial = data['initial']
    final = data['final']
    cycles = data['cycles']

    # Prepare the number of cycles as the first byte
    num_cycles = len(cycles)
    
    # Extract data from the "initial" object
    PC = initial['pc']
    s = initial['s']
    a = initial['a']
    x = initial['x']
    y = initial['y']
    p = initial['p']
    ram = initial['ram']

    PC_expected = final['pc']
    s_expected = final['s']
    a_expected = final['a']
    x_expected = final['x']
    y_expected = final['y']
    p_expected = final['p']
    ram_expected = final['ram']

    # Open a binary file for writing
    with open(binary_filename, 'wb') as bin_file:
        bin_file.write(struct.pack('<BH', num_cycles, len(ram)))
        
        # Write PC and p as two bytes each, and s, a, x, y as one byte each
        bin_file.write(struct.pack('<HHBBBBBBBBHH', PC, PC_expected,
                                                   a, a_expected,
                                                   x, x_expected,
                                                   y, y_expected,
                                                   p, p_expected,
                                                   s, s_expected))
        
        # Write each ram entry as two bytes for the location and one byte for the value
        for mem, mem_expect in zip(ram, ram_expected):
            loc, val = mem
            loc_exp, val_exp = mem_expect
            bin_file.write(struct.pack('<HHHH', loc, loc_exp, val, val_exp))

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Incorrect usage (placeholder).")
        exit(0)
    json_filename = Path(sys.argv[1])  # The input JSON filename
    if len(sys.argv) < 4:
        mv_filename = json_filename.parent.parent / (f"unit/{json_filename.name}")  
        binary_filename = mv_filename.with_suffix(".uunit")
    else:
        binary_filename = sys.argv[3]
    parse_json_to_binary(json_filename, binary_filename)
    print(binary_filename)
