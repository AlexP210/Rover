/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <HardwareSerial.h>

#ifndef STASSID
#define STASSID "BELL129"
#define STAPSK  "52A311925213"

// #define STASSID "NETGEAR30"
// #define STAPSK  "huynhdam2022"

#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

// Specify wifi configurations
IPAddress local_IP(192, 168, 2, 28);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

// Create an instance of the server, specify the port to listen on as an argument
WiFiServer esp(5000);
IPAddress espIP = IPAddress(0);
bool serverStarted = false;
WiFiClient kerbal;

// String to hold the incoming commands
String fromKerbal;
String fromArduino;
String espCommand;

// Blink timer for encoding status
float lastBlinkTime;
bool blinkStatus = false;

// Flags for last connection status
bool wifiConnectionStatus = false;
bool kerbalConnectionStatus = false;
bool newWiFiConnectionStatus = false;
bool newKerbalConnectionStatus = false;

// Functions to handle connecting to WiFi network and client, and updating the connection statuses
void connectToWiFI(const char* ssid, const char* password, bool blocking, bool debug) {
  // Send out the connection request
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // If blocking, then wait until connected and log messages
  if (blocking) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (debug) Serial.print(F("."));
    }
    if (debug) Serial.println(F("WiFi connected"));
  }
}
bool getWiFiConnectionStatus() {
  return WiFi.status() == WL_CONNECTED;
}
bool getKerbalConnectionStatus() {
  return kerbal.connected();
}

// flag for debugging outputs
bool debug = false;

void setup() {
  // Start the serial ports
  Serial.begin(9600);
  
  // Prepare LEDs and output pins
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, blinkStatus);

  // Prepare class variables
  lastBlinkTime = millis();

  // Configure static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  // Initiate WiFi connection
  connectToWiFI(ssid, password, false, true);
}

void loop() {

  // If we're connected to WiFi then do stuff
  if (wifiConnectionStatus) {
    // If server not started, start it
    if (!serverStarted) {
      esp.begin();
      serverStarted = true;
      espIP = WiFi.localIP();
      if (debug) {Serial.print("Server started at: "); Serial.println(espIP);};
    }

    // If client not connected, try connecting
    else if (!kerbal.connected()) {
      kerbal = esp.available();
    }

    // If the server is started and client is connected, then read/write
    else {

      // Read from kerbal if there is anything available, and send it to arduino
      if (kerbal.available()) {
        fromKerbal = kerbal.readStringUntil('\n');
        while (kerbal.available()) {kerbal.read();}
        if (debug) Serial.println("ESP Received from Kerbal: " + fromKerbal);
        // check if it's meant for the esp
        if (fromKerbal.indexOf("ESP: ") != -1) {
          espCommand = fromKerbal.substring(6);
          if (espCommand == "DEBUG") {debug = !debug;}
          if (espCommand == "IP") {kerbal.println(espIP);}
        }
        else {Serial.println(fromKerbal);}
      }

      // If we get a response from the Arduino, then send it to kerbal
      if (Serial.available()) {
        fromArduino = Serial.readStringUntil('\n');
        while (Serial.available()) {Serial.read();};
        if (debug) Serial.println("ESP Received from Arduino: " + fromArduino);
        if (fromArduino.indexOf("ESP: ") != -1) {
          espCommand = fromArduino.substring(6);
          if (espCommand == "DEBUG") {debug = !debug;}
          if (espCommand == "IP") {Serial.println(espIP);}
        }
        kerbal.println(fromArduino);
      }
    }
  }

  // Update flags
  newWiFiConnectionStatus = getWiFiConnectionStatus();
  newKerbalConnectionStatus = getKerbalConnectionStatus();
  if (!wifiConnectionStatus && newWiFiConnectionStatus && debug) {Serial.println("Connected to WiFi");}
  if (!kerbalConnectionStatus && newKerbalConnectionStatus && debug) {Serial.println("Connected to Kerbal");}
  wifiConnectionStatus = newWiFiConnectionStatus;
  kerbalConnectionStatus = newKerbalConnectionStatus;

  // Blink to display our connection status
  blink(wifiConnectionStatus, kerbalConnectionStatus);
}

void blink(bool wifiConnectionStatus, bool kerbalConnectionStatus) {
  // If we're not connected to wifi, or kerbal, keep LED on
  if (!wifiConnectionStatus && !kerbalConnectionStatus) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  // if we're connected to WiFi and not kerbal, long blinks
  else if (wifiConnectionStatus && !kerbalConnectionStatus && millis() - lastBlinkTime > 1000) {
    digitalWrite(LED_BUILTIN, !blinkStatus);
    blinkStatus = !blinkStatus;
    lastBlinkTime = millis();
  }
  // if we're connected to wifi and kerbal, fast blinks
  else if (wifiConnectionStatus && kerbalConnectionStatus && millis() - lastBlinkTime > 250) {
    digitalWrite(LED_BUILTIN, !blinkStatus);
    blinkStatus = !blinkStatus;
    lastBlinkTime = millis();
  }
}