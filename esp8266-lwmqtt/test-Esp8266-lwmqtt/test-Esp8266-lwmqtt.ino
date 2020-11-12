/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include <CloudIoTCore.h>
#include<SoftwareSerial.h>

SoftwareSerial s(3,1);

#include "esp8266_mqtt.h"
#include "ArduinoJson.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

String getSensor(String data, unsigned long padding_in_second)
{
  DynamicJsonDocument doc(128);
  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    Serial.println(data);
    return "";
  }
  doc["timestamp"] = iat + padding_in_second;
//  Serial.println(iat + padding_in_second);

  String output = "";
  serializeJson(doc, output);
  const char* c = output.c_str();
  return c;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupCloudIoT(); // Creates globals for MQTT
  pinMode(LED_BUILTIN, OUTPUT);
}

unsigned long lastMillis = 0;
unsigned long timeSecond = 10000;

void loop()
{
  mqtt->loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!mqttClient->connected())
  {
    ESP.wdtDisable();
    connect();
    ESP.wdtEnable(0);
  }
  unsigned long mil = millis();
  if (mil - lastMillis > timeSecond)
  {
    String data = Serial.readString();
    String result = getSensor(data, mil / 1000);
    if(result != "") {
      Serial.println(result);
      publishTelemetry(result);
    } else {
      // skip
    }
    lastMillis = mil;
  }
}
