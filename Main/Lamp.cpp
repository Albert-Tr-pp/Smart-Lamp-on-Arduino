//Lamp.cpp
#include "Lamp.h"

//#include "Libraries.h"

Lamp::Lamp(uint8_t prPin, uint8_t msPin){
  setPrPin(prPin);
  setMsPin(msPin);
}

// -----------------------------------------------------------------------

uint16_t Lamp::getPrData(){
  uint16_t sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(prPin);
  }
  return sum / 10;
}

void Lamp::printPrData(uint16_t prData){
  Serial.print("ФОТОРЕЗИСТОР: ");
  Serial.println(prData);
}

// -----------------------------------------------------------------------

uint16_t Lamp::getMsData(){
  return digitalRead(msPin);
}

void Lamp::printMsData(uint16_t msData){
  Serial.print("ДВИЖЕНИЕ: ");
  Serial.println(msData);
}

// -----------------------------------------------------------------------

//setters & getters
void Lamp::setPrPin(uint8_t prPin){
  this->prPin = prPin;
}
void Lamp::setMsPin(uint8_t msPin){
  this->msPin = msPin;
}

uint8_t Lamp::getPrPin(){
  return prPin;
}
uint8_t Lamp::getMsPin(){
  return msPin;
}