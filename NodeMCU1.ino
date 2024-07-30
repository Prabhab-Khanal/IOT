#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const char* ssid = "Islington College";
const char* password = "I$LiNGT0N2024";
TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(4, 5); // The serial connection to the GPS device
SoftwareSerial dd(1, 3);
// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "9779843347222";
String apiKey = "8021513";

float latitude, longitude;

void sendMessage(String message) {
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&text=" + urlEncode(message) + "&apikey=" + apiKey;
  
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);
  int httpResponseCode = http.GET(); // Use GET for WhatsApp API
  
  if (httpResponseCode == 200) {
    Serial.println("Message sent successfully");
  }
  else {
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Send Message to WhatsApp
  sendMessage("Landmine Bot Activated.Semper Fi, oorah, give your all!");
}

void loop() {
  if (Serial.available() > 0) {
    // Read the incoming signal from Arduino
    char incomingSignal = Serial.read();

    if (incomingSignal == '1') {
      // Arduino detected something, check if GPS location is available
      if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();

        // Send the location as a message
        String message = "Landmine detected at\nLatitude: " + String(latitude, 6) + "\nLongitude: " + String(longitude, 6);
        sendMessage(message);
      } else {
        // GPS location is not valid, send an alert
        sendMessage("Alert! Landmine detected, but GPS location not available.");
      }
    } else {
      // Signal from Arduino is not recognized
      sendMessage("Alert! Unknown signal received from Arduino.");
    }
  }

  // You can add other non-blocking tasks here if needed
  delay(100);
}

