#include <EasyScheduler.h>

const int live_signal_pin = 2;
const int wake_up_pin = 13;
int unchange_cnt=0;

bool current_state=false;
bool prev_state=false;
Schedular Task1; // สร้างงาน Task

void setup() {
  // put your setup code here, to run once:
  pinMode(live_signal_pin, INPUT);
  pinMode(wake_up_pin, OUTPUT);
  Serial.begin(115200);
  Task1.start();
}

void loop() {
  Task1.check(live_signal_check, 1000); // ทำงานฟังก์ชั่น LED1_blink ทุก 1 วินาที  
}

void live_signal_check() {
  // put your main code here, to run repeatedly:  
  delay(1000);  
  if(digitalRead(live_signal_pin)==HIGH)
    current_state=true;
  else
    current_state=false;

  if (prev_state!=current_state){
    unchange_cnt=0;
    prev_state=current_state;
  } else {
    unchange_cnt++;
  }

  if (unchange_cnt>10) {
    digitalWrite(wake_up_pin, HIGH);
  } else if (unchange_cnt<10) {
    digitalWrite(wake_up_pin, LOW);
  }
}
