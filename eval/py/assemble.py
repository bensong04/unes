from typing import TypeAlias, Generator
from spec import NES_SPEC
import argparse, re, sys

TOK_DELIMITER = ' '
INSTR_DELIMITER = ','

ByteGenerator: TypeAlias = Generator[int, None, None]

parser = argparse.ArgumentParser(
            description="Assembles a .uasm file into an executable .nes binary.",
            epilog="Helper script written by me!")

# info message
parser.add_argument(
    "-h", "--help", action="help", help="Prints this message."
)

# verbose mode
parser.add_argument(
    "-v", "--verbose", action="store_true", help="Prints a lot of extra information."
)

# path of .uasm file to assemble
parser.add_argument(
    "filepath", help="Path of the .uasm file to assemble."
)

args = parser.parse_args()

UASM_FILE = args.filepath
VERBOSE = args.verbose

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

def next_byte(src: str) -> ByteGenerator:
    """
    Given a UASM source string, returns a generator
    iterating over the corresponding component bytes.
    """
    idx: int = 0
    for tok in isplit(src, TOK_DELIMITER):
        try:
            instr, addrmode = src.split(INSTR_DELIMITER)
        except ValueError:
            sys.stderr.write(f"Token: {tok} incorrect format")
            sys.exit(1)
        # mnemonics for common addressing modes
        if addrmode == "I":
            addrmode = "IMMED"
        elif addrmode == "Z":
            addrmode = "ZPAGE"
        elif addrmode == "ZX":
            addrmode = "ZPAGE_X"
        elif addrmode == "A":
            addrmode = "ABS"
        
        