
import requests
import json
import time

type = 1

# Replace with the IP of your ESP32 board
ESP32_IP = "http://192.168.1.100"  # or your local IP from serial monitor

## torqueLimit range is 0-1000(0-100% of the servo's torque)
if type == 1:
    data ={
    "enabled": True,
    "id": 1,
    "restPos": 2048,
    "springK": 200,
    "maxTorque": 500
}
else:
    data ={
  "enabled": False,
  "id": 1,
  "restPos": 2048,
  "springK": 0.6,
  "maxTorque": 150
}
try:
    response = requests.post(url= f"{ESP32_IP}/torsion", json=data)
    print("Status code:", response.status_code)
    print("Response:", response.text)
except requests.exceptions.RequestException as e:
    print("Error connecting to ESP32:", e)