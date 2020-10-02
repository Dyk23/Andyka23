#define BLYNK_PRINT Serial
#define SPMDataLength 16
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_INA219.h>
#include <Wire.h>

char auth[] = "Q-Jvv4DRyqVXY4eTwkRPrcrxzbH0o0Oo";
char ssid[] = "abc";
char pass[] = "12345678";

byte SolarData [SPMDataLength];
String nilai;
float iradiasi;

float shuntvoltage1 = 0;
float busvoltage1 = 0;
float current_mA1 = 0;
float loadvoltage1 = 0;
float power_mW1 = 0;

float shuntvoltage2 = 0;
float busvoltage2 = 0;
float current_mA2 = 0;
float loadvoltage2 = 0;
float power_mW2 = 0;

float suhu1 = 0;
float suhu2 = 0;

float luaspermukaan = 0.495;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_INA219 ina219_1;
Adafruit_INA219 ina219_2(0x41);
SoftwareSerial mySerial (16, 17);

#define PIN_UPTIME V6

void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

BLYNK_READ(PIN_UPTIME)

{
  Blynk.virtualWrite(PIN_UPTIME, millis() / 1000);
}
BlynkTimer timer;

void myTimerEvent()
{
  Blynk.virtualWrite(V0, power_mW1);
  Blynk.virtualWrite(V1, power_mW2);
  Blynk.virtualWrite(V2, suhu1);
  Blynk.virtualWrite(V3, suhu2);
  Blynk.virtualWrite(V4, eff1);
  Blynk.virtualWrite(V5, eff2);
  Blynk.virtualWrite(V6, Irradiasi);
  //sending to Blynk
}

void setup()
{
  MySerial.begin(9600);
  Wire.begin();
  ina219_1.begin();
  ina219_2.begin();
  mlx.begin();

  TCA9548A(0);
  TCA9548A(1);

  uint32_t currentFrequency;
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  mySerial.readBytes(SolarData, SPMDataLength);
  float Irradiasi = getData() / 10;

  shuntvoltage1 = ina219_1.getShuntVoltage_mV();
  busvoltage1 = ina219_1.getBusVoltage_V();
  current_mA1 = ina219_1.getCurrent_mA();
  power_mW1 = ina219_1.getPower_mW();
  loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);

  shuntvoltage2 = ina219_2.getShuntVoltage_mV();
  busvoltage2 = ina219_2.getBusVoltage_V();
  current_mA2 = ina219_2.getCurrent_mA();
  power_mW2 = ina219_2.getPower_mW();
  loadvoltage2 = busvoltage2 + (shuntvoltage2 / 1000);

  eff1=power_mW1/(luaspermukaan*Irradiasi);
  eff2=power_mW2/(luaspermukaan*Irradiasi);

  TCA9548A(0);
  suhu1 = mlx.readObjectTempC();

  TCA9548A(1);
  suhu2 = mlx.readObjectTempC();

  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
float getData(){
  nilai = "";
  float irradiance = 0;
  for(int i = 9 ; i <13 ; i++){
    nilai += String(konversiNilai(SolarData[i]));  
  }
  nilai += "." + String(konversiNilai(SolarData[13]));
//  Serial.print("Raw Data : ");Serial.println(nilai);
  irradiance = nilai.toFloat();
  return irradiance;
}
int konversiNilai(int x){
//  Serial.print("Inputan : ");Serial.println(x);
  if (x == 159){
    return 0;
  }
  if (x == 157){
    return 1;
  }
  if (x == 155){
    return 2;
  }
  if (x == 153){
    return 3;
  }
  if (x == 151){
    return 4;
  }
  if (x == 149){
    return 5;
  }
  if (x == 147){
    return 6;
  }
  if (x == 145){
    return 7;
  }
  if (x == 143){
    return 8;
  }
  if (x == 141){
    return 9;
  }
  return -1;
}
