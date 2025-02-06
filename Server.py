import socket
import struct
import threading
import queue
import pyautogui
import subprocess
from ctypes import cast, POINTER
import comtypes
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume


# Impostazioni del server
UDP_IP = "0.0.0.0"
UDP_PORT = 5006      
BUFFER_SIZE = 8 

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((UDP_IP, UDP_PORT))

# Coda per la gestione degli eventi
event_queue = queue.Queue()

def set_volume(level):
    """Imposta il volume del sistema a un livello specifico (0.0 - 1.0)."""
    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(
        IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume = cast(interface, POINTER(IAudioEndpointVolume))
    volume.SetMasterVolumeLevelScalar(level, None)

def press_right_click():
    pyautogui.rightClick()

def press_left_click():
    pyautogui.click()

def open_virtual_keyboard():
    subprocess.run('osk', shell=True)

def moveX(val):
    pyautogui.move(val, 0, duration=1)

def moveY(val):
    pyautogui.move(0, val, duration=1)

def event_handler():
    while True:
        action, value, addr = event_queue.get()
        print(f"Ricevuto azione {action} con valore {value}\n")

        
        if action == 1:
            moveX(value)
        elif action == 2:
            moveY(value)
        elif action == 3:
            comtypes.CoInitialize()  # Inizializza la libreria COM per il thread
            set_volume(0.5)          # Chiama la funzione per impostare il volume
            comtypes.CoUninitialize()
        elif action == 4:
            press_left_click()
        elif action == 5:
            press_left_click()
        elif action == 6:
            open_virtual_keyboard()
        else:
            print("Comando non riconosciuto")
            
        event_queue.task_done()

def receive_data():
    while True:
        data, addr = server_socket.recvfrom(BUFFER_SIZE)  # Riceve dati dalla rete
        if len(data) == BUFFER_SIZE:
            action, value = struct.unpack('!If', data)  # Converte i dati ricevuti (int, float)
            event_queue.put((action, value, addr))
        else:
            print("Dati non validi ricevuti")

# Thread per la gestione degli eventi
event_thread = threading.Thread(target=event_handler, daemon=True)
event_thread.start()

# Avvia il thread di ricezione dati
receive_thread = threading.Thread(target=receive_data, daemon=True)
receive_thread.start()

# Mantieni il programma in esecuzione
receive_thread.join()