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
    subprocess.run('osk', shell=True)

def update_mouse_position(x, y):
    """Updates the mouse position."""
    global prev_x, prev_y
    prev_x = x
    prev_y = y

def move_mouse():
    """Moves the mouse to the previously updated position."""
    while True:
        pyautogui.move(prev_x, prev_y, duration=0.0001)

def press_escape():
    """Simulates pressing the Escape key."""
    pyautogui.press('esc')

def event_handler():
    """Handles the events received in the event queue."""
    while True:
        action, value, value1, addr = event_queue.get()
        print(f"Received action {action} with values {value} and {value1}\n")

        # Process the action based on its type
        if action == 1:
            update_mouse_position(value - 63, value1 - 63)
        elif action == 2:
            # Adjust volume
            if os.name == 'nt':
                comtypes.CoInitialize()  # Initialize COM library for the thread
                set_volume(value)        # Set the volume
                comtypes.CoUninitialize()
            elif os.name == 'posix':
                set_volume(value)
        elif action == 3:
            press_left_click()
        elif action == 4:
            press_right_click()
        elif action == 5:
            open_virtual_keyboard()
        elif action == 6:
            press_escape()
        else:
            print("Unrecognized command")

        event_queue.task_done()

def receive_data():
    """Receives data from the network and puts it in the event queue."""
    while True:
        data, addr = server_socket.recvfrom(BUFFER_SIZE)  # Receive data from the network
        if len(data) == BUFFER_SIZE:
            action, value, value2 = struct.unpack('<Bff', data)  # Unpack the received data (int, float)
            event_queue.put((action, value, value2, addr))
        else:
            print("Invalid data received")

# Thread for event handling
event_thread = threading.Thread(target=event_handler, daemon=True)
event_thread.start()

# Thread for mouse movement
mouse_move_thread = threading.Thread(target=move_mouse, daemon=True)
mouse_move_thread.start()

# Start the data receiving thread
receive_thread = threading.Thread(target=receive_data, daemon=True)
receive_thread.start()

# Keep the program running
receive_thread.join()
