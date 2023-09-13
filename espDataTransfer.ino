#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include<esp_spiffs.h>
#include<SPIFFS.h>

WebServer server(80);
 
int producing = 0;
String s = "0";

void handleGetNextValue(){
    delay(3000);

  char jsonArrayBuffer[2048];


  File file = SPIFFS.open("/data.txt", "r");

  if(!file){
    Serial.println("Failed to open file");
    server.send(500, "text/plain", "Internal Server Error because no file");
    return;
  }

  size_t fileSize = file.size();
  Serial.println("Yo" + fileSize);
  Serial.println(fileSize);
  if(fileSize >= sizeof(jsonArrayBuffer)){
    Serial.println("File too large");
    server.send(500, "text/plain", "Internal Server Error because size");
    file.close();
    return;
  }

  size_t bytesRead = file.readBytes(jsonArrayBuffer, fileSize);
  file.close();
  
  if(bytesRead < sizeof(jsonArrayBuffer)){
    jsonArrayBuffer[bytesRead] = '\0';
  } else {
        server.send(500, "text/plain", "Internal Server Error because yoyoy");
        return;
  }


  Serial.println("JSON Array Content: ");
  Serial.println(jsonArrayBuffer);

  StaticJsonDocument<2048> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, jsonArrayBuffer);
 
  if (error) {
    Serial.println("Failed to parse JSON");
    server.send(500, "text/plain", "Internal Server Error because si=hiz");
    return;
  }
 
  // Check if we have reached the end of the array
  static int currentIndex = 0;
  if (currentIndex >= jsonDoc.size()) {
    currentIndex = 0; // Reset to the beginning
  }
 
  // Get the next value from the array
  float nextValue = jsonDoc[currentIndex++];
  unsigned long currentTime = millis();

  // Create a JSON response with the next value
  StaticJsonDocument<64> jsonResponse;
  JsonArray jsonArray = jsonResponse.createNestedArray("data");
  jsonArray.add(currentTime);
  jsonArray.add(nextValue);
 
  String responseJson;
  serializeJson(jsonResponse, responseJson);
 
  // Enable CORS by setting appropriate headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
 
  // Send the JSON response with the next value
  server.send(200, "application/json", responseJson);

}


void generateOnScreen2() {
  delay(1000);
  int y = generateRandomNumber();
  unsigned long currentTime = millis();
 
  // Create a JSON document
  StaticJsonDocument<64> jsonDoc;
 
  // Create an array and add the timestamp and random number
  JsonArray jsonArray = jsonDoc.createNestedArray("data");
  jsonArray.add(currentTime);
  jsonArray.add(y);
 
  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
 

  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");  
  // Send the JSON data as the response
  server.send(200, "application/json", jsonString);
}

void generateOnScreen() {
  
  delay(1000);
  int y = generateRandomNumber();
//  s+=String(y);
  unsigned long currentTime = millis();
  s += "(" + String(currentTime) + "," + String(y) + ")\n";
  //producing = 1;
  server.send(200, "text/plain", s);
}
 
void clearscreen(){
  s = "";
  server.send(200, "text/plain", s);
  }
 
void handleRoot(){
  server.send(200, "text/plain", s);
 }  
void handleGet() {
  if (server.hasArg("data")) {
    String data = server.arg("data");
    Serial.println("Data: " + data);
  }
  server.send(200, "text/plain", "Data Received");
}
 
void handlePost() {
  server.send(200, "text/plain", "Processing Data");
}
 
int generateRandomNumber() {
  return random(2);  // Produces a random number between 0 and 1
}
 
void handleStart(){
    producing = 1;
    server.send(200, "text/plain", "Started Recieving Data");
}
 
void handleStop(){
  producing = 0;
  server.send(200, "text/plain", "Stopped Recieving Data");
  }
 
void handleUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Receiving data:");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    server.send(200, "text/plain", "Data: ");
  }
}
 
void setup() {
  Serial.begin(115200);
  WiFi.softAP("ESP32");
  server.on("/", handleRoot);
  server.on("/gen",generateOnScreen);
  server.on("/gen2", HTTP_GET, generateOnScreen2);
  server.on("/clrscr", clearscreen);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.on("/get", HTTP_GET, handleGet);
  server.on("/final", HTTP_GET, handleGetNextValue);
  server.on("/post", HTTP_POST, handlePost, handleUpload);
  server.begin();
  
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS initialization failes");
    return;
  }
}
 
void loop() {
//  generateOnScreen();
  if (producing) {
    int randomNumber = generateRandomNumber();
    if (randomNumber == 0) {
      delay(1000);  // Wait for 1 seconds
      Serial.println(randomNumber);
 
      //means no sleep apnea
    } else {
    }
 
    Serial.print("Random Number: ");
//    server.send(200, "text/plain", String(randomNumber));
  }
 
 
  
  server.handleClient();
}
