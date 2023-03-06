#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <esp_adc_cal.h>
#include "SPIFFS.h"
#include "Arduino.h"
#include "FS.h"
#include <PID_v1.h>

const char* ssid = "esp32";  // Nome da rede Wi-Fi do ponto de acesso
const char* password = "senha123";  // Senha da rede Wi-Fi do ponto de acesso


#define PIN_INPUT 34
#define PIN_OUTPUT 2

double Setpoint, Input, Output; // VARIÁVEIS DO PID
double Kp=5, Ki=2, Kd=2;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);


AsyncWebServer server(80);  // Criar o servidor web

void requestStyle(AsyncWebServerRequest *requeste){
  requeste->send(SPIFFS,"/style.css", "text/css");
}

void setup() {

  ledcAttachPin(2, 0);//Atribuimos o pino 2 ao canal 0.
  ledcSetup(0, 1000, 8);//Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits.

  Input = analogRead(PIN_INPUT);
  Setpoint = 100; // SP DE TEMPERATURA

  //turn the PID on
  myPID.SetMode(AUTOMATIC);


  Serial.begin(115200);

  SPIFFS.begin(true);

  // Configurar o ESP32 como ponto de acesso Wi-Fi
  WiFi.softAP(ssid, password);
  //WiFi.begin(ssid, password);

  Serial.print("Endereço IP do ponto de acesso: ");
  Serial.println(WiFi.softAPIP());

  // Configurar o servidor web para servir a página HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/style.css",requestStyle);

  // Configurar o servidor web para fornecer a temperatura atual
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    Input = analogRead(PIN_INPUT);
    Serial.print("INPUT: ");
    Serial.println(Input);
    myPID.Compute();
    ledcWrite(PIN_OUTPUT, Output);
    Serial.print("OUTPUT: ");
    Serial.println(Output);

    float voltage = Input * 3.3 / pow(2, 12);  // Converter para tensão
    float temperature = (voltage) * 100;  // Calcular a temperatura em graus Celsius
    String temperatureString = String(temperature, 1);//String(temperature, 1);  // Converter para uma string com uma casa decimal
    request->send(200, "text/plain", temperatureString);  // Enviar a temperatura como resposta
  });

  // Iniciar o servidor web
  server.begin();
}

void loop() {

}
