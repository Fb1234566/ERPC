import os
import socket
import struct
import threading
import queue
import pyautogui
import subprocess
from ctypes import cast, POINTER
if os.name == 'nt':
    import comtypes
    from comtypes import CLSCTX_ALL
    from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
if os.name == 'posix':
    import LinuxVolume

# Impostazioni del server
UDP_IP = "0.0.0.0"
UDP_PORT = 5006 
BUFFER_SIZE = 9

prev_x = 0
prev_y = 0

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((UDP_IP, UDP_PORT))

# Coda per la gestione degli eventi
event_queue = queue.Queue()

def set_volume(level):
    """Imposta il volume del sistema a un livello specifico (0.0 - 1.0)."""
    if os.name == 'nt':
        devices = AudioUtilities.GetSpeakers()
        interface = devices.Activate(
        IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        volume = cast(interface, POINTER(IAudioEndpointVolume))
        volume.SetMasterVolumeLevelScalar(level, None)
    if os.name == 'posix':
        LinuxVolume.changeVolume(int(level))

def press_right_click():
    pyautogui.rightClick()

def press_left_click():
    pyautogui.click()

def open_virtual_keyboard():
    subprocess.run('osk', shell=True)

def updateXY(val, val1):
    prev_x = val
    prev_y = val1

def moveXY():
    while True:
        pyautogui.move(prev_x, prev_y, duration=0.0001)    

def moveY(val):
    pyautogui.move(0, val, duration=0.0001)

def esc_pressed():
    pyautogui.press('esc')

def event_handler():
    while True:
        action, value,value1, addr = event_queue.get()
        print(f"Ricevuto azione {action} con valore {value} e {value1}\n")

        match action:
            case 1:
                updateXY(value-63, value1-63)

            case 2:
                if os.name == 'nt':
                    comtypes.CoInitialize()  # Inizializza la libreria COM per il thread
                    set_volume(value)          # Chiama la funzione per impostare il volume
                    comtypes.CoUninitialize()
                if os.name == 'posix':
                    set_volume(value)
            case 3:
                press_left_click()
            case 4:
                press_right_click()
            case 5:
                open_virtual_keyboard()
            case 6:
                esc_pressed()
            case _:
                print("Comando non riconosciuto")
            
        event_queue.task_done()

def receive_data():
    while True:
        data, addr = server_socket.recvfrom(BUFFER_SIZE)  # Riceve dati dalla rete
        print(data)
        if len(data) == BUFFER_SIZE:
            action, value, value2 = struct.unpack('<Bff', data)  # Converte i dati ricevuti (int, float)
            event_queue.put((action, value, value2, addr))
        else:
            print("Dati non validi ricevuti")

# Thread per la gestione degli eventi
event_thread = threading.Thread(target=event_handler, daemon=True)
event_thread.start()

event_thread = threading.Thread(target=moveXY, daemon=True)
event_thread.start()

# Avvia il thread di ricezione dati
receive_thread = threading.Thread(target=receive_data, daemon=True)
receive_thread.start()

# Mantieni il programma in esecuzione
receive_thread.join()
