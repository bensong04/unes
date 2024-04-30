NES_SPEC = {   
    "ADC": {
        "IMMED" : "105",
        "ZPAGE" : "101",
        "ZPAGE_X" : "117",
        "ABS" : "109",
        "ABS_X" : "125",
        "ABS_Y" : "121",
        "INDIR_X" : "97",
        "INDIR_Y" : "113",
    },
    "AND": {
        "IMMED" : "41",
        "ZPAGE" : "37",
        "ZPAGE_X" : "53",
        "ABS" : "45",
        "ABS_X" : "61",
        "ABS_Y" : "57",
        "INDIR_X" : "33",
        "INDIR_Y" : "49",
    },
    "ASL": {
        "ACCUM" : "10",
        "ZPAGE" : "6",
        "ZPAGE_X" : "22",
        "ABS" : "14",
        "ABS_X" : "30",
    },
    "BCC": {
        "REL" : "144",
    },
    "BCS": {
        "REL" : "176",
    },
    "BEQ": {
        "REL" : "240",
    },
    "BIT": {
        "ZPAGE" : "36",
        "ABS" : "44",
    },
    "BMI": {
        "REL" : "48",
    },
    "BNE": {
        "REL" : "208",
    },
    "BPL": {
        "REL" : "16",
    },
    "BRK": {
        "IMPL" : "0",
    },
    "BVC": {
        "REL" : "80",
    },
    "CLC": {
        "IMPL" : "24",
    },
    "CLD": {
        "IMPL" : "216",
    },
    "CLI": {
        "IMPL" : "88",
    },
    "CLV": {
        "IMPL" : "184",
    },
    "CMP": {
        "IMMED" : "201",
        "ZPAGE" : "197",
        "ZPAGE_X" : "213",
        "ABS" : "205",
        "ABS_X" : "221",
        "ABS_Y" : "217",
        "INDIR_X" : "193",
        "INDIR_Y" : "209",
    },
    "CPX": {
        "IMMED" : "224",
        "ZPAGE" : "228",
        "ABS" : "236",
    },
    "CPY": {
        "IMMED" : "192",
        "ZPAGE" : "196",
        "ABS" : "204",
    },
    "DEC": {
        "ZPAGE" : "198",
        "ZPAGE_X" : "214",
        "ABS" : "206",
        "ABS_X" : "222",
    },
    "DEX": {
        "IMPL" : "202",
    },
    "DEY": {
        "IMPL" : "136",
    },
    "EOR": {
        "IMMED" : "73",
        "ZPAGE" : "69",
        "ZPAGE_X" : "85",
        "ABS" : "77",
        "ABS_X" : "93",
        "ABS_Y" : "89",
        "INDIR_X" : "65",
        "INDIR_Y" : "81",
    },
    "INC": {
        "ZPAGE" : "230",
        "ZPAGE_X" : "246",
        "ABS" : "238",
        "ABS_X" : "254",
    },
    "INX": {
        "IMPL" : "232",
    },
    "INY": {
        "IMPL" : "200",
    },
    "JMP": {
        "ABS" : "76",
        "INDIR" : "108",
    },
    "JSR": {
        "ABS" : "32",
    },
    "LDA": {
        "IMMED" : "169",
        "ZPAGE" : "165",
        "ZPAGE_X" : "181",
        "ABS" : "173",
        "ABS_X" : "189",
        "ABS_Y" : "185",
        "INDIR_X" : "161",
        "INDIR_Y" : "177",
    },
    "LDX": {
        "IMMED" : "162",
        "ZPAGE" : "166",
        "ZPAGE_Y" : "182",
        "ABS" : "174",
        "ABS_Y" : "190",
    },
    "LDY": {
        "IMMED" : "160",
        "ZPAGE" : "164",
        "ZPAGE_X" : "180",
        "ABS" : "172",
        "ABS_X" : "188",
    },
    "LSR": {
        "ACCUM" : "74",
        "ZPAGE" : "70",
        "ZPAGE_X" : "86",
        "ABS" : "78",
        "ABS_X" : "94",
    },
    "NOP": {
        "IMPL" : "234",
    },
    "ORA": {
        "IMMED" : "9",
        "ZPAGE" : "5",
        "ZPAGE_X" : "21",
        "ABS" : "13",
        "ABS_X" : "29",
        "ABS_Y" : "25",
        "INDIR_X" : "1",
        "INDIR_Y" : "17",
    },
    "PHA": {
        "IMPL" : "72",
    },
    "PHP": {
        "IMPL" : "8",
    },
    "PLA": {
        "IMPL" : "104",
    },
    "PLP": {
        "IMPL" : "40",
    },
    "ROL": {
        "ACCUM" : "42",
        "ZPAGE" : "38",
        "ZPAGE_X" : "54",
        "ABS" : "46",
        "ABS_X" : "62",
    },
    "ROR": {
        "ACCUM" : "106",
        "ZPAGE" : "102",
        "ZPAGE_X" : "118",
        "ABS" : "110",
        "ABS_X" : "126",
    },
    "RTI": {
        "IMPL" : "64",
    },
    "RTS": {
        "IMPL" : "96",
    },
    "SBC": {
        "IMMED" : "233",
        "ZPAGE" : "229",
        "ZPAGE_X" : "245",
        "ABS" : "237",
        "ABS_X" : "253",
        "ABS_Y" : "249",
        "INDIR_X" : "225",
        "INDIR_Y" : "241",
    },
    "SEC": {
        "IMPL" : "56",
    },
    "SED": {
        "IMPL" : "248",
    },
    "SEI": {
        "IMPL" : "120",
    },
    "STA": {
        "ZPAGE" : "133",
        "ZPAGE_X" : "149",
        "ABS" : "141",
        "ABS_X" : "157",
        "ABS_Y" : "153",
        "INDIR_X" : "129",
        "INDIR_Y" : "145",
    },
    "STX": {
        "ZPAGE" : "134",
        "ZPAGE_Y" : "150",
        "ABS" : "142",
    },
    "STY": {
        "ZPAGE" : "132",
        "ZPAGE_X" : "148",
        "ABS" : "140",
    },
    "TAX": {
        "IMPL" : "170",
    },
    "TAY": {
        "IMPL" : "168",
    },
    "TSX": {
        "IMPL" : "184",
    },
    "TXA": {
        "IMPL" : "138",
    },
    "TXS": {
        "IMPL" : "154",
    },
    "TYA": {
        "IMPL" : "152",
    },
}