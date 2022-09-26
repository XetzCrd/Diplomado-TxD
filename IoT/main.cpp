#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

void GetHTTPRequest(String url, String data);
String JsonFormatData(String data);

const char* ssid[2] = {"redpucp", "Casa"};
const char* password[2] = {"C9AA28BA93", "SOPADEPOLLOCONCARNE789"};
int myNet = 1;

String url = "http://192.168.1.17:5000/sensor";
String epDistancia = "/distancia";

int i = 0;

void setup() {  
  Serial.begin(115200);
  
  WiFi.begin(ssid[myNet], password[myNet]);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); Serial.print(".");
  }
  
  Serial.println("\n Connected to the WiFi network");  
}
  
void loop() {  
  i++;
  //Serial.println(i);
  GetHTTPRequest(url+epDistancia, String(i)); 
  delay(3000);
}

void GetHTTPRequest(String url, String data){
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
   	
    http.begin(url); //Specify the URL 
    http.addHeader("Content-Type", "application/json"); //contenido tipo JSON
    
    String payload = JsonFormatData(data);

    int httpCode = http.POST(payload); //int httpCode = http.GET();//Make the request
    if (httpCode > 0) { //Check for the returning code
        Serial.print("Status code: "); Serial.println(httpCode);

        String response = http.getString();        
        Serial.println(response);
      }  
    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
  }
  else{
    Serial.println("No internet connectrion");
  }
}

String JsonFormatData(String data){
  
  String FormatedData = "{\"data\": " + data + "}";

  return FormatedData;
}