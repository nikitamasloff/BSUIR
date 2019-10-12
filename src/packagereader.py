from port import Port
from sys import argv
from packager import Packager, BIT_STFFNG, BYTE_STFFNG

if len(argv) != 2:
    print("One argument required: port name")
    exit(1)

port_name = argv[1]
port = Port(port_name)
packager = Packager(port, stffng_type = BIT_STFFNG)

pckg = packager.read_pckg().decode('utf-8')
print('RECEIVED PACKAGE: ' + pckg)
