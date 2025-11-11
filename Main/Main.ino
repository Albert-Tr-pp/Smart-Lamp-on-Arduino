// таски:
о добавить inperatp для энк;
о оптимизировать обновление дисплея;


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

// ------------ ENC ----------------------

// настройка энкодера
#include <Arduino.h>
#define EB_NO_FOR           // отключить поддержку pressFor/holdFor/stepFor и счётчик степов (экономит 2 байта оперативки)
#define EB_NO_CALLBACK      // отключить обработчик событий attach (экономит 2 байта оперативки)
#define EB_NO_COUNTER       // отключить счётчик энкодера (экономит 4 байта оперативки)
#define EB_NO_BUFFER        // отключить буферизацию энкодера (экономит 1 байт оперативки)

#define EB_DEB_TIME 50      // таймаут гашения дребезга кнопки (кнопка)
#define EB_CLICK_TIME 500   // таймаут ожидания кликов (кнопка)
#define EB_HOLD_TIME 600    // таймаут удержания (кнопка)
#define EB_STEP_TIME 200    // таймаут импульсного удержания (кнопка)
#define EB_FAST_TIME 30     // таймаут быстрого поворота (энкодер)
#define EB_TOUT_TIME 1000   // таймаут действия (кнопка и энкодер)

#include <EncButton.h>

EncButton eb(9, 8, 10, INPUT); // + режим пинов энкодера



// ---------------- TIMMERS and FLAGS------------------------

unsigned long last_encoder_signal = 0; // Время последнего изменения энкодера
bool encoder_flag = 0;




void setup() {
  Serial.begin(9600);

  lamp.initComps();


  // ------------------------ ENC -------------------------

  //настройка энкодера
  eb.setBtnLevel(LOW);
  eb.setClickTimeout(10);
  eb.setDebTimeout(100);
  eb.setHoldTimeout(600);
  eb.setStepTimeout(500);
  eb.setTimeout(1000);

  eb.setEncReverse(0);
  eb.setEncType(EB_STEP4_LOW);
  eb.setFastTimeout(50);

  // сбросить счётчик энкодера
  //eb.counter = 0; // выключен


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


  //lamp.incrementThreshold(-1551);
}

void loop() {
  //lamp.printPrData(lamp.getPrData());

  // delay(100);

  // lamp.printMsData(lamp.getMsData());

  // delay(100);

  // lamp.testMatrix();

  // lamp.turnOnMatrix();

  // delay(10000);

  // lamp.turnOffMatrix();



  // butt1.tick();

  // if (butt1.isPress()) {
  //   lamp.incrementSelector();

  //   lamp.updateOled();
  // }



  if (millis() - last_encoder_signal > 5000 && encoder_flag) {
    lamp.updateOled();
    lamp.displaySettings(0);
    encoder_flag = 0;
  }




  if (eb.tick()) {

    last_encoder_signal = millis();
    encoder_flag = 1;

    if (eb.click()) {
      lamp.incrementSelector();
      lamp.updateOled();
      lamp.displaySettings(0);
    }

    if (eb.turn()) {
      if (eb.fast()) {
        lamp.incrementSettingsValues(lamp.getSelector(), eb.dir() * 10);
      } else {
        lamp.incrementSettingsValues(lamp.getSelector(), eb.dir());
      }
      lamp.displaySettings(1);
    }

    if (eb.step() && lamp.getSelector() == 0) {
      lamp.displaySettings(2);
    } else if (eb.releaseStep() && lamp.getSelector() == 0) {
      lamp.updateOled();
      lamp.displaySettings(1);
    }




  }


}