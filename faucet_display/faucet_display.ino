#include "CustomLCD_ESP32.h"
#include "Paint.h"

#define SV_1 25         //B-1A
#define SV_2 26         //B-1B 
#define IR_1 32         //적외선센서 들올 때
#define IR_2 33         //적외선센서 나갈 때
#define SENSOR 27
#define TIMER 30000
#define SOAP 5000
#define COOLTIME 35000

CustomLCD LCD128;

bool trigger = false;

int rinse = 0;

long currentMillis = 0;   //1초를 측정하기 위한 현제 millis
long previousMillis = 0;  //1초를 측정하기 위한 과거 millis
int interval = 1000;      //현제 millis와 과거 millis의 차이값 1초
int value = 0;

volatile bool solstate = 0;           // 솔 벨브 상태
volatile unsigned long solMillis;   // Mode 1&2의 시작을 감지하는 변수
volatile unsigned long timerMillis; // Mode3를 감지하는 변수
volatile int Mode = 0;              //비누 대기상태[시작, 5초 이내, 쿨타임 이내]

void IRAM_ATTR IRsignal_1();
void IRAM_ATTR IRsignal_2();

void setup() {
  // Set pin mode 
  
  pinMode(IR_1, INPUT);
  pinMode(IR_2, INPUT);
  pinMode(SV_1, OUTPUT);
  pinMode(SV_2, OUTPUT);
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(IR_1), IRsignal_1, RISING); 
  attachInterrupt(digitalPinToInterrupt(IR_2), IRsignal_2, RISING);
  digitalWrite(SV_1, LOW);
  digitalWrite(SV_2, LOW);
  Serial.begin(115200);
  LCD128.begin();
  LCD128.Clear(0);
} 
void loop() {
  currentMillis = millis();
  if(currentMillis - previousMillis > interval){
    previousMillis = millis();
    Serial.print("Mode : ");
    Serial.println(Mode);
  }
  if(trigger){
    Serial.println("===============================");
    rinse = 0;    
    trigger = false;
    LCD128.Clear(0);
    DrawString(LCD128, 120, 100, "Waiting", WHITE, true,2);
   }
  SolvalveStep();
  roundLCD();
  
}
void IRAM_ATTR IRsignal_1(){
  solstate = 1;
  // solscan 이 1일 때만 체크
  if(Mode == 0) solMillis = millis(); // 최초 감지 시간을 체크
}
void IRAM_ATTR IRsignal_2(){
  solstate = 0;
}
void roundLCD() {
  static int tempCount = 0, prevMode = -1;
  int count;
  switch(Mode)
  {
    case 0:
      if(Mode != prevMode) // Mode 2로 넘어오는 최초 1회 출력
      {
        // WAITING 출력
        LCD128.Clear(0);
        if(trigger){
          DrawString(LCD128, 120, 100, "GREAT", WHITE, true,3);
        }
        else{
          DrawString(LCD128, 120, 100, "Waiting", WHITE, true,2);
        }
      }
      break;
    case 1:
      // 5초 카운팅 출력
      count = (millis() - solMillis) / 1000;
      if(tempCount != count && count <= (SOAP / 1000)) // 1초가 바뀌는 시점에만 각 최초 1회 출력
      {
        tempCount = count;
        // count 출력
        LCD128.Clear(0);
        DrawString(LCD128, 120, 100, (String)(5-count), GREEN, true, 8);
      }
      break;
    case 2:
      count = (millis() - solMillis) / 1000;
      if(Mode != prevMode &&  count <= (COOLTIME / 1000)) // Mode 2로 넘어오는 최초 1회 출력
      {
        // SOFT  출력
        LCD128.Clear(0);
        DrawString(LCD128, 120, 50, "SOAP", YELLOW, true, 2);
      }
      if(tempCount != count) // 1초가 바뀌는 시점에만 각 최초 1회 출력
      {
        tempCount = count;
        DrawRect(LCD128, 50, 100, 140, 80, BLACK, true);
        //LCD128.Clear(0);        //다 지우지말고 숫자 카운트만 지워야함       
        DrawString(LCD128, 120, 100, (String)(35-count), YELLOW, true, 7);
      }        
      break;
    case 3:
      // 30초 카운팅 출력
      count = (millis() - timerMillis) / 1000;
      if(Mode != prevMode){
        LCD128.Clear(0);
        DrawString(LCD128, 120, 50, "RINSE AWAY", GRAY, true, 2);
      }
      if(tempCount != count) // 1초가 바뀌는 시점에만 각 최초 1회 출력
      {
        tempCount = count;      
        DrawRect(LCD128, 50, 100, 140, 80, BLACK, true);
          //LCD128.Clear(0);          
        DrawString(LCD128, 120, 100, (String)(30-count), GRAY, true, 7);
            
            // count 출력
      }      
      break; 
  }
  prevMode = Mode;
}

void SolvalveStep() {
  
  switch(Mode)
  {
    case 0: // 감지되면 Mode -> 1
      
        if(solstate) Mode = 1;
        solvalveonoff(0);
      break;
    case 1: // 5초 이내의 경우 원하는데로 제어가능
      if(millis() - solMillis <= SOAP) solvalveonoff(solstate);
      else // 5초가 지나면 자동으로 꺼짐
      {
        solvalveonoff(0);
        Mode = 2;
        solstate = 0;
      }
      break;
    case 2: // 5초 이후 30초 이내에 원하는데로 제어 가능 / 30초가 지나면 Mode -> 3
      if(millis() - solMillis <= COOLTIME)
      {
        solvalveonoff(solstate);         
      }
      else {     
        Mode = 3;
        timerMillis = millis(); // 30 피크에 대한 최초 시간을 체크
      }
      break;
    case 3: // 대기하다가 출수 후 정지하면 Mode ->0
      if(millis() - timerMillis <= TIMER) //행굼 대기
      {
        if(solstate == true){             //이미 헹구고 있거나 출수가 시작되면 헹굼모드로 변경
          solvalveonoff(solstate);
          rinse = 1; 
          }
        else if(rinse == 1){          //행굼모드일때 다 헹구고 가면 Mode = 0
          solvalveonoff(solstate);
          if(solstate == 0){
            Mode = 0;
            trigger = true;
            }
          }
      }
      else if(millis() - timerMillis > TIMER){  //헹굼 대기 상태가 만료되면 Mode = 0
        Mode = 0;
        trigger = true;
      }
      break;
  } 
}

void solvalveonoff(bool state){
  digitalWrite(SV_1, !state);
  digitalWrite(SV_2, state); 
}
