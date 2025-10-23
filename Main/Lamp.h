//Lamp.h
#ifndef LAMP_H
#define LAMP_H

#include "Libraries.h"

class Lamp {
  private:
    uint8_t prPin;
    uint8_t msPin;

  public:
    Lamp(uint8_t prPin, uint8_t msPin);

    uint16_t getPrData();
    void printPrData(uint16_t prData);

    uint16_t getMsData();
    void printMsData(uint16_t msData);

//setters & getters
    void setPrPin(uint8_t prPin);
    void setMsPin(uint8_t msPin);

    uint8_t getPrPin();
    uint8_t getMsPin();
};

#endif