# Servo-Driver-with-ESP32

### Description:

This is based on the cloned repository : https://github.com/waveshare/Servo-Driver-with-ESP32 

## Wifi accespoint definition
add a headded file in the Servo driver subfolder and include following lines in it 

const char* STA_SSID = "your ssid";
const char* STA_PWD  = "your pawifi password";

### Updates with respect to cloned repo
 1) switch to st as default servo
 2) switch from access point mode to external wifi connection
 3) added the possibility to change the locking torque of the servo with /setTorque rest api endpoint / modify SMS_STS.h and .cpp files