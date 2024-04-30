#!/usr/bin/env python3
from typing import TypeAlias, Generator
from spec import NES_SPEC
import argparse, re, sys, io, os.path

TOK_DELIMITER = ' '
LINE_DELIMITER = '\n'
INSTR_DELIMITER = ','
COMMENT_START = '#'

ByteGenerator: TypeAlias = Generator[int, None, None]

def translate(addrmode: str) -> str:
    # it's just a script, doesn't have to be nice.
    if addrmode == "I":
        addrmode = "IMMED"
    elif addrmode == "Z":
        addrmode = "ZPAGE"
    elif addrmode == "ZX":
        addrmode = "ZPAGE_X"
    elif addrmode == "A":
        addrmode = "ABS"
    elif addrmode == "AX":
        addrmode = "ABS_X"
    elif addrmode == "AY":
        addrmode = "ABS_Y"
    elif addrmode == "IX":
        addrmode = "INDIR_X"
    elif addrmode == "IY":
        addrmode = "INDIR_Y"
    elif addrmode == "ACM":
        addrmode = "ACCUM"
    elif addrmode == "R":
        addrmode = "REL"
    elif addrmode == "IL":
        addrmode = "IMPL"
    return addrmode

# taken from https://stackoverflow.com/a/9773142
def isplit(source, sep=None, regex=False):
    """
    generator version of str.split()

    :param source:
        source string (unicode or bytes)

    :param sep:
        separator to split on.

    :param regex:
        if True, will treat sep as regular expression.

    :returns:
        generator yielding elements of string.
    """
    if sep is None:
        # mimic default python behavior
        source = source.strip()
        sep = "\\s+"
        if isinstance(source, bytes):
            sep = sep.encode("ascii")
        regex = True
    if regex:
        # version using re.finditer()
        if not hasattr(sep, "finditer"):
            sep = re.compile(sep)
        start = 0
        for m in sep.finditer(source):
            idx = m.start()
            assert idx >= start
            yield source[start:idx]
            start = m.end()
        yield source[start:]
    else:
        # version using str.find(), less overhead than re.finditer()
        sepsize = len(sep)
        start = 0
        while True:
            idx = source.find(sep, start)
            if idx == -1:
                yield source[start:]
                return
            yield source[start:idx]
            start = idx + sepsize

def next_byte(line: str, verbose) -> ByteGenerator:
    """
    Given a UASM source string line, returns a generator
    iterating over the corresponding component bytes.
    """
    if len(line) == 0 or line[0] == COMMENT_START:
        return
    
    tok_gen = isplit(line, TOK_DELIMITER)
    head_tok = next(tok_gen)

    spl = head_tok.split(INSTR_DELIMITER)
    if len(spl) == 1:
        instr = spl[0].strip()
        addrmode = None
    else:
        # error handling
        try:
            instr, addrmode = spl
        except ValueError:
            sys.stderr.write(f"Token: {head_tok} incorrect format\n")
            sys.exit(1)
        if instr not in NES_SPEC:
            sys.stderr.write(f"Unrecognized instruction {instr}\n")
            sys.exit(1)
    
    # mnemonics for common addressing modes
    if addrmode is not None:
        addrmode = translate(addrmode)
    
    instr_mapping = NES_SPEC[instr]
    if addrmode is None and len(instr_mapping) == 1:
        addrmode = next(iter(instr_mapping.keys()))

    if addrmode not in instr_mapping:
        sys.stderr.write(f"Unrecognized address mode \"{addrmode}\" "
                            f"for instruction \"{instr}\"\n")
        sys.exit(1)
    
    head_tok = head_tok.strip()
    if verbose:
        print(f"\tToken {head_tok} -> ", end="")
    
    yield instr_mapping[addrmode]

    for tok in tok_gen:
        tok = tok.strip()
        if len(tok) == 0:
            continue
        if tok[0] == COMMENT_START:
            return
        if verbose:
            print(f"\tToken {tok} -> ", end="")
        yield int(tok, base=16)

def fnext_byte(srcfile: io.TextIOWrapper, verbose: bool) -> ByteGenerator:
    for lineno, line in enumerate(srcfile):
        if verbose:
            print(f"Parsing next line: line {lineno}: {line.strip()}")
        for byte in next_byte(line, verbose):
            if byte >= 256 or byte < 0:
                sys.stderr.write("Invalid spec!\n")
                sys.exit(1)
            yield byte.to_bytes()
            
def assemble(fname: str, out: str, verbose: bool):
    with open(fname, "r") as content, open(out, "wb") as bytecode:
        for byte in fnext_byte(content, verbose):
            if verbose:
                print(f"byte {hex(int.from_bytes(byte))}")
            bytecode.write(byte)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
            description="Assembles a .uasm file into an executable .nes binary.",
            epilog="Helper script written by me!")

    # verbose mode
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Prints a lot of extra information."
    )

    # path of .uasm file to assemble
    parser.add_argument(
        "filepath", help="Path of the .uasm file to assemble."
    )

    args = parser.parse_args()

    uasm_file = args.filepath
    verbose = args.verbose

    o_file = list(os.path.splitext(uasm_file)[:-1])
    o_file.append(".nes")
    out_file = "".join(o_file)

    assemble(uasm_file, out_file, verbose)