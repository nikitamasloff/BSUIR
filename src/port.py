import serial as srl

PARITY_NONE, PARITY_EVEN, PARITY_ODD, PARITY_MARK, PARITY_SPACE = 'N', 'E', 'O', 'M', 'S'
STOPBITS_ONE, STOPBITS_ONE_POINT_FIVE, STOPBITS_TWO = (1, 1.5, 2)
FIVEBITS, SIXBITS, SEVENBITS, EIGHTBITS = (5, 6, 7, 8)
PARITY_NAMES = {
    PARITY_NONE: 'None',
    PARITY_EVEN: 'Even',
    PARITY_ODD: 'Odd',
    PARITY_MARK: 'Mark',
    PARITY_SPACE: 'Space',
}

# Reference: https://buildmedia.readthedocs.org/media/pdf/pyserial/latest/pyserial.pdf
class Port:

    BAUD_RATES = (50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800,
                  2400, 4800, 9600, 19200, 38400, 57600, 115200)
    BYTESIZES = (FIVEBITS, SIXBITS, SEVENBITS, EIGHTBITS)
    PARITIES = (PARITY_NONE, PARITY_EVEN, PARITY_ODD, PARITY_MARK, PARITY_SPACE)
    STOPBITS = (STOPBITS_ONE, STOPBITS_ONE_POINT_FIVE, STOPBITS_TWO)

    # Possible values for the parameter timeout which controls the behavior of read():
    #   • read_timeout_sec = None: wait forever / until requested number of bytes are received
    #   • read_timeout_sec = 0: non-blocking mode, return immediately in any case, returning zero or more, up to
    #       the requested number of bytes
    #   • read_timeout_sec = x: set timeout to x seconds (float allowed) returns immediately when the requested
    #       number of bytes are available, otherwise wait until the timeout expires and return all bytes that were
    #       received until then.
    # write() is blocking by default, unless write_timeout_sec is set. For possible values refer to the list for
    #       timeout above.
    def __init__(self,
                 port_name,
                 baud_rate = 9600,
                 read_timeout_sec = None,
                 write_timeout_sec = None,
                 byte_size = EIGHTBITS,
                 parity = PARITY_NONE,
                 stop_bits = STOPBITS_ONE,
                 xonxoff = False,
                 rtscts = False,
                 dsrdtr = False):
        self._port = srl.Serial()
        self.port_name = port_name
        self.baud_rate = baud_rate
        self.read_timeout_sec = read_timeout_sec
        self.write_timeout_sec = write_timeout_sec
        self.byte_size = byte_size
        self.parity = parity
        self.stop_bits = stop_bits
        self.xonxoff = xonxoff
        self.rtscts = rtscts
        self.dsrdtr = dsrdtr

    # Open or reopen port.
    def open(self):
        self._port.open()

    # Close port immediately.
    def close(self):
        self._port.close()

    # Reads bytes from specified port.
    # Returns bytes read.
    def read(self, size = 1):
        return self._port.read(size = size)

    # Writes bytes to specified port.
    # Returns number of bytes written.
    def write(self, data):
        return self._port.write(data)

    @property
    def port_name(self):
        return self._port.port

    @port_name.setter
    def port_name(self, port_name):
        self._port.port = port_name

    @property
    def read_timeout_sec(self):
        return self._port.timeout

    @read_timeout_sec.setter
    def read_timeout_sec(self, read_timeout_sec):
        self._port.timeout = read_timeout_sec

    @property
    def write_timeout_sec(self):
        return self._port.write_timeout

    @write_timeout_sec.setter
    def write_timeout_sec(self, write_timeout_sec):
        self._port.write_timeout = write_timeout_sec

    @property
    def baud_rate(self):
        return self._port.baudrate

    @baud_rate.setter
    def baud_rate(self, baud_rate):
        assert(baud_rate in self.BAUD_RATES)
        self._port.baudrate = baud_rate

    @property
    def byte_size(self):
        byte_size = self._port.bytesize
        if byte_size == srl.FIVEBITS: return FIVEBITS
        elif byte_size == srl.SIXBITS: return SIXBITS
        elif byte_size == srl.SEVENBITS: return SEVENBITS
        elif byte_size == srl.EIGHTBITS: return EIGHTBITS

    @byte_size.setter
    def byte_size(self, byte_size):
        assert byte_size in self.BYTESIZES
        if byte_size == FIVEBITS: value = srl.FIVEBITS
        elif byte_size == SIXBITS: value = srl.SIXBITS
        elif byte_size == SEVENBITS: value = srl.SEVENBITS
        elif byte_size == EIGHTBITS: value = srl.EIGHTBITS
        self._port.bytesize = value

    @property
    def parity(self):
        parity = self._port.parity
        if parity == srl.PARITY_NONE: return PARITY_NONE
        elif parity == srl.PARITY_EVEN: return PARITY_EVEN
        elif parity == srl.PARITY_ODD: return PARITY_ODD
        elif parity == srl.PARITY_MARK: return PARITY_MARK
        elif parity == srl.PARITY_SPACE: return PARITY_SPACE

    @parity.setter
    def parity(self, parity):
        assert parity in self.PARITIES
        if parity == PARITY_NONE: value = srl.PARITY_NONE
        elif parity == PARITY_EVEN: value = srl.PARITY_EVEN
        elif parity == PARITY_ODD: value = srl.PARITY_ODD
        elif parity == PARITY_MARK: value = srl.PARITY_MARK
        elif parity == PARITY_SPACE: value = srl.PARITY_SPACE
        self._port.parity = value

    @property
    def stop_bits(self):
        stop_bits = self._port.stopbits
        if stop_bits == srl.STOPBITS_ONE: return STOPBITS_ONE
        elif stop_bits == srl.STOPBITS_ONE_POINT_FIVE: return STOPBITS_ONE_POINT_FIVE
        elif stop_bits == srl.STOPBITS_TWO: return STOPBITS_TWO

    @stop_bits.setter
    def stop_bits(self, stop_bits):
        assert  stop_bits in self.STOPBITS
        if stop_bits == STOPBITS_ONE: value = srl.STOPBITS_ONE
        elif stop_bits == STOPBITS_ONE_POINT_FIVE: value = srl.STOPBITS_ONE_POINT_FIVE
        elif stop_bits == STOPBITS_TWO: value = srl.STOPBITS_TWO
        self._port.stopbits = value

    @property
    def xonxoff(self):
        return self._port.xonxoff

    @xonxoff.setter
    def xonxoff(self, xonxoff):
        self._port.xonxoff = xonxoff

    @property
    def rtscts(self):
        return self._port.rtscts

    @rtscts.setter
    def rtscts(self, rtscts):
        self._port.rtscts = rtscts

    @property
    def dsrdtr(self):
        return self._port.dsrdtr

    @dsrdtr.setter
    def dsrdtr(self, dsrdtr):
        self._port.dsrdtr = dsrdtr
