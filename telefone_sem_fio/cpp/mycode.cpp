#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <errno.h>
#include <limits.h>

const char* WIFI_NOME  = "Convidado iot";
const char* WIFI_SENHA = "CONFIGURE_SUA_SENHA";

// Dados do SEU ESP8266
IPAddress ipLocal(192, 168, 1, 163);
IPAddress gateway(192, 168, 1, 1);
IPAddress mascara(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

IPAddress ipProximoESP(192, 168, 1, 174);


ESP8266WebServer servidor(80);

bool envioPendente = false;
long proximoValor = 0;
unsigned long momentoRecebimento = 0;

const unsigned long TEMPO_ESPERA = 2000;


void receberRelay() {
  if (!servidor.hasArg("valor")) {
    servidor.send(
      400,
      "text/plain",
      "Informe o valor. Exemplo: /relay?valor=5"
    );
    return;
  }

  if (envioPendente) {
    servidor.send(
      503,
      "text/plain",
      "Ainda estou aguardando para enviar o valor anterior."
    );
    return;
  }

  String texto = servidor.arg("valor");
  char* fimConversao;

  errno = 0;
  long valorRecebido = strtol(texto.c_str(), &fimConversao, 10);

  if (
    texto.length() == 0 ||
    *fimConversao != '\0' ||
    errno == ERANGE ||
    valorRecebido == LONG_MAX
  ) {
    servidor.send(
      400,
      "text/plain",
      "O parametro valor deve ser um numero inteiro valido."
    );
    return;
  }

  Serial.print("Valor recebido: ");
  Serial.println(valorRecebido);

  proximoValor = valorRecebido + 1;

  Serial.print("Valor que sera enviado: ");
  Serial.println(proximoValor);

  momentoRecebimento = millis();
  envioPendente = true;

  servidor.send(
    200,
    "text/plain",
    "Recebido: " + String(valorRecebido) +
    "\nProximo valor: " + String(proximoValor) +
    "\nEnvio em 2 segundos."
  );
}


void enviarParaProximoESP() {
  if (ipProximoESP == IPAddress(0, 0, 0, 0)) {
    Serial.println("Configure o IP do proximo ESP no codigo.");
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Falha no envio: Wi-Fi desconectado.");
    return;
  }

  String endereco = "http://" + ipProximoESP.toString() +
                    "/relay?valor=" + String(proximoValor);

  Serial.print("Enviando para: ");
  Serial.println(endereco);

  WiFiClient cliente;
  HTTPClient http;

  if (!http.begin(cliente, endereco)) {
    Serial.println("Nao foi possivel iniciar a requisicao HTTP.");
    return;
  }

  http.setTimeout(1500);

  int codigoHTTP = http.GET();

  if (codigoHTTP > 0) {
    Serial.print("Resposta HTTP: ");
    Serial.println(codigoHTTP);
    Serial.println(http.getString());
  } else {
    Serial.print("Erro no envio: ");
    Serial.println(http.errorToString(codigoHTTP));
  }

  http.end();
}


void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("Iniciando ESP8266...");

  WiFi.mode(WIFI_STA);

  if (!WiFi.config(ipLocal, gateway, mascara, dns)) {
    Serial.println("Falha ao configurar o IP fixo.");
  }

  WiFi.begin(WIFI_NOME, WIFI_SENHA);

  Serial.print("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado!");

  Serial.print("IP deste ESP: ");
  Serial.println(WiFi.localIP());

  Serial.print("IP do proximo ESP: ");
  Serial.println(ipProximoESP);

  servidor.on("/relay", HTTP_GET, receberRelay);

  servidor.on("/", HTTP_GET, []() {
    servidor.send(
      200,
      "text/plain",
      "ESP8266 funcionando. Use /relay?valor=0"
    );
  });

  servidor.begin();
  Serial.println("Servidor HTTP iniciado.");
}


void loop() {
  servidor.handleClient();

  if (envioPendente &&
      millis() - momentoRecebimento >= TEMPO_ESPERA) {
    envioPendente = false;
    enviarParaProximoESP();
  }
}
