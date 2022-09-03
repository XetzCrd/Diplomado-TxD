#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

void GetHTTPRequest(String url);

const char* ssid[2] = {"anotherAccount", "myHome"};
const char* password[2] = {"ThisIsAnotherPassword", "ThisIsNotaPasswordWithNumbers000"};
int myNet = 1;
String url = "https://www.google.com/";

void setup() {  
  Serial.begin(115200);
  WiFi.begin(ssid[myNet], password[myNet]);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); Serial.print(".");
  }
  	
  Serial.println("\n Connected to the WiFi network");  
}
  
void loop() {  
  GetHTTPRequest(url); 
  while(1);
}

void GetHTTPRequest(String url){
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
   	
    http.begin(url); //Specify the URL 
    int httpCode = http.GET();//Make the request
    
    if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }
  
    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  }
}