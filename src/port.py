from serial import Serial


BAUD_RATES = [50, 75, 110, 134, 150, 200, 300, 600, 1200,1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200]


# Reference: https://buildmedia.readthedocs.org/media/pdf/pyserial/latest/pyserial.pdf
class Port:

    # Possible values for the parameter timeout which controls the behavior of read():
    #   • timeout = None: wait forever / until requested number of bytes are received
    #   • timeout = 0: non-blocking mode, return immediately in any case, returning zero or more, up to
    #       the requested number of bytes
    #   • timeout = x: set timeout to x seconds (float allowed) returns immediately when the requested
    #       number of bytes are available, otherwise wait until the timeout expires and return all bytes that were
    #       received until then.
    # write() is blocking by default, unless write_timeout is set. For possible values refer to the list for
    #       timeout above.
    def open(self,
             port_name,
             baud_rate = 9600,
             read_timeout_sec = None,
             write_timeout_sec = None):
        self._port = Serial(port_name,
                            baud_rate,
                            timeout = read_timeout_sec,
                            write_timeout = write_timeout_sec)

    # Close port immediately.
    def close(self):
        del self._port

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
        assert(read_timeout_sec >= 0)
        self._port.timeout = read_timeout_sec

    @property
    def write_timeout_sec(self):
        return self._port.write_timeout

    @write_timeout_sec.setter
    def write_timeout_sec(self, write_timeout_sec):
        assert(write_timeout_sec >= 0)
        self._port.write_timeout = write_timeout_sec

    @property
    def baud_rate(self):
        return self._port.baudrate

    @baud_rate.setter
    def baud_rate(self, baud_rate):
        assert(baud_rate in BAUD_RATES)
        self._port.baudrate = baud_rate
