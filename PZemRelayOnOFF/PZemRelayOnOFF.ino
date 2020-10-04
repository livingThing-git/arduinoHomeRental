#include <PZEM004Tv30.h>

#include <EasyScheduler.h>
PZEM004Tv30 pzem(11, 12);
Schedular Task1;
int relay =13;

void setup() {                 
  pinMode(relay, OUTPUT);
  Task1.start();
  Serial.begin(9600);

}

void loop() //{
 //Task1.check(Power,500); 
 {
  float pf = pzem.pf();
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  if (Serial.available()) {
    char c = Serial.read();
    if (c=='0') {
      digitalWrite(relay, LOW);
      Serial.println("relay turned off!");
    }
    if (c=='1') {
      digitalWrite(relay, HIGH);
      Serial.println("relay turned on!");
    }
    
    if (c=='3') {
      Serial.print("PF: "); Serial.println(pf);
    } //else {
       // Serial.println("Error reading power factor");
    
    //}
  }
 yield();
}
