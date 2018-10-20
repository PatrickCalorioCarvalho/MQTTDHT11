#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include "DHT.h"
//DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE, 15);
//WiFi
const char* SSID = "SSID";               
const char* PASSWORD = "SENHA"; 
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "192.168.18.100";
int BROKER_PORT = 1883;
const char* USER_MQTT = "pi";
const char* PASS_MQTT = "raspberry";

#define ID_MQTT  "DHT3"          
#define TOPIC_TEMP "/Cozinha/Temperatura/label"
#define TOPIC_HUMI "/Cozinha/Humidade/label"
PubSubClient MQTT(wifiClient);        

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void enviaPacote();     //

void setup() {       
  Serial.begin(115200);
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
}

void loop() {
  mantemConexoes();
  delay(10000);
  enviaValores();
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT,USER_MQTT,PASS_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void enviaValores() 
{
  int u = dht.readHumidity();
  int t = dht.readTemperature();
  if (isnan(t) || isnan(u)) 
  {
    Serial.println("Failed to read from DHT");
  }else
  {  
    char msgt[55];
    sprintf(msgt, "%d", t);
    char msgu[55];
    sprintf(msgu, "%d", u);
    MQTT.publish(TOPIC_TEMP, msgt);
    MQTT.publish(TOPIC_HUMI, msgu);
  }  
}

