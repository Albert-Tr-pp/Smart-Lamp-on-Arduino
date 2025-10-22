//Lamp.h
#ifndef LAMP_H
#define LAMP_H

#include "Libraries.h"

class Lamp {
  private:
    int prPin;

  public:
    Lamp(int prPin);

    uint16_t getPrData();
    void printPrData(uint16_t PrData);

//setters & getters
    void setPrPin(int prPin);
    int getPrPin();
};

#endif