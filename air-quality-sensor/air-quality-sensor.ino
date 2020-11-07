#include <ArduinoJson.h>

#define        COV_RATIO                       0.2            // ug / mmm / mv
#define        NO_DUST_VOLTAGE                 400            // mv
#define        SYS_VOLTAGE                     5000           

const int iled = 7;                                            // drive the led of sensor
const int vout = A0;                                           // analog input
const int gas_ain = A3;
const int delay_second = 10000;

float density, voltage;
int   adcvalue, ad_value;

// private function
int Filter(int m) {
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0) {
    
    flag_first = 1;
    for(i = 0, sum = 0; i < _buff_max; i++) {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  } else {
    
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++) {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}


void setup(void) {
  
  pinMode(iled, OUTPUT);
  pinMode(gas_ain,INPUT);
  digitalWrite(iled, LOW); // iled default is closed
  
  Serial.begin(115200);
  // Serial.print("*********************************** WaveShare ***********************************\n");
}

float hp2y(void) {
  // get adcvalue
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);
  
  adcvalue = Filter(adcvalue);
  // convert voltage (mv)
  voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;
  
  // voltage to density
  if(voltage >= NO_DUST_VOLTAGE) {
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;
  } else {
    density = 0;
  }
  
  // Serial.print("current dust concentration is: ");
//  Serial.print(density);
//  Serial.println(" ug/m3");  
}

int mp135(void){  
  ad_value = analogRead(gas_ain);
//  Serial.print(ad_value);
//  Serial.println(" ppm");
}

void loop(void){
  hp2y();
  mp135();
  StaticJsonDocument<256> doc;

  doc["density"] = int(density);
  doc["ad_value"] = ad_value;

  String output = "";
  serializeJson(doc, output);
  const char* c = output.c_str();
  int bytesSent = Serial.write(c);
//  Serial.println(bytesSent);
  delay(delay_second);
}
