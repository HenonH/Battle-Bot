#include <WiFi.h>
#include <WebServer.h>

// SSID and password for the ESP32's access point
const char* ssid = "hen";
const char* password = "12345678";  // Set your own password

// Web server on port 80
WebServer server(80);

// Store joystick and slider values
int xValue = 50;
int yValue = 50;
int sliderValue = 50;

// HTML content for the web page with Joystick and Slider (Landscape Mode)
String htmlPage = R"=====( 
<!DOCTYPE html>
<html>
<head>
  <title>Rhoomba Remote</title>
  <style>
    body {
      background-color: #DC143C; /* Crimson background */
      font-family: Arial, sans-serif;
      color: white;
      text-align: center;
      margin: 0;
      padding: 0;
      overflow: hidden;
    }
    h1 {
      margin-top: 10px;
    }
    #container {
      display: flex;
      justify-content: space-around;
      align-items: center;
      flex-direction: row;
      height: 100vh;
    }
    #joystick-container {
      width: 300px;  /* 1.5x original size */
      height: 300px; /* 1.5x original size */
      background-color: white;
      border-radius: 50%;
      position: relative;
      touch-action: none;
    }
    #joystick {
      width: 75px;  /* 1.5x original size */
      height: 75px; /* 1.5x original size */
      background-color: #ff4d4d;
      border-radius: 50%;
      position: absolute;
      top: 112.5px; /* Adjusted for the new size */
      left: 112.5px; /* Adjusted for the new size */
    }
    #slider {
      width: 300px;
    }
    #values {
      margin-top: 10px;
      text-align: left;
    }
  </style>
</head>
<body>
  <h1>Rhoomba Remote</h1>
  <div id="container">
    <!-- Joystick -->
    <div>
      <div id="joystick-container">
        <div id="joystick"></div>
      </div>
      <div id="values">
        <p>X: <span id="xValue">50</span></p>
        <p>Y: <span id="yValue">50</span></p>
      </div>
    </div>

    <!-- Slider -->
    <div>
      <label for="slider">Control:</label>
      <input type="range" id="slider" min="0" max="100" value="50">
      <p>Slider: <span id="sliderValue">50</span></p>
    </div>
  </div>

  <script>
    const joystick = document.getElementById('joystick');
    const container = document.getElementById('joystick-container');
    const slider = document.getElementById('slider');
    let xValue = 50, yValue = 50, sliderValue = 50;

    // Touch-friendly joystick handling
    container.addEventListener('touchstart', handleTouchStart, false);
    container.addEventListener('touchmove', handleTouchMove, false);
    container.addEventListener('touchend', handleTouchEnd, false);

    function handleTouchStart(event) {
      event.preventDefault();
    }

    function handleTouchMove(event) {
      const rect = container.getBoundingClientRect();
      const centerX = rect.left + rect.width / 2;
      const centerY = rect.top + rect.height / 2;
      
      let x = event.touches[0].clientX - centerX;
      let y = event.touches[0].clientY - centerY;
      const distance = Math.min(Math.sqrt(x * x + y * y), 112.5); // Adjusted for larger size

      const angle = Math.atan2(y, x);
      x = distance * Math.cos(angle);
      y = distance * Math.sin(angle);

      joystick.style.left = `${x + 112.5}px`; // Adjusted for larger size
      joystick.style.top = `${y + 112.5}px`;  // Adjusted for larger size

      xValue = Math.round((x / 112.5) * 50 + 50); // Map to 0-100

      // Reverse Y-axis (0 at bottom, 100 at top)
      yValue = Math.round(50 - (y / 112.5) * 50); 

      document.getElementById('xValue').textContent = xValue;
      document.getElementById('yValue').textContent = yValue;
    }

    function handleTouchEnd(event) {
      joystick.style.left = `112.5px`;  // Adjusted for larger size
      joystick.style.top = `112.5px`;   // Adjusted for larger size
      xValue = 50;
      yValue = 50;
      document.getElementById('xValue').textContent = xValue;
      document.getElementById('yValue').textContent = yValue;
    }

    // Update slider value
    slider.oninput = function() {
      sliderValue = this.value;
      document.getElementById('sliderValue').textContent = sliderValue;
    };

    // Function to send values to ESP32
    function sendData() {
      const xhr = new XMLHttpRequest();
      xhr.open('GET', `/update?x=${xValue}&y=${yValue}&slider=${sliderValue}`, true);
      xhr.send();
    }

    // Send data every 100ms
    setInterval(sendData, 100);
  </script>
</body>
</html>
)=====";


// Handle root path "/"
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Handle updating joystick and slider values
void handleUpdate() {
  if (server.hasArg("x") && server.hasArg("y") && server.hasArg("slider")) {
    xValue = server.arg("x").toInt();
    yValue = server.arg("y").toInt();
    sliderValue = server.arg("slider").toInt();

    // Print to Serial Monitor
    Serial.print("X Value: ");
    Serial.print(xValue);
    Serial.print(", Y Value: ");
    Serial.print(yValue);
    Serial.print(", Slider Value: ");
    Serial.println(sliderValue);

    // Send OK response
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);

  // Start the access point
  WiFi.softAP(ssid, password);
  Serial.println("Access point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());  // Print the IP address to access the server

  // Start the server
  server.on("/", handleRoot);        // Serve the HTML page
  server.on("/update", handleUpdate); // Handle updates from joystick and slider
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();  // Listen for client requests
}
