import requests
import json
import time

# Replace with the IP of your ESP32 board
ESP32_IP = "http://192.168.1.100"  # or your local IP from serial monitor
data ={
  "ids": [1],
  "torqueLimit": [50],
  "enable": [1]
}
try:
    response = requests.post(url= f"{ESP32_IP}/setTorque", json=data)
    print("Status code:", response.status_code)
    print("Response:", response.text)
except requests.exceptions.RequestException as e:
    print("Error connecting to ESP32:", e)