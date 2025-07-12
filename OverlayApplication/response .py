import requests
import time
from controller.controller import *



test = readState("192.168.1.34")
print(test.text)