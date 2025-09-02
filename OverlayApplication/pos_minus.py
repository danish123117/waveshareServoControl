import requests
import time

base_url = "http://192.168.1.100/cmd"  # Replace with actual IP

# The parameters you want to replicate
down = {
    "inputT": 1,
    "inputI": 6,
    "inputA": 0,
    "inputB": 0
}
up = {
    "inputT": 1,    
    "inputI": 2,
    "inputA": 0,
    "inputB": 0
}

try:
    # This is the equivalent of xhr.open(...) + xhr.send()
    response = requests.get(base_url, params=down, timeout=1, stream=True)

    # Even if there's no response, this part won't crash
    print("Request sent.")
    if response.status_code:
        print("Status:", response.status_code)
    else:
        print("No valid status code received.")
except requests.exceptions.RequestException as e:
    print("Request failed:", e)

# Wait for 2 seconds
time.sleep(2)   

try:
    response = requests.get(base_url, params=up, timeout=1, stream=True)
    # Even if there's no response, this part won't crash
    print("Request sent.")
    if response.status_code:
        print("Status:", response.status_code)
    else:
        print("No valid status code received.")
except requests.exceptions.RequestException as e:
    print("Request failed:", e)