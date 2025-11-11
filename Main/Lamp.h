//Lamp.h
#ifndef LAMP_H
#define LAMP_H

#include "Libraries.h"

#define M_WIDTH 8
#define M_HEIGHT 8
#define NUM_LEDS (M_WIDTH * M_HEIGHT)

template<uint8_t M_PIN>
class Lamp {
  private:
    uint8_t prPin;
    uint8_t msPin;
    microLED<NUM_LEDS, M_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> matrix;
    GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

    uint8_t selector;

  public:
    // простой int позволяет облегчить валидацию 
    //int light_threshold = 150; // аналоговый диапазон (от фоторезистора) от 0 до 1023
    //int matrix_brightness = 56; // яркость от 1 до 100 == от 1 до 56 светящихся диодов
    //float matrix_temp = 3.3; // температура в Кельвинах (количество в тысячах (K))

    struct Settings {
      int light_threshold;
      int matrix_brightness;
      float matrix_temp;
    };

    Settings settings = {
      150,
      56,
      3.3
    };

    const uint8_t SETTINGS_COUNT = 3;


  public:
    Lamp(uint8_t prPin, uint8_t msPin)
    : matrix(M_WIDTH, M_HEIGHT, PARALLEL, LEFT_TOP, DIR_RIGHT)
    {
      setPrPin(prPin);
      setMsPin(msPin);
      selector = 1;
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

    while(true){
      for (int i = 0; i < NUM_LEDS; i++) {

      matrix.set(i, mYellow);
      matrix.show();
      Serial.println(i);
      delay(100);
      }

      for (int i = 0; i < NUM_LEDS; i++) {

      matrix.set(i, mRGB(0, 0, 0));
      matrix.show();
      Serial.println(i);
      delay(100);
      }
    }

  }

  void turnOnMatrix(){
    //вкл матрицу
    //маппинг стоит вынести в отдельный метод
    uint8_t leds_to_turn = map(settings.matrix_brightness, 1, 100, 1, NUM_LEDS);

    for (int i = 0; i < leds_to_turn; i++) {

      matrix.set(converter(i), mKelvin(settings.matrix_temp * 1000));
      matrix.show();
      //Serial.println(i);
      delay(50);
    }
  }

  void turnOffMatrix(){
    //выкл матрицу
    uint8_t leds_to_turn = map(settings.matrix_brightness, 1, 100, 1, NUM_LEDS);

    for (int i = leds_to_turn; i >= 0; i--) {

      matrix.set(converter(i), mRGB(0, 0, 0));
      matrix.show();
      //Serial.println(i);
      delay(50);
    }
  }

  // -----------------------------------------------------------------------

  uint8_t converter(uint8_t val) {

  uint8_t map[] = {
    63, 62, 61, 60, 59, 58, 57, 56, 36, 35, 34, 33, 32, 31, 30, 55, 37, 16, 15, 14, 13, 12, 29, 54, 38, 17, 4, 3, 2, 11, 28, 53, 39, 18, 5, 0, 1, 10, 27, 52, 40, 19, 6, 7, 8, 9, 26, 51, 41, 20, 21, 22, 23, 24, 25, 50, 42, 43, 44, 45, 46, 47, 48, 49
  };
  //0xFF

  uint8_t array_size = sizeof(map) / sizeof(map[0]);

  //поиск индекса
  for (int i = 0; i < array_size; i++) {
    if (map[i] == val) {
      return i;
    }
  }

  return 0xFF;
  }

  // -----------------------------------------------------------------------

  void updateOled(){
    oled.clear();
    oled.autoPrintln(true);
    oled.setScale(1);
    //oled.invertText(1);
    //oled.flipH(1);
    //oled.invertDisplay(1);

    uint8_t row = 16;
    uint8_t col = 16;

    // oled.fastLineH((row) - row / 2, 0, 127, 1);
    // oled.fastLineH((row * 2) - row / 2, 0, 127, 1);
    // oled.fastLineH((row * 3) - row / 2, 0, 127, 1);
    
    //oled.home();
    oled.setCursorXY(16, row * 0);
    oled.print("threshold: ");
    oled.setCursorXY(127 - 32, row * 0);
    oled.print(settings.light_threshold);

    oled.setCursorXY(16, row * 1);
    oled.print("brightness: ");
    oled.setCursorXY(127 - 32, row * 1);
    oled.print(settings.matrix_brightness);

    oled.setCursorXY(16, row * 2);
    oled.print("temperature: ");
    oled.setCursorXY(127 - 32, row * 2);
    oled.print(settings.matrix_temp);

    oled.setCursorXY(4, selector * row);
    oled.print("*");

    oled.update();
  }

  // -----------------------------------------------------------------------

  void initComps(){
    //init oled
    oled.init();
    oled.clear();
    oled.update();
  }

  // -----------------------------------------------------------------------

  void incrementSelector(){
    selector++;

    if (selector > SETTINGS_COUNT) {
      selector = 1;
    }

    Serial.println("selector is: " + String(selector));
  }

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