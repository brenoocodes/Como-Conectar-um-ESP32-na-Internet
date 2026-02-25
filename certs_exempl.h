#ifndef CERTS_H
#define CERTS_H

const char* WIFI_SSID = "SEU_WIFI";
const char* WIFI_PASSWORD = "SUA_SENHA";


const char* AWS_ENDPOINT = "xxxxxxxxxxxxx-ats.iot.us-east-2.amazonaws.com";
const int AWS_PORT = 8883;
const char* AWS_CLIENT_ID = "esp32-client";


static const char AWS_ROOT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
COLE_AQUI_O_CONTEUDO_DO_AmazonRootCA1.pem
-----END CERTIFICATE-----
)EOF";


static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
COLE_AQUI_O_CONTEUDO_DO_certificate.pem.crt
-----END CERTIFICATE-----
)KEY";


static const char AWS_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
COLE_AQUI_O_CONTEUDO_DO_private.pem.key
-----END RSA PRIVATE KEY-----
)KEY";

#endif