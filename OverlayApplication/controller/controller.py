import requests
import time

def _callCommand(parms):
    base_url = "http://192.168.4.1/cmd" 
    try:
        response = requests.post(base_url, params=parms, timeout=5, stream=True)
        print("Request sent.")
        if response.status_code:
            print("Status:", response.status_code)
        else:
            print("No valid status code received.")
    except requests.exceptions.RequestException as e:
        print("Request failed:", e) 

def rotateTime(rotation_time,direction):
    if direction == "cw":
        inputI_down = 5
    elif direction == "ccw":
        inputI_down =6
    down = {"inputT": 1,"inputI": inputI_down,"inputA": 0,"inputB": 0}
    up = {"inputT": 1, "inputI": 2, "inputA": 0, "inputB": 0}
    _callCommand(down)
    time.sleep(rotation_time)
    _callCommand(up)
    
def speedChange(change):
    if change == "up":
        inputI = 7
    
    elif change == "down":
        inputI = 8
    else:
        raise ValueError("Invalid speed change direction. Use 'up' or 'down'.")
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def setID(change):
    if change == "up":
        inputI = 9
    elif change == "down":
        inputI = 10
    else:
        raise ValueError("Invalid ID change direction. Use 'up' or 'down'.")
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)


def selectID(change):
    if change == "up":
        inputI = 1
    elif change == "down":
        inputI = -1
    else:
        raise ValueError("Invalid ID change direction. Use 'up' or 'down'.")
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def switchRainbow(status):
    if status == "on":
        inputI = 20
    elif status == "off":
        inputI = 21
    else:
        raise ValueError("Invalid rainbow status. Use 'on' or 'off'.")
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)


def commands(command):
    if command == "mid":
        inputI = 1
    elif command == "stop":
        inputI = 2
    elif command == "release":
        inputI = 3
    elif command == "torque":
        inputI = 4
    else:
        raise ValueError("Invalid command. Use 'down', 'up', 'stop', 'release', or 'torque'.") 
          
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def search():
    parms = {"inputT": 9, "inputI": 0, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def setRole(role):
    if role == "normal":
        inputI = 17
    elif role == "leader":
        inputI = 18
    elif role == "follower":
        inputI = 19
    else:
        raise ValueError("Invalid role. Use 'normal', 'leader', or 'follower'.")
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def setServoMode(mode):
    if mode == "servo":
        inputI = 12
    elif mode == "motor":
        inputI = 13
    else:
        raise ValueError("Invalid mode. Use 'servo' or 'stepper'.")
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def setServoMid():
    parms = {"inputT": 1, "inputI": 11, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def serialForwarding(status):
    if status == True:
        inputI = 15
    else:
        inputI = 14
    parms = {"inputT": 1, "inputI": inputI, "inputA": 0, "inputB": 0}
    _callCommand(parms)

def readState(ip):
    url = f"http://{ip}/readSTS"
    try:
        response = requests.get(url, timeout=1)
        print("Request sent.")
        if response:
            return response
    except requests.exceptions.RequestException as e:
        print("Request failed:", e) 

