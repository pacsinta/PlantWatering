#include <Arduino.h>
#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "AsyncElegantOTA.h"

#define ssid "milkyway"
#define password "KntakPV-1922"

#define Led 2
#define motor_pin 25
#define timeServer "http://worldtimeapi.org/api/timezone/Europe/Budapest"
#define sleepTime 10*60*1000  // in milliseconds


AsyncWebServer server(80);

bool sleepEnable = false;

void helloBlink(){
  for (size_t i = 0; i < 3; i++)
  {
    digitalWrite(Led, HIGH);
    delay(500);
    digitalWrite(Led, LOW);
    delay(500);
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  pinMode(Led, OUTPUT);
  pinMode(motor_pin, OUTPUT);

  helloBlink();

  bool ledStatus = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    digitalWrite(Led, ledStatus);
    ledStatus = !ledStatus;
  }
  digitalWrite(Led, LOW);

  esp_sleep_enable_timer_wakeup(sleepTime);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world!");
  });

  // post request to handle motor control
  server.on("/motor1", HTTP_POST, [](AsyncWebServerRequest *request) {
    if(request->hasParam("motor1")){
      String motor1 = request->getParam("motor1")->value();
      Serial.println(motor1);
      if(motor1 == "1"){
        digitalWrite(motor_pin, HIGH);
        request->send(200, "text/plain", "Motor1 is on");
      }
      else{
        digitalWrite(motor_pin, LOW);
        request->send(200, "text/plain", "Motor1 is off");
      }
    }else{
      request->send(400, "text/plain", "No motor1 param");
    }
  });

  server.on("/sleep", HTTP_GET, [](AsyncWebServerRequest *request){
    sleepEnable = true;
    request->send(200, "text/plain", "Sleep mode enabled");
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
}


void loop() {
  if (sleepEnable) {
    sleepEnable = false;
    esp_deep_sleep_start();
  }
  delay(5000);
}