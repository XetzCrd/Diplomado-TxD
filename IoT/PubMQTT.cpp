#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

const char* mqttBroker = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);

void reconnect();

const char* ssid[2] = {"red1", "red2"};
const char* password[2] = {"password1", "password2"};
int myNet = 1;

int i = 0;
uint32_t txWindow;

void setup() {  
  Serial.begin(115200);
  
  WiFi.begin(ssid[myNet], password[myNet]);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); Serial.print(".");
  } Serial.println("\n Connected to the WiFi network");  

  client.setServer(mqttBroker, 1883);
  txWindow = millis();
}
  
void loop() {  
  if(!client.connected()) reconnect();
  client.loop();

  if(millis() - txWindow >= 5000){
    i++; //Serial.println(i);

    String msg = String(i);
    Serial.println("Publicando: " + msg);
    client.publish("diplomado/iot/prueba_python", msg.c_str());

    txWindow = millis();
  }  
}

void reconnect(){
  while(!client.connected()){
    Serial.println("Intentando reconectar al broker...");

    if(client.connect("ESPClient")) Serial.println("Conectando al broker");
    else {
      Serial.print("Fallo, rc="); Serial.print(client.state());
      Serial.println("Intentando de nuevo en 5 segundos"); delay(5000);
    }    
  }
}