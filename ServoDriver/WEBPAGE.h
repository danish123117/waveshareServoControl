const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Servo Driver with ESP32</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial;
      background: #000;
      color: #efefef;
      text-align: center;
      max-width: 600px;
      margin: auto;
      padding: 20px;
    }
    button {
      margin: 5px;
      padding: 10px 15px;
      font-size: 18px;
      border: none;
      border-radius: 8px;
      background: #4247b7;
      color: white;
      cursor: pointer;
    }
    button:hover { background: #ff494d; }
    #status { font-weight: bold; margin-top: 10px; }
  </style>
</head>
<body>
  <h2>SERVO DRIVER with ESP32</h2>

  <p>
    <button onclick="getKinematics()">Get Kinematics</button>
  </p>
  <p>
    <button onclick="startTorque()">Start Torque Control</button>
    <button onclick="stopTorque()">Stop Torque Control</button>
  </p>

  <p><span id="data">No data yet</span></p>
  <p>Status: <span id="torqueStatus">Stopped</span></p>

  <script>
    function getKinematics() {
      fetch("/getKinematics")
        .then(response => response.json())
        .then(data => {
          document.getElementById("data").innerHTML =
            "Enc1: " + data.enc1 + "<br>" +
            "Enc2: " + data.enc2 + "<br>" +
            "X: " + data.x3.toFixed(3) + " m<br>" +
            "Y: " + data.y3.toFixed(3) + " m<br>" +
            "Status: <span id='status'>" + data.status + "</span>";
          let statusEl = document.getElementById("status");
          if (data.status === "Inside range") statusEl.style.color = "lime";
          else if (data.status === "Too close") statusEl.style.color = "yellow";
          else if (data.status === "Too far") statusEl.style.color = "red";
        })
        .catch(error => {
          document.getElementById("data").innerHTML = "Error: " + error;
        });
    }

    function startTorque() {
      fetch("/startTorque").then(() => {
        document.getElementById("torqueStatus").innerText = "Running";
        document.getElementById("torqueStatus").style.color = "lime";
      });
    }

    function stopTorque() {
      fetch("/stopTorque").then(() => {
        document.getElementById("torqueStatus").innerText = "Stopped";
        document.getElementById("torqueStatus").style.color = "red";
      });
    }
  </script>
</body>
</html>
)rawliteral";


