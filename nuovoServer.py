import os
import socket
import struct
import threading
import queue
import pyautogui
import subprocess
from ctypes import cast, POINTER

# Import platform-specific modules
if os.name == 'nt':
    import comtypes
    from comtypes import CLSCTX_ALL
    from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
if os.name == 'posix':
    import LinuxVolume

# Server settings
UDP_IP = "0.0.0.0"
UDP_PORT = 5006
BUFFER_SIZE = 9
pyautogui.FAILSAFE = False

# Variables for tracking mouse position
prev_x = 0
prev_y = 0

# Create and bind the UDP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((UDP_IP, UDP_PORT))

# Queue for handling events
event_queue = queue.Queue()

def set_volume(level):
    """Sets the system volume to a specific level (0.0 - 1.0)."""
    if os.name == 'nt':
        devices = AudioUtilities.GetSpeakers()
        interface = devices.Activate(
            IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        volume = cast(interface, POINTER(IAudioEndpointVolume))
        volume.SetMasterVolumeLevelScalar(level, None)
    elif os.name == 'posix':
        LinuxVolume.changeVolume(int(level))

def press_right_click():
    """Simulates a right mouse click."""
    pyautogui.rightClick()

def press_left_click():
    """Simulates a left mouse click."""
    pyautogui.click()

def open_virtual_keyboard():
    """Opens the virtual keyboard on the system."""
    if os.name=="nt":
        subprocess.run('osk', shell=True)
    if os.name=="posix":
        oskstatus = subprocess.check_output("gsettings get org.gnome.desktop.a11y.applications screen-keyboard-enabled", shell=True)
        if (oskstatus == b"false\n"):
            subprocess.run("gsettings set org.gnome.desktop.a11y.applications screen-keyboard-enabled true", shell=True)
        elif (oskstatus == b"true\n"):
            subprocess.run("gsettings set org.gnome.desktop.a11y.applications screen-keyboard-enabled false", shell=True)

def updateXY(val, val1):
    global prev_x, prev_y

    prev_x = val
    prev_y = val1

def moveXY():
    while True:
        global prev_x, prev_y
        if (prev_x < 10 and prev_x > -10):
            prev_x = 0
        if (prev_y < 10 and prev_y > -10):
            prev_y = 0

        pyautogui.move(2*prev_x, -2*prev_y, duration=0.001)

def press_escape():
    """Simulates pressing the Escape key."""
    pyautogui.press('esc')

def event_handler():
    """Handles the events received in the event queue."""
    while True:
        action, value, value1, addr = event_queue.get()
        print(f"Received action {action} with values {value} and {value1}\n")
        # Process the action based on its type
        match action:
            case 1:
                updateXY(value-63, value1-63)
            case 2:
                # Adjust volume
                if os.name == 'nt':
                    comtypes.CoInitialize()  # Initialize COM library for the thread
                    set_volume(value)        # Set the volume
                    comtypes.CoUninitialize()
                elif os.name == 'posix':
                    set_volume(value)
            case 3:
                press_left_click()
            case 4:
                press_right_click()
            case 5:
                open_virtual_keyboard()
            case 6:
                press_escape()
            case _:
                print("Unrecognized command")

        event_queue.task_done()

def receive_data():
    """Receives data from the network and puts it in the event queue."""
    while True:
        data, addr = server_socket.recvfrom(BUFFER_SIZE)  # Receive data from the network
        if len(data) == BUFFER_SIZE:
            action, value, value2 = struct.unpack('<Bff', data)  # Converte i dati ricevuti (int, float)
            if not ((value > 200 or value < 0.1 or value2 > 200 or value2 < 0.1) and action not in (2,3,4,5,6)):
                event_queue.put((action, value, value2, addr))


        else:
            print("Invalid data received")

# Thread for event handling
event_thread = threading.Thread(target=event_handler, daemon=True)
event_thread.start()

# Thread for mouse movement
mouse_move_thread = threading.Thread(target=moveXY, daemon=True)
mouse_move_thread.start()

# Start the data receiving thread
receive_thread = threading.Thread(target=receive_data, daemon=True)
receive_thread.start()

# Keep the program running
receive_thread.join()
