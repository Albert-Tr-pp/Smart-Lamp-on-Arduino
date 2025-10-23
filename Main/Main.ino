//Main.ino
#include "Libraries.h"
#include "Lamp.h"

#define PR_PIN A3
#define MS_PIN 4

Lamp lamp(PR_PIN, MS_PIN); 

void setup() {
  Serial.begin(9600);
}

void loop() {
  // lamp.printPrData(lamp.getPrData());

  // delay(100);

  lamp.printMsData(lamp.getMsData());

  delay(100);
}
