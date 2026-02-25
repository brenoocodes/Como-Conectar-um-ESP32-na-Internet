#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "certs.h"

#define DHTPIN 4
#define DHTTYPE DHT11

WiFiClientSecure net;
PubSubClient client(net);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastReport = 0;
const unsigned long REPORT_INTERVAL = 30000;

String topicSub = "receivedData/" + String(AWS_CLIENT_ID);
String topicPub = "sendData/" + String(AWS_CLIENT_ID);

void publishReport();

void messageCallback(char* topic, byte* payload, unsigned int length){
    Serial.print("[MQTT] Mensagem recebida em ");
    Serial.print(topic);
    Serial.print(": ");

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if(error){
        Serial.println("Erro ao interpretar JSON");
        return;
    }

    bool solicitar = doc["informacoes"];

    if(solicitar){
        Serial.println("Solicitação de informações recebida");
        publishReport();
    }
}

void connectWiFi(){
    if(WiFi.status() == WL_CONNECTED) return;

    Serial.println("[WIFI] Conectando...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
        attempts++;
        if(attempts > 40){
            Serial.println("\n[WIFI] Timeout. Reiniciando tentativa.");
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            attempts = 0;
        }
    }

    Serial.println("\n[WIFI] Conectado");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WIFI] RSSI: ");
    Serial.println(WiFi.RSSI());
}

void connectAWS(){
    if(client.connected()) return;

    Serial.println("[AWS] Configurando TLS...");
    net.setCACert(AWS_ROOT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_PRIVATE_KEY);

    client.setServer(AWS_ENDPOINT, AWS_PORT);
    client.setCallback(messageCallback);

    Serial.println("[AWS] Conectando ao broker MQTT...");

    while(!client.connected()){
        Serial.print("[AWS] Tentando conectar...");
        if(client.connect(AWS_CLIENT_ID)){
            Serial.println(" conectado");
            if(client.subscribe(topicSub.c_str())){
                Serial.print("[AWS] Subscrito em: ");
                Serial.println(topicSub);
            } else {
                Serial.println("[AWS] Falha ao subscrever");
            }
        } else {
            Serial.print(" falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 2s");
            delay(2000);
        }
    }
}

void publishReport(){
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    if(isnan(temperatura) || isnan(umidade)){
        Serial.println("[DHT] Falha na leitura do sensor");
        return;
    }

    StaticJsonDocument<256> doc;
    doc["temperatura"] = temperatura;
    doc["umidade"] = umidade;
    doc["heap"] = ESP.getFreeHeap();
    doc["rssi"] = WiFi.RSSI();

    char buffer[256];
    serializeJson(doc, buffer);

    if(client.publish(topicPub.c_str(), buffer)){
        Serial.print("[MQTT] Publicado em ");
        Serial.print(topicPub);
        Serial.print(": ");
        Serial.println(buffer);
    } else {
        Serial.println("[MQTT] Falha ao publicar");
    }
}

void setup(){
    Serial.begin(115200);
    delay(1000);
    dht.begin();
    Serial.println("\n[BOOT] Iniciando ESP32");
    connectWiFi();
    connectAWS();
}

void loop(){
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("[WIFI] Conexão perdida");
        connectWiFi();
    }

    if(!client.connected()){
        Serial.println("[AWS] Conexão MQTT perdida");
        connectAWS();
    }

    client.loop();

    if(millis() - lastReport > REPORT_INTERVAL){
        lastReport = millis();
        publishReport();
    }
}