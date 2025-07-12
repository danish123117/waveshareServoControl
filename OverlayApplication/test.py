import requests
import time
from controller.controller import *



setServoMode("servo")

listt = ["ccw","cw"]

t=2

rotateTime(t,"ccw")
time.sleep(0.5)
rotateTime(t,"cw")
