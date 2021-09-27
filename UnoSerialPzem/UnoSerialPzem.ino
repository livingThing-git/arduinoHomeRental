#include <Scheduler.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
/* Use software serial for the PZEM
 * Pin 11 Rx (Connects to the Tx pin on the PZEM)
 * Pin 12 Tx (Connects to the Rx pin on the PZEM)
*/
PZEM004Tv30 pzem(11, 12);
SoftwareSerial UnoSerial(3,2); // RX | TX
const int reset_energy_pin = 4;
const int reboot_uno = 5;
const int restart_interval = 300000;
void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  UnoSerial.begin(57600); 
  Scheduler.startLoop(loop2);
}

float get_measurement(float measurement){
  return (isnan(measurement))? -1.0 : measurement;
}

void loop() {
  // put your main code here, to run repeatedly:
  float voltage = get_measurement(pzem.voltage());
  float energy = get_measurement(pzem.energy());  
  delay(500);
  UnoSerial.print(voltage);
  UnoSerial.print(" ");
  UnoSerial.print(energy);
  UnoSerial.print("\n");
  delay(500);
  if(digitalRead(reset_energy_pin)==HIGH)
    pzem.resetEnergy();
}

void(* resetFunc) (void) = 0;//declare reset function at address 0

// Task no.2: blink LED with 0.1 second delay.
void loop2() {
  
  delay(restart_interval);
  Serial.println("resetting");
  resetFunc();  //call reset
}
