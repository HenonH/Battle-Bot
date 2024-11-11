#include <WiFi.h>
#include <website.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ESC Control for ESP32 S3 WROOM
int pwmVal;
const int servo_pin = 13; // GPIO 13 is more reliable on ESP32 S3

// Initialize Servo object for ESC
Servo ESC;


// Web server on port 80
WebServer server(80);
// Store joystick and slider values
int xValue = 50, yValue = 50, sliderValue = 50;

// SSID and password for the ESP32's access point
const char* ssid = "hen";
const char* password = "12345678";  // Set your own password


void escControl() {
    pwmVal = map(sliderValue, 0, 100, 0, 100);

    if (pwmVal > -10 && pwmVal < 10) 
    {
      pwmVal = 0;
    }

    ESC.write(pwmVal);
}

void setup() {
  Serial.begin(115200);

  ESC.attach(servo_pin, 1000, 2000);
  ESC.write(0);
  delay(2000);

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
  escControl();
}
