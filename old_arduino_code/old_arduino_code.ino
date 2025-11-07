// лампа на основе ИК-сенсора, фоторезистора и сломаной матрицы CJMCU-8*8
// также с энкодером и дисплеем TM1637 для настройки

// настройка дисплея TM1637
#define DISP1637_CLK_DELAY 10   // задержка интерфейса в мкс

#include <Arduino.h>
#include <GyverSegment.h>

#define DIO_PIN 12 // цифровые пины подключения дисплея
#define CLK_PIN 11
#define LAT_PIN

// объявление дисплеев
//Disp595_4 disp(DIO_PIN, CLK_PIN, LAT_PIN);
// Disp595_8 disp(DIO_PIN, CLK_PIN, LAT_PIN);
// Disp595_8v2 disp(DIO_PIN, CLK_PIN, LAT_PIN);
// Disp595Static<4> disp(DIO_PIN, CLK_PIN, LAT_PIN);
// Disp1637_4 disp(DIO_PIN, CLK_PIN);
// Disp1637_6 disp(DIO_PIN, CLK_PIN);
Disp1637Colon disp(DIO_PIN, CLK_PIN);
// Disp7219<1> disp(DIO_PIN, CLK_PIN, LAT_PIN);  // 1 чип - 8 цифр 

uint8_t digs[] = {2, 3, 4, 5, A2, A3, A4, A5};  // пины цифр
uint8_t segs[] = {6, 7, 8, 9, 10, 11, 12, 13};  // пины сегментов
// 8 цифр, дсятичные точки есть, общий катод
// DispBare<8, true, false> disp(digs, segs);

// настройка CJMCU-8*8 матрицы
//количество исправных диодов в матрице = 62, смещение на -1 после 6-го
#define NUM_LEDS 64   // Количество светодиодов (8x8)
#define M_PIN 3       // Пин для данных (подключите к DIN или DOUT)
#define M_WIDTH 8     // Ширина матрицы
#define M_HEIGHT 8    // Высота матрицы

#include <microLED.h>

// объявление матрицы
microLED<NUM_LEDS, M_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> matrix(M_WIDTH, M_HEIGHT, PARALLEL, LEFT_TOP, DIR_RIGHT);

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



// ----------------------------- объявление пинов других компонентов  -----------------------------



#define ir_sensor 2     // пин для ИК-сенсора
#define ph_resistor A2     // пин для фотодиода

// LED индикаторы
#define power_mode_indicator 5     // LED индикатор режима работы
#define ir_sensor_indicator 6     // пин для LED индикатора состояния ИК-сенсора
#define led_light_indicator 7     // пин для подачи high на LED лампочку

// ----------------------------- объявление переменных -----------------------------

unsigned long last_ir_signal_time; // Время последнего сигнала от ИК-сенсора
const unsigned long ir_sensor_timeout = 15000; // 15 секунд (в миллисекундах)

// простой int позволяет облегчить валидацию 
int light_threshold = 150; // аналоговый диапазон (от фоторезистора) от 0 до 1023
int matrix_brightness = 58; // яркость от 1 до 100 == от 1 до 56 светящихся диодов
float matrix_temp = 3.3; // температура в Кельвинах (количество в тысячах (K))

uint16_t light_threshold_range[2] = {1, 250}; // массив диапазон для уровня освещения
uint8_t matrix_brightness_range[2] = {1, 100}; // массив диапазон для яркости матрицы
float matrix_temp_range[2] = {0.0, 12.0001}; // массив диапазон для температуры света матрицы + погрешность

uint8_t settings_mode_timeout = 10; // количество секунд timeout-а для режима настройки лампы (будет умноженно на 1000)

uint8_t fast_turn_steps = 10; // количество единиц прокрутки при быстрой прокрутке

// ----------------------------- SETUP() -----------------------------

void setup() {
  //Serial.begin(9600);
  Serial.begin(115200);

  //настройки оставшихся пинов
  pinMode(ir_sensor, INPUT);
  pinMode(ph_resistor, INPUT);

  pinMode(power_mode_indicator, OUTPUT);
  pinMode(led_light_indicator, OUTPUT);
  pinMode(ir_sensor_indicator, OUTPUT);

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

  matrix.setBrightness(255);  // яркость (0-255)

  // очистка матрицы
  matrix.begin();
  matrix.clear();
  matrix.show();
  
  // очистить дисплей
  disp.clear();
  disp.update();

  // запустить режим настройки
  settings_mode();

  //запустить постоянный красный индикатор
  digitalWrite(power_mode_indicator, HIGH);
}



// ----------------------------- LOOP() -----------------------------



void loop() {
  //тест ик-сенсора
  //print_ir_sensor_data();

  //тест фотодиода
  // Serial.print("FINAL light_threshold: ");
  // Serial.print(light_threshold);
  // Serial.print(" --- ");
  // print_photoresistor_data();
  
  main_func();
}



void settings_mode() {
  // ----------------------------- settings_mode() - МЕТОД ДЛЯ НАСТРОЙКИ ЛАМПЫ ПЕРЕД ВКЛЮЧЕНИЕМ -----------------------------
  Serial.println();
  Serial.println("settings_mode() - start!");

  //автонастройка фоторезистора - решил не использовать
  //light_threshold = get_photoresistor_data() + 10;

  uint8_t setting_selector = 0;   //deff set to NO
  uint8_t settings_amound = 3;    //NO, SENS, LGHT, TEMP

  unsigned long time_func_called = millis(); // время, когда функция settings_mode() была запущена
  unsigned long last_blink_time = time_func_called; // время последнего переключения LED индикатора

  unsigned long last_encoder_signal = 0; // Время последнего изменения энкодера

  digitalWrite(power_mode_indicator, HIGH); //вкл LED (power_mode_indicator)
  bool led_state = true; // Текущее состояние светодиода (вкл)

  while (millis() - time_func_called < (settings_mode_timeout * 1000)) { // Работать указанное количество секунд

    //обработка LED диода
    if (millis() - last_blink_time >= 500) { // мигание 
      led_state = !led_state; // переключение
      digitalWrite(power_mode_indicator, led_state ? HIGH : LOW);
      last_blink_time = millis();
    }

    if (millis() - last_encoder_signal > 3000 && setting_selector != 0) { // если энкодер не крутить, через 3 сек он покажет выбранную настройку
      switch (setting_selector) {
          case 1:
            call_tm1637_string("SENS", false);
            break;

          case 2:
            call_tm1637_string("LGHT", false);
            break;

            case 3:
            call_tm1637_string("TEMP", false);
            break;

          default:
            // statements
            break;
        }
    }

    //опрос энкодера
    if (eb.tick()) {

      last_encoder_signal = millis(); // обновить время если пришел любой сигнал
      time_func_called = millis();
      Serial.println("Обновить timeout для time_func_called и last_encoder_signal!");

      // если клик
      if (eb.click()) {
        Serial.println("click!");
        
        // листаем список настроек если click, но кроме изначальной настройки (NO)
        if (setting_selector == settings_amound) {
          setting_selector = 1;
        } else {
          setting_selector++;
        }

        Serial.print("setting_selector: ");
        Serial.println(setting_selector);

        // первое повторений!!!!!
        switch (setting_selector) {
          case 1:
            call_tm1637_string("SENS", false);
            break;

          case 2:
            call_tm1637_string("LGHT", false);
            break;

            case 3:
            call_tm1637_string("TEMP", false);
            break;

          default:
            break;
        }
      }

      if (eb.turn() && setting_selector != 0) {
        switch (setting_selector) {
          case 1:
            int new_light_threshold;

            if (eb.fast()) {
              new_light_threshold = light_threshold + (eb.dir() * fast_turn_steps);
            } else{ // прокрутка без fast
              new_light_threshold = light_threshold + eb.dir();      
            }

            // валидация
            if (light_threshold_range[0] <= new_light_threshold && new_light_threshold <= light_threshold_range[1]) {
              light_threshold = new_light_threshold;
            }
            
            Serial.print("light_threshold: ");
            Serial.println(light_threshold);

            call_tm1637_int(light_threshold);
            break;
            
          case 2:
            int new_matrix_brightness;

            if (eb.fast()) {
              new_matrix_brightness = matrix_brightness + (eb.dir() * fast_turn_steps);
            } else{ // прокрутка без fast
              new_matrix_brightness = matrix_brightness + eb.dir();      
            }

            // валидация
            if (matrix_brightness_range[0] <= new_matrix_brightness && new_matrix_brightness <= matrix_brightness_range[1]) {
              matrix_brightness = new_matrix_brightness;
            }
            
            Serial.print("matrix_brightness: ");
            Serial.println(matrix_brightness);

            call_tm1637_int(matrix_brightness);
            break;

          case 3:
            float new_matrix_temp;

            if (eb.fast()) {
              new_matrix_temp = matrix_temp + eb.dir();
            } else{ // прокрутка без fast
              new_matrix_temp = matrix_temp + (eb.dir() / 10.0);
            }

            // валидация
            if (matrix_temp_range[0] <= new_matrix_temp && new_matrix_temp <= matrix_temp_range[1]) {
              matrix_temp = new_matrix_temp;
            }
            
            Serial.print("matrix_temp: ");
            Serial.println(matrix_temp, 9);

            call_tm1637_string(String(matrix_temp, 1) + "K", true);
            break;
            
          default:
            // statements
            break;
        }
      }

      if (eb.step() && setting_selector == 1) {

      print_photoresistor_data();

      call_tm1637_int(get_photoresistor_data());
      }

      if (eb.releaseStep() && setting_selector == 1) {
        call_tm1637_int(light_threshold);
      }

      //eb.clear();
    }
  }

  digitalWrite(power_mode_indicator, LOW); //выкл LED (power_mode_indicator) при выключении функции

  disp.clear();
  disp.update();
}

void call_tm1637_int (int val) {
  disp.clear();
  disp.printRight(true);
  disp.setCursorEnd();
  disp.print(val);
  disp.update();
  //disp.delay(100); // не уверен
}

void call_tm1637_string (String string, bool dots) {
  disp.clear();
  
  disp.printRight(true);
  disp.setCursorEnd();
  disp.print(string);
  disp.colon(dots);
  disp.update();
  //disp.delay(100); // не уверен
}

void print_photoresistor_data () {
  Serial.print("ФОТОРЕЗИСТОР (RAW): ");
  Serial.println(get_photoresistor_data());
}

// unsigned int get_photoresistor_data () {
//   return analogRead(ph_resistor);
// }

unsigned int get_photoresistor_data () {
  uint16_t sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(ph_resistor);
  }
  return sum / 10;
}

void print_ir_sensor_data () {
  Serial.print("ИК-СЕНСОР (RAW): ");
  Serial.println(get_ir_sensor_data());
}

uint8_t get_ir_sensor_data () {
  return digitalRead(ir_sensor);
}


void main_func() {
  // ----------------------------- main_func() - ГЛАВНЫЙ МЕТОД ДЛЯ ВКЛЮЧЕНИЯ МАТРИЦЫ (сам свет) -----------------------------

  // взять значение с фоторезистора и запустить ИК сенсор если оно меньше нужного
  int light_level = get_photoresistor_data();

  if (light_level < light_threshold) {
    // взять статус ИК сенсора и включить LED если оно == 1

    digitalWrite(ir_sensor_indicator, HIGH); //вкл ИК индикатор
    Serial.println("ИК ЗАПУЩЕН!");
    
    if (get_ir_sensor_data() == 1) {
      // включить LED и отключить если от ИК не поступал сигнал в течение отведенного времени
      digitalWrite(led_light_indicator, HIGH); //вкл индикатор света
      turn_on_matrix();
      last_ir_signal_time = millis();

      Serial.println("ИК - ВЫСОКИЙ СИГНАЛ! - ВКЛ СВЕТ!");

      while ((millis() - last_ir_signal_time) <= ir_sensor_timeout) {
        //обновляем таймер при получении сигнала от ИК
        if (get_ir_sensor_data() == 1) {
          last_ir_signal_time = millis();
          Serial.println("ОБНОВИТЬ ТАЙМЕР!");
          
        } else {
          Serial.println("ПУСТОЙ ЦИКЛ! - " + String(ir_sensor_timeout - (millis() - last_ir_signal_time)));
        }
      }

      // тайм аут - выключение всета
      digitalWrite(led_light_indicator, LOW); //выкл индикатор света
      turn_off_matrix();
      Serial.println("ТАЙМ АУТ!");

    }
  } else {
    digitalWrite(ir_sensor_indicator, LOW); //выкл ИК индикатор
  }
}

void turn_on_matrix () {
  //включение матрицы
  uint8_t leds_to_turn = map(matrix_brightness, 1, 100, 1, 63);

  for (int i = 0; i < leds_to_turn; i++) {
      int n = converter(i);
      n--;

      matrix.set(n, mKelvin(matrix_temp * 1000));
      matrix.show();
      Serial.println(n);
      delay(50);
  }
}

void turn_off_matrix () {
  //выключение матрицы
  uint8_t leds_to_turn = map(matrix_brightness, 1, 100, 1, 63);

  
  for (int i = leds_to_turn; i >= 0; i--) {
    int n = converter(i);
    n--;

    matrix.set(n, mRGB(0, 0, 0)); // Зажигаем светодиод
    matrix.show();
    Serial.println(n);
    delay(50);
  }
}







