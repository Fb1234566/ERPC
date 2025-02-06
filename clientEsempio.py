import socket
import struct
import time
import random

# Impostazioni del client
SERVER_IP = "127.0.0.1"  # Indirizzo del server
SERVER_PORT = 5006       # Porta del server

# Creazione del socket UDP
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Funzione per inviare dati simulati
def send_data(action, value):
    data = struct.pack('!If', action, value)  # Converte l'intero e il float in una struct
    client_socket.sendto(data, (SERVER_IP, SERVER_PORT))
    print(f"Inviato azione {action} con valore {value}")

# Simulazione invio dati
actions = [1, 2, 3]  # Possibili azioni corrispondenti ai pulsanti
send_data(1, 300)
time.sleep(0.5)
send_data(2, 300)
time.sleep(0.5)
send_data(1, -300)
time.sleep(0.5)
send_data(2, -300)
time.sleep(2)
send_data(3, -300)
time.sleep(2)
send_data(4, -300)
time.sleep(2)
send_data(5, -300)
time.sleep(2)
send_data(6, -300)
time.sleep(0.5)

