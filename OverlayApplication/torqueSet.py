import requests
import json
import time

# Replace with the IP of your ESP32 board
ESP32_IP = "http://172.17.40.100/"  # or your local IP from serial monitor

## torqueLimit range is 0-1000(0-100% of the servo's torque)
data ={
  "ids": [1,2],
  "torqueLimit": [1000,1000],
  "enable": [1,1]
}

try:
    response = requests.post(url= f"{ESP32_IP}/setTorque", json=data)
    print("Status code:", response.status_code)
    print("Response:", response.text)
except requests.exceptions.RequestException as e:
    print("Error connecting to ESP32:", e)