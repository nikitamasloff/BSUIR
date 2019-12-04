from sys import argv
from bitstring import Bits
from random import randint


# CONSTANTS region

MODE_HAMMING = "Hamming"
MODE_CRC = "CRC"

ENCODING = 'utf-8'

# end of region


# COMMAND LINE ARGUMENTS region

if len(argv) < 2:
    print("One argument should be provided: error protection mode {Hamming, CRC}")
    exit(1)

mode = argv[1]

if not (mode == MODE_HAMMING or mode == MODE_CRC):
    print("Invalid error protection mode")
    exit(1)

# end of region


# FUNCTIONS region

def to_hamming(data):
    if len(data) == 0:
        return ""
    intdata = [int(i) for i in data]
    buf = [None] * 1024                         # max buffer == 1024
    parity_indices = [2**i for i in range(11)]  # max power of 2 == 11
    j = 0
    for i in range(len(buf)):
        tmp = i + 1
        if tmp & (tmp - 1):                     # check whether i + 1 is power of 2
            # todo


def to_crc(data):
    pass

def check_hamming(data):
    pass

def check_crc(data):
    pass

def correct_hamming(data):
    pass

def corrent_crc(data):
    pass

# end of region


# MAIN region

inp = input("# Input source: ").encode(ENCODING)
src = Bits(bytes=inp).bin
if len(src) > 1000:
    print("Source is too large")
    exit(2)
print("# ORIGINAL SOURCE = " + src)


if mode == MODE_HAMMING:
    protctd_src = to_hamming(src)
elif mode == MODE_CRC:
    protctd_src = to_crc(src)
print("# PROTECTED SOURCE = " + protctd_src)


is_invalidate = input("# Invalidate a bit?  Y/N")
if (is_invalidate == "Y" or is_invalidate == 'y') and len(src) != 0:
    bad_bit = randint(0, len(src) - 1)
    cur = src[bad_bit]
    new = '1' if cur == '0' else '0'
    src = src[:bad_bit] + new + src[bad_bit + 1:]
    print("# UPDATED SOURCE = " + src)

# end of region
