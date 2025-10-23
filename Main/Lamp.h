//Lamp.h
#ifndef LAMP_H
#define LAMP_H

#include "Libraries.h"

#define NUM_LEDS 64
#define M_WIDTH 8
#define M_HEIGHT 8

template<uint8_t M_PIN>
class Lamp {
  private:
    uint8_t prPin;
    uint8_t msPin;
    microLED<NUM_LEDS, M_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> matrix;

  public:
    Lamp(uint8_t prPin, uint8_t msPin)
    : matrix(M_WIDTH, M_HEIGHT, PARALLEL, LEFT_TOP, DIR_RIGHT)
    {
      setPrPin(prPin);
      setMsPin(msPin);
    }

  // -----------------------------------------------------------------------

  uint16_t getPrData(){
    uint16_t sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += analogRead(prPin);
    }
    return sum / 10;
  }

  void printPrData(uint16_t prData){
    Serial.print("ФОТОРЕЗИСТОР: ");
    Serial.println(prData);
  }

  // -----------------------------------------------------------------------

  uint16_t getMsData(){
    return digitalRead(msPin);
  }

  void printMsData(uint16_t msData){
    Serial.print("ДВИЖЕНИЕ: ");
    Serial.println(msData);
  }

  // -----------------------------------------------------------------------

  void testMatrix(){
    //тест матрицы
  }

  void turnOnMatrix(){
    //вкл матрицу
  }

  void turnOffMatrix(){
    //выкл матрицу
  }

  // -----------------------------------------------------------------------


  // -----------------------------------------------------------------------


  // -----------------------------------------------------------------------


  // -----------------------------------------------------------------------

  //setters
  void setPrPin(uint8_t prPin){
    this->prPin = prPin;
  }
  void setMsPin(uint8_t msPin){
    this->msPin = msPin;
  }

  //getters
  uint8_t getPrPin(){
    return prPin;
  }
  uint8_t getMsPin(){
    return msPin;
  }
  uint8_t getMatPin(){
    return M_PIN;
  }

};

#endif