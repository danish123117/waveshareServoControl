import time
import requests 

# === CONFIG ===
ESP32_IP = "http://172.17.40.100"  # Replace with your ESP32's IP
SERVO_ID = 1
POSITION_MIN = 500
POSITION_MAX = 3000
TORQUE_LOCK_POSITIONS = [1700]

# === PID Tunable Parameters ===
Kp = 200

Ki = 0
Kd = 0
CONTROL_INTERVAL = 0.0001  # seconds

# === PID Controller ===
class PIDController:
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.integral = 0
        self.last_error = 0

    def compute(self, error, dt):
        self.integral += error * dt
        derivative = (error - self.last_error) / dt if dt > 0 else 0
        output = self.kp * error + self.ki * self.integral + self.kd * derivative
        self.last_error = error
        return output

# === Motor Commands ===
def send_position(position):
    payload = {
        "ids": [SERVO_ID],
        "positions": [int(position)],
        "speed": [2000],
        "acc": [50]
    }
    try:
        requests.post(f"{ESP32_IP}/setPosition", json=payload, timeout=1)
    except requests.exceptions.RequestException as e:
        print("Error sending position:", e)

def send_position0(position):
    payload = {
        "ids": [SERVO_ID],
        "positions": [int(position)],
        "speed": [100],
        "acc": [0]
    }
    try:
        requests.post(f"{ESP32_IP}/setPosition", json=payload, timeout=1)
    except requests.exceptions.RequestException as e:
        print("Error sending position:", e)

def get_servo_status():
    try:
        response = requests.get(f"{ESP32_IP}/getServoStatus", timeout=1)
        if response.status_code == 200:
            data = response.json()
            servo_data = data.get(f"id{SERVO_ID}", {})
            return {
                "position": servo_data.get("position", 0),
                "load": servo_data.get("load", 0)
            }
    except requests.exceptions.RequestException as e:
        print("Error reading servo status:", e)
    return {"position": 0, "load": 0}

def torque_lock():
    for pos in TORQUE_LOCK_POSITIONS:
        send_position0(pos)
        time.sleep(5)

# === Assistive Loop ===
def assist_loop():
    pid = PIDController(Kp, Ki, Kd)
    target_position = 1700  # Start in lock zone

    while True:
        status = get_servo_status()
        current_position = status["position"]
        current_load = status["load"]

        if POSITION_MIN <= current_position <= POSITION_MAX:
            error = current_load/1000  # Normalize
            correction = pid.compute(error, CONTROL_INTERVAL)
            target_position += correction
            target_position = max(POSITION_MIN, min(POSITION_MAX, target_position))
            send_position(target_position)
        else:
            print(f"Position {current_position} outside assist range. Holding...")

        time.sleep(CONTROL_INTERVAL)

# === Main ===
if __name__ == "__main__":
    print("Activating torque lock...")
    torque_lock()
    print("Starting assist loop with PID...")
    assist_loop()
