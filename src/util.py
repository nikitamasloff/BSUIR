from bitstring import Bits
from functools import reduce
import random
from string import ascii_lowercase

# Converts passed int to string of bits ('0' or '1') leaving leading '0'.
def int_to_bitstr(data):
    return '{:08b}'.format(data)

# Converts passed bytes to string of bits ('0' or '1') leaving leading '0'.
def bytes_to_bitstr(data):
    return ''.join(map(int_to_bitstr, data))

# Converts string of bits ('0' or '1') to bytes.
def bitstr_to_bytes(data):
    return Bits(bin = data).tobytes()

# Replaces all paired values in string.
def mulreplace(src, *pairs):
    return reduce(lambda fr, to: fr.replace(*to), pairs, src)

def random_str(len = 10):
    """Generate a random string of fixed length """
    return ''.join(random.choice(ascii_lowercase) for i in range(len))
