//Main.ino
#include "Libraries.h"
#include "Lamp.h"

#define PR_PIN A3
#define MS_PIN 4
#define M_PIN 2

Lamp<M_PIN> lamp(PR_PIN, MS_PIN); 


// ------------ TEST ----------------------



#define BTN_PIN 7				// кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)

#include "GyverButton.h"
GButton butt1(BTN_PIN);

// ----------------------------------------

void setup() {
  Serial.begin(9600);

  lamp.initComps();


// ----------------- TEST

  
  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)

  // HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC (BTN_PIN --- КНОПКА --- GND)
  // LOW_PULL  - кнопка подключена к VCC, пин подтянут к GND
  // по умолчанию стоит HIGH_PULL
  butt1.setType(HIGH_PULL);

  // NORM_OPEN - нормально-разомкнутая кнопка
  // NORM_CLOSE - нормально-замкнутая кнопка
  // по умолчанию стоит NORM_OPEN
  butt1.setDirection(NORM_OPEN);
}

void loop() {
  // lamp.printPrData(lamp.getPrData());

  // delay(100);

  // lamp.printMsData(lamp.getMsData());

  // delay(100);

  // lamp.testMatrix();

  // lamp.turnOnMatrix();

  // delay(10000);

  // lamp.turnOffMatrix();



  butt1.tick();

  if (butt1.isPress()) {
    lamp.incrementSelector();

    lamp.updateOled();
  }


}
