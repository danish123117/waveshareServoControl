// https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
#include <esp_now.h>
#include <WiFi.h>
#include <WebServer.h>
#include "WEBPAGE.h"
#include <ArduinoJson.h>
#include <cmath>
// =======================
// Torsional Spring Globals
// =======================
TaskHandle_t TorsionHandle;

bool springEnabled   = false;   // default disabled
int  springID        = 1;       // default servo ID
int  restPos         = 2000;    // neutral rest position
float springK        = 0.5;     // spring constant
int  maxTorque       = 1000;     // clamp torque


bool loopRunning = false;
unsigned long lastLoopTime = 0;

TaskHandle_t torqueControlHandle;
bool tortionControlRunning = false;
// Create AsyncWebServer object on port 80
WebServer server(80);



// select the ID of active servo.
void activeID(int cmdInput){
  activeNumInList += cmdInput;
  if(activeNumInList >= searchNum){
    activeNumInList = 0;
  }
  else if(activeNumInList < 0){
    activeNumInList = searchNum;
  }
}


void activeSpeed(int cmdInput){
  activeServoSpeed += cmdInput;
  if (activeServoSpeed > ServoMaxSpeed){
    activeServoSpeed = ServoMaxSpeed;
  }
  else if(activeServoSpeed < 0){
    activeServoSpeed = 0;
  }
}


int rangeCtrl(int rawInput, int minInput, int maxInput){
  if(rawInput > maxInput){
    return maxInput;
  }
  else if(rawInput < minInput){
    return minInput;
  }
  else{
    return rawInput;
  }
}


void activeCtrl(int cmdInput){
  switch(cmdInput){
    case 1:st.WritePosEx(listID[activeNumInList], ServoDigitalMiddle, activeServoSpeed, ServoInitACC);break;
    case 2:
      if(modeRead[listID[activeNumInList]] == 0) {
        servoStop(listID[activeNumInList]);
      }
      else if(modeRead[listID[activeNumInList]] == 3){
        st.WritePosEx(listID[activeNumInList], 0, 0, 0);//replaced pos with posx
      }
      break;
    case 3:servoTorque(listID[activeNumInList],0);Torque_List[activeNumInList] = false;break;
    case 4:servoTorque(listID[activeNumInList],1);Torque_List[activeNumInList] = true;break;
    case 5:
      if(modeRead[listID[activeNumInList]] == 0){
        if(SERVO_TYPE_SELECT == 1){
          st.WritePosEx(listID[activeNumInList], ServoDigitalRange - 1, activeServoSpeed, ServoInitACC);
        }
        else if(SERVO_TYPE_SELECT == 2){
          st.WritePosEx(listID[activeNumInList], ServoDigitalRange - MAX_MIN_OFFSET, activeServoSpeed, ServoInitACC);
        }
      }


      else if(modeRead[listID[activeNumInList]] == 3){
        if(SERVO_TYPE_SELECT == 1){
          st.WritePosEx(listID[activeNumInList], 10000, activeServoSpeed, ServoInitACC);
        }
        else if(SERVO_TYPE_SELECT == 2){
          st.WritePosEx(listID[activeNumInList], 0, rangeCtrl(activeServoSpeed,200,999), 0); //replaced pos with posx
        }
      }
      break;
    case 6:
      if(modeRead[listID[activeNumInList]] == 0){
        if(SERVO_TYPE_SELECT == 1){
          st.WritePosEx(listID[activeNumInList], 0, activeServoSpeed, ServoInitACC);
        }
        else if(SERVO_TYPE_SELECT == 2){
          st.WritePosEx(listID[activeNumInList], MAX_MIN_OFFSET, activeServoSpeed, ServoInitACC);
        }
      }


      else if(modeRead[listID[activeNumInList]] == 3){
        if(SERVO_TYPE_SELECT == 1){
          st.WritePosEx(listID[activeNumInList], -10000, activeServoSpeed, ServoInitACC);
        }
        else if(SERVO_TYPE_SELECT == 2){
          st.WritePosEx(listID[activeNumInList], 0, rangeCtrl(activeServoSpeed,200,999)+1024, 0);
        }
      }
      break;
    case 7:activeSpeed(100);break;
    case 8:activeSpeed(-100);break;
    case 9:servotoSet += 1;if(servotoSet > 250){servotoSet = 0;}break;
    case 10:servotoSet -= 1;if(servotoSet < 0){servotoSet = 0;}break;
    case 11:setMiddle(listID[activeNumInList]);break;
    case 12:setMode(listID[activeNumInList], 0);break;
    case 13:setMode(listID[activeNumInList], 3);break;
    case 14:SERIAL_FORWARDING = true;break;
    case 15:SERIAL_FORWARDING = false;break;
    case 16:setID(listID[activeNumInList], servotoSet);break;

    case 17:DEV_ROLE = 0;break;
    case 18:DEV_ROLE = 1;break;
    case 19:DEV_ROLE = 2;break;

    case 20:RAINBOW_STATUS = 1;break;
    case 21:RAINBOW_STATUS = 0;break;
  }
}


void handleRoot() {
 server.send(200, "text/html", index_html); //Send web page
}


void handleID() {
  if(!searchedStatus && searchFinished){
    String IDmessage = "ID:";
    for(int i = 0; i< searchNum; i++){
      IDmessage += String(listID[i]) + " ";
    }
    server.send(200, "text/plane", IDmessage);
  }
  else if(searchedStatus){
    String IDmessage = "Searching...";
    server.send(200, "text/plane", IDmessage);
  }
}


void handleSTS() {
  String stsValue = "Active ID:" + String(listID[activeNumInList]);
  if(voltageRead[listID[activeNumInList]] != -1){
    stsValue += "  Position:" + String(posRead[listID[activeNumInList]]);
    if(DEV_ROLE == 0){
      stsValue += "<p>Device Mode: Normal";
    }
    else if(DEV_ROLE == 1){
      stsValue += "<p>Device Mode: Leader";
    }
    else if(DEV_ROLE == 2){
      stsValue += "<p>Device Mode: Follower";
    }
    stsValue += "<p>Voltage:" + String(float(voltageRead[listID[activeNumInList]])/10);
    stsValue += "  Load:" + String(loadRead[listID[activeNumInList]]);
    stsValue += "<p>Speed:" + String(speedRead[listID[activeNumInList]]);

    stsValue += "  Temper:" + String(temperRead[listID[activeNumInList]]);
    stsValue += "<p>Speed Set:" + String(activeServoSpeed);
    stsValue += "<p>ID to Set:" + String(servotoSet);
    stsValue += "<p>Mode:";
    if(modeRead[listID[activeNumInList]] == 0){
      stsValue += "Servo Mode";
    }
    else if(modeRead[listID[activeNumInList]] == 3){
      stsValue += "Motor Mode";
    }

    if(Torque_List[activeNumInList]){
      stsValue += "<p>Torque On";
    }
    else{
      stsValue += "<p>Torque Off";
    }
  }
  else{
    stsValue += " FeedBack err";
  }
  server.send(200, "text/plane", stsValue); //Send ADC value only to client ajax request
}

/*
//old webCtrlServer function
void webCtrlServer(){
    server.on("/", handleRoot);
    server.on("/readID", handleID);
    server.on("/readSTS", handleSTS);

    server.on("/cmd", [](){
    int cmdT = server.arg(0).toInt();
    int cmdI = server.arg(1).toInt();
    int cmdA = server.arg(2).toInt();
    int cmdB = server.arg(3).toInt();

    switch(cmdT){
      case 0:activeID(cmdI);break;
      case 1:activeCtrl(cmdI);break;
      case 9:searchCmd = true;break;
    }
  });

    // Start server
  server.begin();
  Serial.println("Server Starts.");
}
*/
/*
// old webCtrlServer function with POST method
void webCtrlServer() {
  server.on("/", handleRoot);
  server.on("/readID", handleID);
  server.on("/readSTS", handleSTS);

  // Handle both GET and POST on /cmd with parameters
  server.on("/cmd", HTTP_ANY, []() {
    int cmdT = server.arg(0).toInt();
    int cmdI = server.arg(1).toInt();
    int cmdA = server.arg(2).toInt();
    int cmdB = server.arg(3).toInt();

    switch (cmdT) {
      case 0: activeID(cmdI); break;
      case 1: activeCtrl(cmdI); break;
      case 9: searchCmd = true; break;
    }

    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server Starts.");
}


*/

// =======================
// Torsional Spring Control Loop
// =======================
void torsionalLoop(void *pvParameter) {
  while (1) {
    if (!springEnabled) {
      delay(50);
      continue;
    }

    // Get feedback
    getFeedBack(springID);
    int pos  = posRead[springID];
    int load = loadRead[springID];

    // Compute displacement
    int displacement = pos - restPos;
    int torqueCmd = (int)(springK * displacement);

    // Clamp
    if (torqueCmd > maxTorque) torqueCmd = maxTorque;
    if (torqueCmd < -maxTorque) torqueCmd = -maxTorque;

    // Apply torque
    int target = pos + torqueCmd;
    st.WritePosEx(springID, target, 1000, 0);

    delay(50);  // 50 ms (like your 50000 µs loop)
  }
}

void torsionInit() {
  xTaskCreatePinnedToCore(
    &torsionalLoop,   // function
    "TorsionalTask",  // task name
    4000,             // stack size
    NULL,             // params
    5,                // priority
    &TorsionHandle,   // handle
    ARDUINO_RUNNING_CORE  // core
  );
}

void forwardKinematics ( double enc1_data , double enc2_data , double &x3 , double &y3) {
      double L1 = 0.168;
      double L2 = 0.17;
      double L3 = 0.168;
      double L4 = 0.17;
      double L5 = 0.1;
  
      double t1 = ((3072-enc1_data)*3.1416)/2048 ;
      double t5 = ((3072- enc2_data)*3.1416)/2048;
      double x2 = L1 * cos (t1);
      double y2 = L1 * sin (t1);
      double x4 = (L4 * cos (t5)) - L5;
      double y4 = L4 * sin (t5);
      double P4P2 = sqrt (( pow (x4 - x2 , 2)) + (pow (y4 - y2 , 2)));
      double PHP2 = (( pow (L2 , 2)) - ( pow (L3 , 2)) + ( pow (P4P2 , 2))) / (2 * P4P2 );
      double PHP3 = sqrt (( pow (L2 , 2)) - ( pow (PHP2 , 2)));
      double xH = x2 + (( PHP2 / P4P2 ) * (x4 - x2));
      double yH = y2 + (( PHP2 / P4P2 ) * (y4 - y2));
      x3 = xH + (( PHP3 / P4P2 ) * (y4 - y2));
      y3 = yH - (( PHP3 / P4P2 ) * (x4 - x2));
}


bool inverseKinematics(double x3, double y3, int &enc1, int &enc2) {
    double L1 = 0.17;
    double L2 = 0.17;
    double L3 = 0.17;
    double L4 = 0.17;
    double L5 = 0.1;  
  
  // Distances for circle intersections
    double d_left  = hypot(x3, y3);
    double d_right = hypot(x3 + L5, y3);

    // Reachability check
    if (d_left > (L1 + L2) || d_left < fabs(L1 - L2)) return false;
    if (d_right > (L3 + L4) || d_right < fabs(L4 - L3)) return false;

    // --- Left side (joint 2) ---
    double a1 = (L1*L1 - L2*L2 + d_left*d_left) / (2 * d_left);
    double h1 = sqrt(L1*L1 - a1*a1);
    double x2 = (a1/d_left) * x3 + (h1/d_left) * y3;
    double y2 = (a1/d_left) * y3 - (h1/d_left) * x3;
    double t1 = atan2(y2, x2);

    // --- Right side (joint 4) ---
    double dx = x3 + L5;
    double dy = y3;
    double d  = hypot(dx, dy);
    double a2 = (L4*L4 - L3*L3 + d*d) / (2 * d);
    double h2 = sqrt(L4*L4 - a2*a2);
    double x4 = (a2/d) * dx - (h2/d) * dy - L5;
    double y4 = (a2/d) * dy + (h2/d) * dx;
    double t5 = atan2(y4, x4 + L5);

    // Convert radians back to encoder counts
    enc1 = (int)lround(3072.0 - (t1 * 2048.0 / M_PI));
    enc2 = (int)lround(3072.0 - (t5 * 2048.0 / M_PI));

    return true;   // success
}

int checkDistance(double x, double y, double x1, double y1, double r0, double r1) {
    double dx = x - x1;
    double dy = y - y1;
    double dist2 = dx*dx + dy*dy;  // squared distance

    if (dist2 < r0*r0) {
        return -1;
    } else if (dist2 > r1*r1) {
        return 1;
    } else {
        return 0;
    }
}

int checkDistance2(double x) {
    // Default workspace limits
    double x_lower = -0.15;  // center x
    double x_upper =  0.05;       // outer radius
    if (x < x_lower) {
        return -1;  // too far left
    } else if (x > x_upper) {
        return 1;   // too far right
    } else {
        return 0;   // within limits
    }
    
}

int checkDistance3(double y) {
    // Default workspace limits
    double y_lower = 0.2;  // center x
    double y_upper =  0.27;       // outer radius
    if (y < y_lower) {
        return -1;  // too far left
    } else if (y > y_upper) {
        return 1;   // too far right
    } else {
        return 0;   // within limits
    }
    
}
int generateCorrection1(double x3, double y3, int &enc1, int &enc2) {
    double x_center = -0.05;
    double y_center = 0.165;
    double r0 = 0.06;
    double r1 = 0.08;

    double dx = x3 - x_center;
    double dy = y3 - y_center;
    double dist = sqrt(dx*dx + dy*dy);

    if (dist < r0) {
        // Inside inner circle
        double scale = r0 / dist;
        double new_x = x_center + dx * scale;
        double new_y = y_center + dy * scale;
        if (inverseKinematics(new_x, new_y, enc1, enc2)) {
            return 1; // correction applied
        }
    } else if (dist > r1) {
        // Outside outer circle
        double scale = r1 / dist;
        double new_x = x_center + dx * scale;
        double new_y = y_center + dy * scale;
        if (inverseKinematics(new_x, new_y, enc1, enc2)) {
            return 1; // correction applied
        }
    }
    return 0; // no correction needed or failed
}
int generateCorrection2(double x3 , double y3, int zone, int &enc1, int &enc2) {
    double x_off_l = -0.145;
    double x_off_2 = 0.045;

    if (zone ==  -1) {
        // Too far left, correct to left boundary
        if (inverseKinematics(x_off_l, y3, enc1, enc2)) {
            return 1; // correction applied
        }
    } else if (zone == 1 && x3 > x_off_2) {
        // Too far right, correct to right boundary
        if (inverseKinematics(x_off_2, y3, enc1, enc2)) {
            return 1; // correction applied
        }
    }
    return 0; // no correction needed or failed
}

int generateCorrection3(double x3 , double y3, int zone, int &enc1, int &enc2) {
    double y_off_l = 0.205;
    double y_off_2 = 0.265;

    if (zone ==  -1) {
        // Too far left, correct to left boundary
        if (inverseKinematics(x3, y_off_l, enc1, enc2)) {
            return 1; // correction applied
        }
    } else if (zone == 1 && y3 > y_off_2) {
        // Too far right, correct to right boundary
        if (inverseKinematics(x3, y_off_2, enc1, enc2)) {
            return 1; // correction applied
        }
    }
    return 0; // no correction needed or failed
}




void handleGetKinematics() {
    // Read motor feedback
    getFeedBack(1);
    getFeedBack(2);

    int enc1 = (int)round(posRead[1]);
    int enc2 = (int)round(posRead[2]);
    int enc1_b = 3072-enc1;
    int enc2_b = 3072-enc2;
    // Forward kinematics
    double x3, y3;
    forwardKinematics(enc1, enc2, x3, y3);

    // Check distance (example: workspace between 5cm and 20cm from origin)
    //int status = checkDistance(x3, y3, -0.05, 0.165, 0.06, 0.08);
    int status = checkDistance2(x3);
    // Prepare JSON
    StaticJsonDocument<256> doc;
    doc["enc1"] = enc1;
    doc["enc2"] = enc2;
    doc["x3"] = x3;
    doc["y3"] = y3;
    doc["enc1_b"] = enc1_b;
    doc["enc2_b"] = enc2_b;
    doc["status"] = status;

    String json;
    serializeJson(doc, json);

    server.send(200, "application/json", json);
}

void releaseLock(){

        servoTorque(1, 0);
        servoTorque(2, 0);
}

void motorLock(){

        servoTorque(1, 1);
        servoTorque(2, 1);
}


void controlTorque(double x_center, double y_center, double r0, double r1) {
    // 1. Read encoders from servo motors
    // (replace these calls with the actual ST3125 API functions)
    getFeedBack(1);
    getFeedBack(2);

    // Step 2: compute end-effector position
    double x3, y3;
    forwardKinematics(posRead[1], posRead[2], x3, y3);

    // 3. Run distance check
    //int result = checkDistance(x3, y3, x_center, y_center, r0, r1);
    int result = checkDistance2(x3);
      //result= 0;
    // 4. Apply or release torque
    if (result == 0) {
        servoTorque(1, 0);
        servoTorque(2, 0);
    } else {
        servoTorque(1, 1);
        servoTorque(2, 1);
    }
}

void controlTorque2(void * parameter) {
    // 1. Read encoders from servo motors
    // (replace these calls with the actual ST3125 API functions)
    while (1) {
    //if (!tortionControlRunning) {
     // delay(50);
      //continue;
    //}
    getFeedBack(1);
    getFeedBack(2);

    // Step 2: compute end-effector position
    double x3, y3;
    forwardKinematics(posRead[1], posRead[2], x3, y3);

    // 3. Run distance check
    //int zone = checkDistance(x3, y3, x_center, y_center, r0, r1);
    int zone = checkDistance3(y3);
      //zone= 0;
    int enc1 = (int)round(posRead[1]);
    int enc2 = (int)round(posRead[2]);
    // 4. Apply or release torque
    if (zone == 0) {
      releaseLock();
    } else {
        // Try to generate correction
        int enc1c , enc2c;
        int correctionStatus = generateCorrection3(x3, y3, zone, enc1c, enc2c);
        if (correctionStatus == 1) {
            // Correction successful, apply new positions
            u8 ids[2]        = {1, 2};
            s16 positions[2] = {enc1c, enc2c};
            u16 speeds[2]    = {4000, 4000};
            u8 accs[2]       = {100, 100};

            st.SyncWritePosEx(ids, 2, positions, speeds, accs);

            delay(100); // allow some time to reach
            releaseLock();
        } else {
            // Correction failed or not needed
            releaseLock();
        }
    }
    delay(50);  // 50 ms loop
  }
}

void controlTorque3(void * parameter) {
    // 1. Read encoders from servo motors
    // (replace these calls with the actual ST3125 API functions)
    while (1) {
    //if (!tortionControlRunning) {
     // delay(50);
      //continue;
    //}
    getFeedBack(1);
    getFeedBack(2);

    // Step 2: compute end-effector position
    double x3, y3;
    forwardKinematics(posRead[1], posRead[2], x3, y3);

    // 3. Run distance check
    //int zone = checkDistance(x3, y3, x_center, y_center, r0, r1);
    int zone = checkDistance3(y3);
      //zone= 0;
    int enc1 = (int)round(posRead[1]);
    int enc2 = (int)round(posRead[2]);
    // 4. Apply or release torque
    if (zone == 0) {
      releaseLock();
    } else {
        motorLock();

    }
    delay(50);  // 50 ms loop
  }
}


void controlTorque2Init() {
  xTaskCreatePinnedToCore(
    &controlTorque2,   // function
    "CT2",  // task name
    4000,             // stack size
    NULL,             // params
    5,                // priority
    &torqueControlHandle,   // handle
    ARDUINO_RUNNING_CORE  // core
  );
}
void webCtrlServer() {
  server.on("/", handleRoot);
  server.on("/readID", handleID);
  server.on("/readSTS", handleSTS);

  // Handle basic command
  server.on("/cmd", HTTP_ANY, []() {
    int cmdT = server.arg(0).toInt();
    int cmdI = server.arg(1).toInt();
    int cmdA = server.arg(2).toInt();
    int cmdB = server.arg(3).toInt();

    switch (cmdT) {
      case 0: activeID(cmdI); break;
      case 1: activeCtrl(cmdI); break;
      case 9: searchCmd = true; break;
    }

    server.send(200, "text/plain", "OK");
  });

  // New endpoint to control multiple motors with position, speed, and acc lists
  server.on("/setPosition", HTTP_POST, []() {
    if (server.args() == 0) {
      server.send(400, "text/plain", "No body received");
      return;
    }

    // Allocate a reasonable buffer (adjust upward for >10 motors)
    const size_t capacity = JSON_ARRAY_SIZE(10) * 4 + JSON_OBJECT_SIZE(4) + 300;
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, server.arg(0));
    if (error) {
      server.send(400, "text/plain", "Invalid JSON");
      return;
    }

    JsonArray idsArray     = doc["ids"];
    JsonArray posArray     = doc["positions"];
    JsonArray speedArray   = doc["speed"];
    JsonArray accArray     = doc["acc"];

    size_t n = idsArray.size();
    if (n == 0 ||
        posArray.size() != n ||
        speedArray.size() != n ||
        accArray.size() != n) {
      server.send(400, "text/plain", "Array size mismatch or empty lists");
      return;
    }

    u8* ids        = new u8[n];
    s16* positions = new s16[n];
    u16* speeds    = new u16[n];
    u8* accs       = new u8[n];

    for (size_t i = 0; i < n; ++i) {
      ids[i]        = (u8)idsArray[i].as<int>();
      positions[i]  = (s16)posArray[i].as<int>();
      speeds[i]     = (u16)speedArray[i].as<int>();
      accs[i]       = (u8)accArray[i].as<int>();
    }

    st.SyncWritePosEx(ids, n, positions, speeds, accs);

    delete[] ids;
    delete[] positions;
    delete[] speeds;
    delete[] accs;

    server.send(200, "text/plain", "Motor positions updated");
  }
);
server.on("/getServoStatus", HTTP_GET, []() {
  String response = "{";
  for (int i = 0; i < searchNum; i++) {
    byte id = listID[i];
    getFeedBack(id);  // <- This is key

    response += "\"id" + String(id) + "\": {";
    response += "\"position\": " + String(posRead[id]) + ",";
    response += "\"speed\": " + String(speedRead[id]) + ",";
    response += "\"load\": " + String(loadRead[id]) + ",";
    response += "\"voltage\": " + String(float(voltageRead[id]) / 10) + ",";
    response += "\"temperature\": " + String(temperRead[id]) + ",";
    response += "\"mode\": " + String(modeRead[id]);
    response += "}";

    if (i < searchNum - 1) {
      response += ",";
    }
  }
  response += "}";

  server.send(200, "application/json", response);
});
//THis was added on 14-8
server.on("/setTorque", HTTP_POST, []() {
    if (server.args() == 0) {
      server.send(400, "text/plain", "No body received");
      return;
    }

    // Adjust size as needed
    const size_t capacity = JSON_ARRAY_SIZE(10) * 3 + JSON_OBJECT_SIZE(3) + 200;
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, server.arg(0));
    if (error) {
      server.send(400, "text/plain", "Invalid JSON");
      return;
    }

    JsonArray idsArray         = doc["ids"];
    JsonArray torqueArray      = doc["torqueLimit"];
    JsonArray enableArray      = doc["enable"];

    size_t n = idsArray.size();
    if (n == 0 ||
        torqueArray.size() != n ||
        enableArray.size() != n) {
      server.send(400, "text/plain", "Array size mismatch or empty lists");
      return;
    }

    u8* ids        = new u8[n];
    u16* torques   = new u16[n];
    u8* enables    = new u8[n];

    for (size_t i = 0; i < n; ++i) {
      ids[i]     = (u8)idsArray[i].as<int>();
      torques[i] = (u16)torqueArray[i].as<int>();
      enables[i] = (u8)enableArray[i].as<int>();

      // Apply torque limit and enable for each motor
      st.SetTorqueLimit(ids[i], torques[i]);
      st.EnableTorque(ids[i], enables[i]);
    }

    delete[] ids;
    delete[] torques;
    delete[] enables;

    server.send(200, "text/plain", "Torque settings updated");
});
server.on("/torsion", HTTP_POST, []() {
    if (server.hasArg("plain") == false) {
      server.send(400, "application/json", "{\"error\":\"No body received\"}");
      return;
    }

    String body = server.arg("plain");
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Parse parameters
    if (doc.containsKey("enabled"))   springEnabled = doc["enabled"];
    if (doc.containsKey("id"))        springID      = doc["id"];
    if (doc.containsKey("restPos"))   restPos       = doc["restPos"];
    if (doc.containsKey("springK"))   springK       = doc["springK"];
    if (doc.containsKey("maxTorque")) maxTorque     = doc["maxTorque"];

    // Send response
    StaticJsonDocument<256> resp;
    resp["enabled"]   = springEnabled;
    resp["id"]        = springID;
    resp["restPos"]   = restPos;
    resp["springK"]   = springK;
    resp["maxTorque"] = maxTorque;

    String response;
    serializeJson(resp, response);
    server.send(200, "application/json", response);
  });
    server.on("/startLoop", HTTP_GET, []() {
    tortionControlRunning = true;
    server.send(200, "text/plain", "Loop started");
  });

  server.on("/stopLoop", HTTP_GET, []() {
    tortionControlRunning= false;
    server.send(200, "text/plain", "Loop stopped");
  });

  server.on("/getKinematics", handleGetKinematics);
  server.begin();
  Serial.println("Server Starts.");
}


void webServerSetup(){
  webCtrlServer();
}


void getMAC(){
  WiFi.mode(WIFI_AP_STA);
  MAC_ADDRESS = WiFi.macAddress();
  Serial.print("MAC:");
  Serial.println(WiFi.macAddress());
}


void getIP(){
  IP_ADDRESS = WiFi.localIP();
}


void setAP(){
  WiFi.softAP(AP_SSID, AP_PWD);
  IPAddress myIP = WiFi.softAPIP();
  IP_ADDRESS = myIP;
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  WIFI_MODE = 1;
}

/*
void setSTA(){
  WIFI_MODE = 3;
  WiFi.begin(STA_SSID, STA_PWD);
}
*/
void setSTA(){
  WiFi.mode(WIFI_STA);
  //IPAddress local_IP(192, 168, 1, 100);      // Desired static IP
  //IPAddress gateway(192, 168, 1, 1);         // Your router’s IP
  //IPAddress subnet(255, 255, 255, 0);        // Subnet mask


  IPAddress local_IP(172, 17, 40, 100);      // Desired static IP
  IPAddress gateway(172, 17, 40, 1);         // Your router’s IP
  IPAddress subnet(255, 255, 255, 0); 

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }



  WiFi.begin(STA_SSID, STA_PWD);
  Serial.print("Connecting to WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    WIFI_MODE = 2;
    getIP();
  } else {
    Serial.println("\nWiFi connection failed.");
    // Optional: fallback here if you want
  }
  Serial.print("Device IP: ");
  Serial.println(WiFi.localIP());

}



void getWifiStatus(){
  if(WiFi.status() == WL_CONNECTED){
    WIFI_MODE = 2;
    getIP();
    WIFI_RSSI = WiFi.RSSI();
  }
  else if(WiFi.status() == WL_CONNECTION_LOST && DEFAULT_WIFI_MODE == 2){
    WIFI_MODE = 3;
    // WiFi.disconnect();
    WiFi.reconnect();
  }
}


/* 
//old wifiInit function
void wifiInit(){
  DEV_ROLE  = DEFAULT_ROLE;
  WIFI_MODE = DEFAULT_WIFI_MODE;
  if(WIFI_MODE == 1){setAP();}
  else if(WIFI_MODE == 2){setSTA();}
}
*/

void wifiInit(){
  DEV_ROLE  = DEFAULT_ROLE;
  WIFI_MODE = 2; // Force station mode
  setSTA();      // Always call station init
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if(DEV_ROLE == 2){
    memcpy(&myData, incomingData, sizeof(myData));
    myData.Spd_send = abs(myData.Spd_send);
    if(myData.Spd_send < 50){
      myData.Spd_send = 200;
    }
    st.WritePosEx(myData.ID_send, myData.POS_send, abs(myData.Spd_send), 0);

    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("POS: ");
    Serial.println(myData.POS_send);
    Serial.print("SPEED: ");
    Serial.println(abs(myData.Spd_send));
  }
}


void espNowInit(){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  MAC_ADDRESS = WiFi.macAddress();
  Serial.print("MAC:");
  Serial.println(WiFi.macAddress());
}