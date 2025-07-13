import requests
import json
import time

# Replace with the IP of your ESP32 board
ESP32_IP = "http://192.168.1.100"  # or your local IP from serial monitor

# Example payload (single or multiple motors)
payload1 = {
    "ids": [1],
    "positions": [2500],
    "speed": [4000],
    "acc": [-30]
}

payload2= {
    "ids": [1],
    "positions": [500],
    "speed": [2000],
    "acc": [30]
}

# Send the POST request
try:
    response = requests.post(f"{ESP32_IP}/setPosition", json=payload1)
    print("Status code:", response.status_code)
    print("Response:", response.text)
except requests.exceptions.RequestException as e:
    print("Error connecting to ESP32:", e)

time.sleep(2)  # This needs to be calculated 
try:
    response = requests.post(f"{ESP32_IP}/setPosition", json=payload2)
    print("Status code:", response.status_code)
    print("Response:", response.text)
except requests.exceptions.RequestException as e:
    print("Error connecting to ESP32:", e)