from com import Com, BAUD_RATES
from sys import argv
import random
import string
import time
import keyboard


# Check arguments

if len(argv) != 2:
    print("One argument required: port name")
    exit(1)

port_name = argv[1]

# --------------------------------------



# Init port

port = Com()
port.open(port_name)

# -------------------------------------------------



# Utils

def print_baudrate():
    print("current baudrate: " + repr(port.baud_rate))

def random_char():
    return random.choice(string.ascii_letters)

# -------------------------------------------------



# Set hotkeys

keyboard.add_hotkey('shift+1', lambda: on_shift1())
keyboard.add_hotkey('shift+2', lambda: on_shift2())
keyboard.add_hotkey('shift+3', lambda: on_shift3())

def on_shift1():
    global port
    port.baud_rate = BAUD_RATES[0]
    print_baudrate()

def on_shift2():
    global port
    rand_index = random.randrange(1, len(BAUD_RATES) - 1)
    port.baud_rate = BAUD_RATES[rand_index]
    print_baudrate()

def on_shift3():
    global port
    port.baud_rate = BAUD_RATES[-1]
    print_baudrate()

# -------------------------------------------------


# Main

while True:
    data = random_char()
    print("WRITE: " + str(data))
    port.write(data.encode())
    time.sleep(0.2)
