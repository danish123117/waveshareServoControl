# Servo-Driver-with-ESP32

### Description:

This is based on the cloned repository : https://github.com/waveshare/Servo-Driver-with-ESP32 

## WiFi Access Point Definition Instructions

Add a header file named `secrets.h` in the ServoDriver subfolder and include the following lines in it:

```cpp
const char* STA_SSID = "your ssid";
const char* STA_PWD  = "your wifi password";
```
The board will connect the above wifi credentials with the ip address 192.168.1.100
p.s. please verify the lines 426 - 428 of CONNECT.h to make sure the wifi router uses the same ip ranges and gateway IP.
update these values in case needed

### Updates with respect to cloned repo

 1) switch to st as default servo
 2) switch from access point mode to external wifi connection
 3) added the possibility to change the locking torque of the servo with /setTorque rest api endpoint / modify SMS_STS.h and .cpp files

### Adding SCServo library 
 Please add the SCServo folder to the following path 
 ```
 Documents\Arduino\libraries
```

