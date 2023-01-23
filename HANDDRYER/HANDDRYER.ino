#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <MQUnifiedsensor.h>

#define         Board                   ("ESP-32") // Wemos ESP-32 or other board, whatever have ESP32 core.
#define         Pin                     (32) //check the esp32-wroom-32d.jpg image on ESP32 folder 
#define         Type                    ("MQ-4") //MQ4 또는 기타 MQ 센서, 이를 변경하는 경우 a 및 b 값을 확인하십시오.
#define         Voltage_Resolution      (3.3) // 3V3 <- IMPORTANT. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define         ADC_Bit_Resolution      (12) // ESP-32 bit resolution. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define         RatioMQ4CleanAir        (4.4) //RS / R0 = 60 ppm
#define SS_PIN  21  // ESP32 pin GIOP21 
#define RST_PIN 22 // ESP32 pin GIOP22 
#define STEP 5
#define DIR 17
#define DISPENSERIR 33
#define IRCOOLTIME 7000
#define GASCOOLTIME 600000
#define SWITCH 16
#define DYE_IR 35 

MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

const char* Host = "http://wmsadmin.co.kr/";

long irmillis = 0;
long gasmillis = 0;
double T = 0.0001, RadA = 0.024, RadB = 0.0754, Length = 160, Remain;

int Gas_State = 0;

MFRC522 rfid(SS_PIN, RST_PIN);

String UID = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  MQ4.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ4.setA(1012.7); MQ4.setB(-2.786); // Configurate the ecuation values to get H2 concentration
  MQ4.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ4.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    Serial.print(".");
  }
  MQ4.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}

   
  WiFi.begin("wifi name", "wifi Pw");
  
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WIFI OK");
  
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  Serial.println("Tap RFID/NFC Tag on reader");
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(SWITCH,INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(SWITCH) == HIGH) Length = 160;
  
  
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      NFCUID();      
    } 
  }
  
  double IRvalue = IRdistance();
  
  if(millis()-irmillis >= IRCOOLTIME){
      if(IRvalue <= 11){
      Serial.println(IRvalue);
      stepmoter(getDeg(0.2));
      irmillis = millis();
      Remain = Length / 160 * 100;
     
      String Path = "test_remain?id=" + String(Remain);
      Serial.println(Path);
      sendHTTPData(Path);
      }
  }
  if(millis()-gasmillis >= GASCOOLTIME){
    GASSENSOR();
    gasmillis = millis();
  }
}
double getDeg(double curCut)
{
  // 예외처리
  if(Length < 0) return 0;
  // 외곽 반지름
  RadB = sqrt(( (Length * T) + (PI * pow(RadA, 2)) ) / PI);
  // 외곽 길이 (한바퀴 기준)
  double LengthB = 2*RadB*PI;
  // 자르는 길이가 외곽 길이를 넘을 때 한바퀴를 뺀 나머지를 재귀 호출
  if(LengthB < curCut)
  {
    // 외곽 길이를 뺀다
    Length -= LengthB;
    // 외곽 한바퀴를 넘어서는 나머지를 계산한다.
    return 360.0 + getDeg(curCut - LengthB);
  }
  // 외곽 길이를 넘지 않으면 회전각 출력
  else
  {
    Length -= curCut;
    return curCut / LengthB * 360.0;
  }
}
void stepmoter(double GETDEG){
  int steps = GETDEG / 360 * 200;

  digitalWrite(DIR, LOW);
  for(int x = 0; x < steps; x++)  
    {  
        digitalWrite(STEP, HIGH);  
        delayMicroseconds(25000);  
        digitalWrite(STEP, LOW);  
        delayMicroseconds(25000);  
    }
}

double IRdistance(void){ //적외선 센서 거리 측정
  int sum = 0;
  for(int i = 0; i < 10; i++) sum += analogRead(DISPENSERIR);
  sum /= 10;
  double volt = sum * 5.0 / 4096;
  if(volt > 3.0) volt = 3.0;
  else if(volt < 0.4) volt = 0.4;
  volt -= 0.4;
  double distance = volt * 0.05 + 0.01;
  distance = 1 / distance;
  return distance;
}
void NFCUID(void){
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        Serial.print("RFID/NFC Tag Type: ");
        Serial.println(rfid.PICC_GetTypeName(piccType));
  
        // print NUID in Serial Monitor in the hex format
        Serial.print("UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
          UID += String(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
        Serial.print("String : ");
        Serial.println(UID);
        String Path = "nfc_recieve?id=" + String(UID) + ",HAND001";
        Serial.println(Path);
        sendHTTPData(Path);

        UID = "";
        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
}

void GASSENSOR(void){
  int gassum = 0;
  for(int i = 0; i < 50; i++){
    MQ4.update();
    gassum += MQ4.readSensor();
    }
  Gas_State = gassum/50; 
  String Path = "test_gassensor?id=" + String(Gas_State);
  Serial.println(Path);
  sendHTTPData(Path);
}

bool sendHTTPData(String raw)  //서버에 송신 함수
{
  if (WiFi.status() != WL_CONNECTED) return false;
  bool result = false;
  HTTPClient http;
  String Path = Host + raw;
  http.begin(Path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) result = true;
  http.end();
  return result;
}
