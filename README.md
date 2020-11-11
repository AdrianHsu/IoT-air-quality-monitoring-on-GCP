# IoT-air-quality-monitoring-on-GCP

> 🌬💫 Collective Indoor Air Quality Monitoring System on Google Cloud (Google Pub/Sub, Google Power Shell)



## Introduction

Air quality has a huge influence on people’s life, especially in this COVID era. Bad air quality could cause Asthma and other respiratory symptoms. Thus, how we appropriately monitor indoor air quality in a real-time matter is definitely a critical topic to explore.

We will implement and install multiple low-cost sensors in different rooms at home, and then conduct a **real-time, collective air quality monitoring system** to check the pollutants of air pollutions. We will upload data and visualize them to Google Cloud Platform (GCP).



## Objectives

We would like to build an indoor air quality monitoring system having the following features:

* Low-cost: using cost-effective IOT board & sensors
* Sync with Cloud: gather and visualize the sensing data in GCP
* Notification: notify users when the air quality becomes too badTriggering: turn on air purifier automatically



## Main Hardware

* Arduino UNO R3 [Link](https://www.amazon.com/Arduino-A000066-ARDUINO-UNO-R3/dp/B008GRTSV6/ref=pd_rhf_ee_p_img_6?_encoding=UTF8&psc=1&refRID=69KJ1619BGYYVM05VQ71)
* ESP8266 NodeMCU [Link](https://www.amazon.com/gp/product/B081CSJV2V/ref=ppx_yo_dt_b_asin_title_o09_s01?ie=UTF8&psc=1)
* MQ-135 Gas Sensor [Link](https://www.amazon.com/gp/product/B07RKQZTMQ/ref=ppx_yo_dt_b_asin_image_o09_s00?ie=UTF8&psc=1)
* DHT22 / AM2032 [Link](https://www.amazon.com/gp/product/B018JO5BRK/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1)
* GP2Y Optical Dust Sensor [Link](https://www.amazon.com/gp/product/B07P94LCFZ/ref=ppx_od_dt_b_asin_title_s01?ie=UTF8&psc=1)



## Electronic Components

* Resistors [Link](https://www.amazon.com/gp/product/B072BL2VX1/ref=ppx_yo_dt_b_asin_title_o09_s01?ie=UTF8&psc=1)
* Capacitor 220 uF [Link](https://www.amazon.com/gp/product/B07YDY8K7T/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&psc=1)
* Breadboard Jumper Wires [Link](https://www.amazon.com/gp/product/B07GD2BWPY/ref=ppx_yo_dt_b_asin_title_o09_s01?ie=UTF8&psc=1)
* Breadboard [Link](https://www.amazon.com/gp/product/B07DL13RZH/ref=ppx_yo_dt_b_asin_image_o09_s01?ie=UTF8&psc=1)
* Arduino Power Supply Adapter [Link](https://www.amazon.com/gp/product/B018OLREG4/ref=ppx_yo_dt_b_asin_image_o09_s00?ie=UTF8&psc=1)
* USB 2.0 - Printer cable (ps. for powering our arduino from the laptop) [Link](https://www.amazon.com/gp/product/B00NH11KIK/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
* USB 2.0 - MicroUSB cable* [Link](https://www.amazon.com/AmazonBasics-Male-Micro-Cable-Black/dp/B0711PVX6Z/ref=sr_1_4?dchild=1&keywords=micro+usb+cable&s=electronics&sr=1-4)



*ps., Note that it has to be a data/sync cable, and not a charge-only cable. They are different.

## Directory Tree

```
$ Tree
.
├── README.md
├── air-quality-sensor
│   └── air-quality-sensor.ino
└── esp8266-lwmqtt
    ├── ciotc_config.h
    ├── esp8266_mqtt.h
    └── test-Esp8266-lwmqtt.ino

2 directories, 5 files
```



## Guidance for enabling Arduino IDE & ESP8266

* To develop on Arduino, you have to first set up your environment
* [Youtube: Getting Started | ESP8266 Programming Using Arduino IDE (Mac OSX and Windows)](https://www.youtube.com/watch?v=G6CqvhXpBKM&feature=emb_title&loop=0)
* Follow the above link to set up your (1) arduino IDE (2) support for ESP8266 in the IDE
* by running some examples (e.g., Blink) to make sure they works



## Included Library

How-to: within Arduino App, go to **Sketch -> Include Library -> Manage Libraries (Library Manager will pop up)**

* **MQTT** [Link](https://github.com/256dpi/arduino-mqtt): by Joel Gaehwiller, Version `2.4.7`
* **Google Cloud IoT Core JWT** [Link](https://github.com/GoogleCloudPlatform/google-cloud-iot-arduino): by Vladimir Korukov, Version `1.1.10`
* **ArduinoJson** [Link](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties): by Benoit Blanchon, Version `6.16.1` 
* **DHT** [Link](https://github.com/adafruit/DHT-sensor-library): by Adafruit, Version `1.4.0`



## Connect to GCP

Note that you have to fill in these values to connect your device to GCP. In `esp8266-lwmqtt/ciotc_config.h`: 

```c++
// WIFI
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Cloud iot details.
const char* project_id = "PROJECT_ID";
const char* location = "LOCATION";
const char* registry_id = "REGISTRY_ID";
const char* device_id = "DEVICE_ID";

const char* private_key_str =
  "PRIVATE_KEY_STR";
```

My setting is like:

```c++
const char* project_id = "iot-air-quality-monitor";
const char* location = "us-central1";
const char* registry_id = "air-quality-core";
const char* device_id = "atest-dev";
```

Note that you don't have to change the `primary_ca` and `backup_ca` variable value in this config file. They are fixed.

## Implementation

### Step 1. Arduino + Dust Sensor (GP2Y)

The code `air-quality-sensor.ino` seems bulky, but actually they are mostly taken directly from the GP2Y example. You can find the original example here [Link](https://www.waveshare.com/wiki/Dust_Sensor).

For the hardware part, we followed the **hardware installation** figure (https://content.instructables.com/ORIG/F1L/10D4/IQHIUL3L/F1L10D4IQHIUL3L.png?auto=webp&frame=1&fit=bounds&md=dd024e98eb9911e39179e4096187e358) shown in this tutorial [Link](https://www.instructables.com/How-to-Interface-With-Optical-Dust-Sensor/), This is when our resistor of `150ohm` and our capacitor of `220uF` comes into play. However, we found that their dust sensor has 6 fan-out, but ours has only 4. Thus, we tried to find the relationship between them and found the following mappings:



| Pin Number | Symbol | Description                   | Mapping to 6 fan-out version    |
| ---------- | ------ | ----------------------------- | ------------------------------- |
| 1          | V_CC   | [Red] Supply Voltage          | VCC (pin #6) & V-LED (pin #1)   |
| 2          | GND    | [Black] Ground                | GND (pin #4) & LED_GND (pin #2) |
| 3          | A_OUT  | [Blue] Analog voltage output  | A_OUT (or V0, pin #5)           |
| 4          | I_LED  | [Yellow] LED terminal current | I_LED (or LED, pin #3)          |

* Firstly, we connect our V_CC to our Arduino's `5V` supply voltage
* Secondly, we connect our GND to our Arduino's `GND` ground
* Then, for our analog output, we choose to connect it with Arduino's analog `A0`
* Finally, we connect our LED terminal to our Arduino's digital `D7`



As you can see you the `.ino` source code, we have the following config for the dust sensor:

```c++
const int iled = 7;        // drive the led of sensor
const int vout = A0;       // analog 
...
```



After you finished the hardware setting part, we then turn to our software programming. It is quite straightforward as the example code provided by the suppliers works well ([Link](https://www.waveshare.com/wiki/Dust_Sensor)). Basically, just copy and paste the code then you can successfully run your dust sensor.



#### Evaluation

To make sure your dust sensor works successfully, the only thing you need to do is to put a pen into the hole of our dust sensor. As it detects something in the air, the value will goes up.



### Step 2. Arduino + Gas Sensor (MQ-135)

It is pretty trivial to integrate your dust sensor with arduino compared to the dust sensor. MQ-135 gas sensor senses the gases like *ammonia, nitrogen oxides (NOx), alcohol, aromatic compounds, sulfide, and smoke* (氨氣、氮氧化合物、醇類、芳族化合物、硫化物和煙霧). MQ-135 gas sensor can be implemented to detect benzene, steam and other harmful gases. Please see our code reference here [Link](https://www.waveshare.com/wiki/MQ-135_Gas_Sensor). The code shown below is enough to run the gas sensor.

```c++
int gas_ain=A3;

int ad_value;
void setup()
{
  pinMode(gas_ain,INPUT);
  Serial.begin(9600);
}
void loop()
{
  ad_value=analogRead(gas_ain);
  Serial.println(ad_value);
  delay(500);
}
```

#### Evaluation

To make sure your gas sensor works successfully, just use your lighter and release some gas from it. The `ad_value` should jump to `> 500` compared to the original value (it is roughly 5~30 normally). 

### Step 3. ESP8266 Setup & Send Sample Messages to Google Pub/Sub

#### GCP part

* Enable the Cloud IoT Core API by opening the [Google Cloud IoT Core console](https://console.cloud.google.com/iot/)
* Next, create your device registry as described in [the Quickstart](https://cloud.google.com/iot/docs/quickstart) or by using the [Google Cloud SDK](https://cloud.google.com/sdk).
* Follow **Google Cloud IoT JWT** [Link](https://github.com/GoogleCloudPlatform/google-cloud-iot-arduino) for creating your .pem by openssl. 
* Once you've tested your Pub/Sub using PowerShell commands, you can turn to the ESP8266 code part.

#### ESP on arduino IDE part

* Make sure that you've imported the libraries shown in *Imported Library* section
* Arduino App -> File -> Examples -> Google Cloud IoT Core JWT -> Esp8266-lwmqtt
* fill in the project params according to your own GCP config as shown in *Connect to GCP* section
* then modify our `test-ESP8266-lwmqtt.iso` to satisfy our needs
  1. We want to get sensor data by reading our `Rx` on ESP8266. The data will be sent from the Arduino's `Tx`.
  2. We also will perform a small tweak on the data to insert a `timestamp` into our JSON object.
* toggle your Pub/Sub subscription results by pulling the messages from Google Power Shell by running:

```sh
$ gcloud pubsub subscriptions pull --limit 500 --auto-ack <your-subscription-name>
```

* ps., if you cannot connect to WIFI, it might be that your WIFI is on 5Ghz, and the ESP can't see it because it is only for **2.4 Ghz**.



### Step 4. Serial Communication between Arduino & ESP8266

* [Youtube: Serial communication between NodeMCU and Arduino](https://www.youtube.com/watch?v=9W6PiPFfVjg&loop=0)

This article briefly talks about how we do communication between these two devices [Link](https://iot-guider.com/esp8266-nodemcu/serial-communication-between-nodemcu-and-arduino/). It is more straightforward than we thought: we just utilize Arduino's `Tx` as the sendor, and connect it with our ESP8266's `Rx` as the receiver. Also don't forget to give a `3.3V` supply voltage and `GND` for the ESP8266 devices.

Note that you have to upload your different source code to these two devices separately, and that's why we need to make sure that we can upload code to both Arduino and ESP8266 directly. For instance, the minimum viable code for communication between these two is like:


### mcc-comm

```c++
#include<SoftwareSerial.h>

SoftwareSerial s(3,1);
void setup() {
  //Serial S Begin at 115200 Baud
  Serial.begin(115200);
}

void loop() {
  //Write 'abcde' to Serial
  String data = Serial.readString();
  Serial.println(data);
  delay(1000);
}
```

### uno-comm

```c++
void setup() {
  //Serial Begin at 9600 Baud 
  Serial.begin(115200);
}

void loop() {
  Serial.write("abcde"); //Read the serial data and store it
  delay(1000);
}
```

* One thing important to know is that when you are uploading code into ESP8266 or Arduino, please remove the Tx-to-Rx wire temporarily. Otherwise, you will not be able to upload the code to them.
* It will show something like:

```
warning: espcomm_sync failed
error: espcomm_open failed
error: espcomm_upload_mem failed
error: espcomm_upload_mem failed
```



### Step 5. Put values into JSON & Serialize to tranfer data

#### Serialization

* Firstly, you will need to include library `ArduinoJson` using the Library Manager in Arduino IDE.
* The serialzation part is very straightforward. In `air-quality-sensor.ino`:

```c++
StaticJsonDocument<256> doc;

doc["density"] = int(density);
doc["ad_value"] = ad_value;

String output = "";
serializeJson(doc, output);
const char* c = output.c_str();
int bytesSent = Serial.write(c);
```

* Basically, we create a json document, and then we put values into the dictionary. Finally we serialize it into a single string.
* Then, we write our string into our Tx on Arduino. 
* The ESP8266 should be able to receive our signal on its Rx.



#### Deserialization & add timestamp

* In `test-esp8266-lwmqtt.ino`:

```c++
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

```

* Just to clarify, the `iat` variable here is a `long` value that denotes the starting timestamp that we boot the device. It will fetch the date time through WIFI, as we know that our ESP8266 doesn't have a clocking system that could know what time it is at a given time. 
* Therefore, we will use the `iat` and a padding (in second) value to calculate the timestamp. We could not do that in our Arduino as it cannot even fetch time data, so we cannot help but deserialize the data to make it to become a JSON object again, and then insert our timestamp into the JSON. Finally, we still return it as a String.

### Step 6. DHT 22 for temperature & humidity monitoring

* We will want to do Temperature Monitoring With DHT22. It is pretty straightforward.
* You have to install this library [Link](https://github.com/adafruit/DHT-sensor-library) in your arduino manager to enable the monitor.
* Temperature is in Celsius, and Humidity is ranging from 0 to 100%.
* [Link](https://create.arduino.cc/projecthub/mafzal/temperature-monitoring-with-dht22-arduino-15b013)



## Some Sample Commands in Google Power Shell

* Follow this tutorial: [Link] (https://github.com/GoogleCloudPlatform/google-cloud-iot-arduino)

### Create Subscription

```sh
$ gcloud pubsub subscriptions create \
    projects/iot-air-quality-monitor/subscriptions/device-subscription \
    --topic=projects/iot-air-quality-monitor/topics/device-events

Created subscription [projects/iot-air-quality-monitor/subscriptions/device-subscription].
```

### Create EC pem

```sh
$ openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem
$ openssl ec -in ec_private.pem -pubout -out ec_public.pem
```

### Create Devices in IoT Core

* You could do this either in web GUI or using the command

```sh
$ gcloud iot devices create atest-dev --region=us-central1 \
  --registry=air-quality-core \
  --public-key path=ec_public.pem,type=es256
Created device [atest-dev]. 
```

## Result

* On your Google Power Shell, you could see the result like:

```
adrianhsu1995@cloudshell:~ (iot-air-quality-monitor)$ gcloud pubsub subscriptions pull --limit 500 --auto-ack device-subscription
┌───────────────────────────────────────────────────────────────────────────────────┬──────────────────┬──────────────┬────────────────────────────────────┬──────────────────┐
│                                        DATA                                       │    MESSAGE_ID    │ ORDERING_KEY │             ATTRIBUTES             │ DELIVERY_ATTEMPT │
├───────────────────────────────────────────────────────────────────────────────────┼──────────────────┼──────────────┼────────────────────────────────────┼──────────────────┤
│ {"density":16,"ad_value":5,"humidity":50,"temperature":20,"timestamp":1605123792} │ 1724380540105952 │              │ deviceId=atest-dev                 │                  │
│                                                                                   │                  │              │ deviceNumId=2580803067423718       │                  │
│                                                                                   │                  │              │ deviceRegistryId=air-quality-core  │                  │
│                                                                                   │                  │              │ deviceRegistryLocation=us-central1 │                  │
│                                                                                   │                  │              │ projectId=iot-air-quality-monitor  │                  │
│                                                                                   │                  │              │ subFolder=                         │                  │
└───────────────────────────────────────────────────────────────────────────────────┴──────────────────┴──────────────┴────────────────────────────────────┴──────────────────┘
```

