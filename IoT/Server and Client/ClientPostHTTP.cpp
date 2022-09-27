#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"
#include "ICM_20948.h" 

void PostHTTPRequest(String url, String name[], String data[], int cant);
String JSONFormatData(String name[], String data[], int cant);
String printLocalTime();
float getIMUData(ICM_20948_I2C *sensor, int s);

const char* ssid[2] = {"red1", "red2"};
const char* password[2] = {"password1", "password2"};
int myNet = 1;

String url = "http://192.168.1.15:5000/IMU";

#define PERIODO_SERVER 1000 //tiempo entre muestras enviadas a la base de datos
#define PERIODO_SENSOR 500  //tiempo entre lecturas de sensor
unsigned long timeServer, timeSensor;

#define SERIAL_PORT Serial
#define SPI_PORT SPI 
#define CS_PIN 2     
#define WIRE_PORT Wire
#define AD0_VAL 1   
ICM_20948_I2C myICM; 
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0; //sensor IMU

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = -5 * 3600;

void setup() {  
  Serial.begin(115200);
  
  //WiFi
  WiFi.begin(ssid[myNet], password[myNet]);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); Serial.print(".");
  } Serial.println("\n Connected to the WiFi network");  

  //NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //IMU
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);
  myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial
  bool initialized = false;
  while (!initialized){
    myICM.begin(WIRE_PORT, AD0_VAL);
    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok){
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else{
      initialized = true;
    }
  }

  timeServer = millis();
  timeSensor = millis();
}
  
void loop() {    
  //LEER SENSORES CADA 500 MS Y CALCULAR PROMEDIO 
  if(millis() - timeSensor >= PERIODO_SENSOR){ //0.5 segundos
    
    ax += getIMUData(&myICM, 0) / (PERIODO_SERVER/PERIODO_SENSOR);
    ay += getIMUData(&myICM, 1) / (PERIODO_SERVER/PERIODO_SENSOR);
    az += getIMUData(&myICM, 2) / (PERIODO_SERVER/PERIODO_SENSOR);
    gx += getIMUData(&myICM, 3) / (PERIODO_SERVER/PERIODO_SENSOR);
    gy += getIMUData(&myICM, 4) / (PERIODO_SERVER/PERIODO_SENSOR);
    gz += getIMUData(&myICM, 5) / (PERIODO_SERVER/PERIODO_SENSOR);

    timeSensor = millis();
  }
 
  //OBTENER TIMESTAMP
  String myDate = printLocalTime();

  //ENVIAR DATOS AL SERVIDOR
  if(millis() - timeServer >= PERIODO_SERVER){ //60 segundos
    int cant = 7;
    String name[cant+1] = {"ax", "ay", "az", "gx", "gy", "gz", "timestamp"};
    String data[cant+1] = {String(ax), String(ay), String(az), String(gx), String(gy), String(gz), myDate};

    //int i = 0;
    //for(i = 0 ; i<cant; i++) Serial.println(data[i]);

    PostHTTPRequest(url, name, data, cant); 
    
    ax = 0;
    ay = 0;
    az = 0;
    gx = 0;
    gy = 0;
    gz = 0;

    timeServer = millis();  
  }  
}

void PostHTTPRequest(String url, String name[], String data[], int cant){
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
   	
    http.begin(url); //Specify the URL 
    http.addHeader("Content-Type", "application/json"); //contenido tipo JSON
    
    String payload = JSONFormatData(name, data, cant); //String payload = JSONFormatData(data);

    int httpCode = http.POST(payload); //int httpCode = http.GET();//Make the request
    if (httpCode > 0) { //Check for the returning code
        Serial.print("Status code: "); Serial.println(httpCode);

        String response = http.getString();        
        Serial.println(response);
      }  
    else Serial.println("Error on HTTP request");
    http.end(); //Free the resources
  }
  else Serial.println("No internet connectrion");
}

String JSONFormatData(String name[], String data[], int cant){
  String FormatedData = "{";

  //String FormatedData  = "{\"data\": " + data + "}";
  //String FormatedData2 = "{\"data\": " + data + ", \"data1\" :" + data2 + "}";

  for ( int i = 0 ; i < cant ; i++ ){  
    FormatedData += "\"" + name[i] + "\": " + data[i];
    
    if(i<cant-1) FormatedData += ",";
  } 
  FormatedData += "}";

  return FormatedData;
}

String printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  //String myDate = (&timeinfo, "%Y-%B-%d %H:%M:%S");

  String myDate = "\""+ String(timeinfo.tm_year+1900) + "-" + String(timeinfo.tm_mon+1) + "-" + String(timeinfo.tm_mday) + " " + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec)+"\"";

  //String myDate = asctime(timeinfo);
  return myDate;
}

float getIMUData(ICM_20948_I2C *sensor, int s){  
  float d = 0;

  if (myICM.dataReady()){
    myICM.getAGMT(); //update IMU data

    //ACC
    if(s == 0) d = sensor->accX();
    if(s == 1) d = sensor->accY();
    if(s == 2) d = sensor->accZ();

    //GYRO
    if(s == 3) d = sensor->gyrX();
    if(s == 4) d = sensor->gyrY();
    if(s == 5) d = sensor->gyrZ();
    
    //MAG
    if(s == 6) d = sensor->magX();
    if(s == 7) d = sensor->magY();
    if(s == 8) d = sensor->magZ();
  }

  return d;
}