//Main.ino
#include "Libraries.h"
#include "Lamp.h"

#define PR_PIN A3

Lamp lamp(PR_PIN); 

void setup() {
  Serial.begin(9600);
}

void loop() {
  lamp.printPrData(lamp.getPrData());

  delay(1000);
}
