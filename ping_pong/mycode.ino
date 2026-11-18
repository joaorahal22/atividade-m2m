#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Convidado iot"; 
const char* password = "CONFIGURE_SUA_SENHA";

String IP_DO_COLEGA = "192.168.1.128"; 

ESP8266WebServer server(80);

unsigned long ultimoTempo = 0;
const unsigned long intervalo = 10000;
void setup() {
  Serial.begin(9600);
  
  
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  
  Serial.print("Seu Endereço IP: ");
  Serial.println(WiFi.localIP()); 

  server.on("/toggle", HTTP_GET, []() {
    int estadoAtual = digitalRead(2);
    digitalWrite(2, !estadoAtual);
    server.send(200, "text/plain", "OK");
    Serial.println("Requisição GET recebida! LED invertido.");
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado na porta 80.");
}

void loop() {

  server.handleClient();

  if (millis() - ultimoTempo >= intervalo) {
    if (WiFi.status() == WL_CONNECTED && IP_DO_COLEGA != "192.168.X.X") {
      WiFiClient client;
      HTTPClient http;

      String url = "http://" + IP_DO_COLEGA + "/toggle";
      Serial.println("Enviando GET para: " + url);
      
      http.begin(client, url);
      int codigoResposta = http.GET();

      if (codigoResposta > 0) {
        String payload = http.getString();
        Serial.printf("Resposta do colega: %d - %s\n", codigoResposta, payload.c_str());
      } else {
        Serial.printf("Erro na requisição: %s\n", http.errorToString(codigoResposta).c_str());
      }

      http.end();
    }
    ultimoTempo = millis();
  }
}
