//Lamp.cpp
#include "Lamp.h"

//#include "Libraries.h"

Lamp::Lamp(int prPin){
  setPrPin(prPin);
}

uint16_t Lamp::getPrData(){
  uint16_t sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(prPin);
  }
  return sum / 10;
}

void Lamp::printPrData(uint16_t PrData){
  Serial.print("ФОТОРЕЗИСТОР: ");
  Serial.println(PrData);
}

//setters & getters
void Lamp::setPrPin(int prPin){
  this->prPin = prPin;
}

int Lamp::getPrPin(){
  return prPin;
}