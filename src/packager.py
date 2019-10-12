from bitstring import Bits
from util import int_to_bitstr, bytes_to_bitstr, bitstr_to_bytes, mulreplace

BIT_STFFNG = 1
BYTE_STFFNG = 2

class Packager:

    PCKG_FLAG = 0x7E
    ESCP_FLAG = 0x7D

    STFFNG_TYPES = (BIT_STFFNG, BYTE_STFFNG)

    def __init__(self,
                 port,
                 stffng_type = BIT_STFFNG):
        assert stffng_type in self.STFFNG_TYPES
        self._stffng_type = stffng_type
        self._port = port

    def read_pckg(self):
        BYTES_PER_READ = 1
        pckg = bytes()

        self._port.open()
        while self._count_pckg_flag(pckg) < 2:
            buf = self._port.read(size = BYTES_PER_READ)
            pckg += buf
        self._port.close()

        if self._stffng_type == BIT_STFFNG:
            data = self._unpack_bit_stffd(pckg)
        elif self._stffng_type == BYTE_STFFNG:
            data = self._unpack_byte_stffd(pckg)
        return data

    def write_pckg(self, data):
        if self._stffng_type == BIT_STFFNG:
            pckg = self._pack_bit_stffd(data)
        elif self._stffng_type == BYTE_STFFNG:
            pckg = self._pack_byte_stffd(data)
        self._port.open()
        self._port.write(pckg)
        self._port.close()

    # Packs passed bytes.
    # Returns stuffed with bits package (bytes).
    def _pack_bit_stffd(self, data):
        if len(data) == 0: return bytes()

        flag = int_to_bitstr(self.PCKG_FLAG)
        bitstr = bytes_to_bitstr(data)

        acc = [] # Accumulator
        cntr = 0 # Counter
        for bit in bitstr:
            acc.append(bit)
            cntr = cntr + 1 if bit == '1' else 0
            if cntr == 5:
                acc.append('0')
                cntr = 0

        pckg = flag + ''.join(acc) + flag
        return bitstr_to_bytes(pckg)

    # Unpacks passed bytes.
    # Returns unstuffed from bits package (bytes).
    def _unpack_bit_stffd(self, data):
        if len(data) <= 2: return bytes()

        flag = int_to_bitstr(self.PCKG_FLAG)
        bitstr = bytes_to_bitstr(data)

        # Check whether package has distinct borders.
        beg = bitstr.find(flag) + len(flag)
        if beg == -1: return
        end = bitstr[beg:].find(flag)
        if end == -1: return
        end += beg

        acc = [] # Accumulator
        cntr = 0 # Counter
        for bit in bitstr[beg:end]:
            if cntr == 5:
                cntr = 0
                continue
            acc.append(bit)
            cntr = cntr + 1 if bit == '1' else 0

        return bitstr_to_bytes(''.join(acc))

    # Packs passed bytes.
    # Returns stuffed with bytes package (bytes).
    def _pack_byte_stffd(self, data):
        if len(data) == 0: return bytes()

        flag = int_to_bitstr(self.PCKG_FLAG)
        escp = int_to_bitstr(self.ESCP_FLAG)
        bitstr = bytes_to_bitstr(data)

        repl1 = (flag, escp + flag)
        repl2 = (escp, escp + escp)

        stffd = mulreplace(bitstr, repl1, repl2)
        pckg = flag + stffd + flag
        return bitstr_to_bytes(pckg)

    # Packs passed bytes.
    # Returns unstuffed from bytes package (bytes).
    def _unpack_byte_stffd(self, data):
        if len(data) <= 2: return bytes()

        flag = int_to_bitstr(self.PCKG_FLAG)
        escp = int_to_bitstr(self.ESCP_FLAG)
        bitstr = bytes_to_bitstr(data)

        # Check whether package has distinct borders.
        beg = bitstr.find(flag) + len(flag)
        if beg == -1: return
        end = bitstr[beg:].find(flag)
        if end == -1: return
        end += beg

        repl1 = (escp + escp, escp)
        repl2 = (escp + flag, flag)
        unstffd = mulreplace(bitstr[beg:end], repl1, repl2)
        return bitstr_to_bytes(unstffd)

    # Returns number of occurrences of PCKG_FLAG in passed bytes.
    def _count_pckg_flag(self, data):
        bitstr = bytes_to_bitstr(data)
        flag = int_to_bitstr(self.PCKG_FLAG)
        return bitstr.count(flag)

    @property
    def stffng_type(self):
        return self._stffng_type
