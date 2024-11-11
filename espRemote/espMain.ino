#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "html_page.h"

// WiFi credentials
const char* ssid = "henesp";
const char* password = "12345678";

WebServer server(80);
Servo ESC; // ESC control instance

// Motor pin definitions
const int IN1 = 12;  // Motor 1 direction pin
const int IN2 = 15;  // Motor 1 direction pin
const int ENA = 13;  // Motor 1 speed

const int IN12 = 27;  // Motor 2 direction pin
const int IN22 = 26;  // Motor 2 direction pin
const int ENA2 = 25;  // Motor 2 speed


// Joystick values from HTML page
int xValue = 50;
int yValue = 50;
int sliderValue = 0; // Used for controlling the ESC
bool joystickEnabled = true;

void escControl() {
    int pwmVal = map(sliderValue, 0, 100, 0, 60); // Map slider (0-100) to ESC range (0-60)

    // Dead zone to prevent unintended small values affecting ESC
    if (pwmVal > 0 && pwmVal < 10) {
        pwmVal = 0;
    }

    ESC.write(pwmVal); // Write the PWM value to the ESC
}

// Motor control based on joystick's yValue
void motorControl() {
    int motorSpeed = map(abs(yValue)-50, 50, 100, 0, 255); // Map yValue distance from 50 to motor speed

    if (yValue > 50) { // Move forward
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN12, HIGH);
        digitalWrite(IN22, LOW);
    } else if (yValue < 50) { // Move backward
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN12, LOW);
        digitalWrite(IN22, HIGH);
    } else { // Stop
        motorSpeed = 0;
    }
    analogWrite(ENA, motorSpeed); // Set speed based on mapped value
}

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleUpdate() {
    if (server.hasArg("x") && server.hasArg("y") && server.hasArg("s")) {
        xValue = server.arg("x").toInt();
        yValue = server.arg("y").toInt();
        sliderValue = server.arg("s").toInt();
        
        Serial.printf("X: %d, Y: %d, Slider: %d\n", xValue, yValue, sliderValue);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup() {
    Serial.begin(115200);

    // Setup WiFi
    WiFi.softAP(ssid, password);
    Serial.println("Access point started");
    Serial.printf("IP Address: %s\n", WiFi.softAPIP().toString().c_str());

    // Setup server routes
    server.on("/", handleRoot);
    server.on("/u", handleUpdate);
    server.begin();
    Serial.println("Server started");

    // Initialize ESC
    ESC.attach(32); // Connect ESC to pin 13
    ESC.write(0); // Stop ESC initially
    delay(1000); // Allow time for ESC initialization

    // Initialize motor pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
}

void loop() {
    escControl();     // Control ESC based on slider input
    motorControl();   // Control motors based on joystick yValue
    server.handleClient(); // Handle web server requests
}
