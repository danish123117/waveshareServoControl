import requests
import time

base_url = "http://192.168.1.100/cmd" 
down = {"inputT": 1,"inputI": 5,"inputA": 0,"inputB": 0}
up = {"inputT": 1, "inputI": 2, "inputA": 0, "inputB": 0}

try:
    response = requests.get(base_url, params=down, timeout=1, stream=True)
    print("Request sent.")
    if response.status_code:
        print("Status:", response.status_code)
    else:
        print("No valid status code received.")
except requests.exceptions.RequestException as e:
    print("Request failed:", e)

time.sleep(5)   

try:
    response = requests.get(base_url, params=up, timeout=1, stream=True)
    print("Request sent.")
    if response.status_code:
        print("Status:", response.status_code)
    else:
        print("No valid status code received.")
except requests.exceptions.RequestException as e:
    print("Request failed:", e)
