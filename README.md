# ESP32 + AWS IoT Core + MQTT (TLS) – Monitoramento com DHT11

Projeto para aquisição periódica de temperatura e umidade utilizando **ESP32** e sensor **DHT11**, com publicação dos dados via **MQTT sobre TLS** para a **AWS IoT Core**. O dispositivo também responde a requisições remotas de dados através de tópico MQTT específico.

## Arquitetura

O firmware executa as seguintes funções:

* Conexão Wi-Fi com mecanismo simples de retry e timeout.
* Conexão segura (TLS mutual) com **AWS IoT Core** usando:

  * Root CA
  * Certificado do dispositivo
  * Chave privada
* Publicação periódica (30s) de:

  * Temperatura (°C)
  * Umidade (%)
  * Heap livre (bytes)
  * RSSI (dBm)
* Subscrição a um tópico de comando para envio sob demanda.
* Tratamento de mensagens JSON recebidas.

### Fluxo de comunicação

Dispositivo → `sendData/{AWS_CLIENT_ID}`
Nuvem → `receivedData/{AWS_CLIENT_ID}`

Se o payload recebido contiver:

```json
{
  "informacoes": true
}
```

o dispositivo publica imediatamente um novo relatório.

---

## Requisitos de Hardware

* ESP32
* Sensor DHT11
* Resistor de pull-up (caso necessário, conforme módulo)
* Conexão Wi-Fi estável

### Pinagem

* DHT11 DATA → GPIO 4
* VCC → 3.3V
* GND → GND

---

## Dependências

Instalar as seguintes bibliotecas na Arduino IDE:

* WiFi (já incluída no core ESP32)
* WiFiClientSecure (já incluída no core ESP32)
* PubSubClient
* ArduinoJson
* DHT sensor library

---

## Instalação do Core ESP32

É necessário adicionar o suporte oficial da Espressif na Arduino IDE:

1. Abrir **Arquivo → Preferências**
2. No campo **URLs adicionais para Gerenciadores de Placas**, adicionar:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

3. Ir em **Ferramentas → Placa → Gerenciador de Placas**
4. Buscar por **ESP32 by Espressif Systems**
5. Instalar
