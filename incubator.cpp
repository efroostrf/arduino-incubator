#include <SPI.h>
#include <Servo.h> //Подключаем библиотеку для управление сервоприводом.
#include <EEPROM.h> //Библиотека для хранения переменных.
// #include <Adafruit_Si7021.h> //Библиотека для получения данных с датчика температуры и влажности.
#include <SI7021.h>
#include <avr/wdt.h> //Wathdog
// #include <SD.h> 
// File root;


// #define SerialWifiESP
#define AdafruitDisplay
// #define LightWeightDisplay
// #define NeopixelLed
#define LightLed
#define GyverEncoderOn
// #define WifiStuff
#define RTCLibrary

#ifdef SerialWifiESP
  #include "SoftwareSerial.h"
#endif

#ifdef LightLed
  #include <WS2812.h>

  WS2812 LED(1); // 1 LED	
  cRGB colorLed;
#endif

#ifdef RTCLibrary
  #include "RTClib.h"
#endif

#ifdef AdafruitDisplay
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
#endif

#ifdef LightWeightDisplay
  #include <Cytron_SSD1306.h>
#endif

#ifdef NeopixelLed
  #include <Adafruit_NeoPixel.h>
#endif

#ifdef GyverEncoderOn
  #include <GyverEncoder.h>
#endif

#ifdef WifiStuff
  #include <WiFi.h>
  // const char server[] PROGMEM = "www.google.com"; 

  // WiFiClient client;
  #include <BlynkSimpleWifi.h>
  // #include <ESP8266WiFi.h>
  char auth[] = "DJuOcFgkYFQHOfwuIEsbaDnfX4n1QjdG";
  const char ssid[] PROGMEM = "Protected0x445";
  const char pass[] PROGMEM = "^DEzQ2Mpnq";
#endif

//EEPROM
//0 - Адрес для времени начала инкубации.
//1 - Статус инкубации (0/1).
//5 - Кг для серво.
unsigned long IncubateTime = EEPROM.read(1);
byte IncubateStatus = EEPROM.read(0);
float KGServo = EEPROM.read(5);


//Цифровые пины.
const byte P_LED = 8; //Светодиод.
const byte P_Servo  PROGMEM= 9; //Сервомашинка.
const byte P_EncoderCLK = 10; //CLK Энкодера.
const byte P_EncoderDT = 11; //DT Энкодера.
const byte P_EncoderSW = 12; //SW Энкодера.
const byte P_ReleyHeater PROGMEM = 5; //Реле нагревательного элемента (лампы).
const byte P_ReleyHumidity PROGMEM = 4; //Реле для контроля влажности.
const byte P_ReleyVenting PROGMEM = 3; //Реле для контроля проветривания.
const byte P_ReleySiren PROGMEM = 2; //Реле для звуковой сирены.

//Аналоговые пины.
const byte P_Moisture = A0; //Датчик заполнености ёмкости водой.
const byte P_Light = A1; //Датчик света.


//Цвета светодиода.
byte RedL = 0;
byte GreenL = 0;
byte BlueL = 0;

//Системные переменные.
const byte displayTimeout PROGMEM = 60;
byte secondsDisplay = displayTimeout;
byte mode;
byte menu = 0; //Включённое меню.
byte cursorPosition = 0;
byte Warning = 0; //Мигающий жёлтый индикатор.
byte WarningState = 0; //Положение индикатора.
byte Error = 0; //Мигающий красный индикатор.
byte ErrorState = 0; //Положение красного индикатора.
byte Siren = 0; //Включена сирена или нет.
byte systemEvent = 0; //События в "системе".
//0 - Нормальная работа.
//1

//Переменные для модулей.
byte SirenState = 0;
byte HeaterState = 0;
byte HumidityState = 0;
byte VentingState = 0;

//mode
//0 - Установлена пауза, инкубатор не работает (режим для тестирования).
//1 - Автоматический режим.

byte autoEnable; //Состояние включённое инкубации.

//Переменные ручного инкубирования
byte manualEnable = 0;
float ManualTemp = 37.5;
byte ManualHum = 60;
byte ManualRotate = 0;
byte ManualVenting = 0;

//Всплывающее окно
byte Popup = 0;

int TimeoutSensors = 1000;
int TimeoutOperations = 1000;
unsigned long now_time;
unsigned long alertBlink = millis() + 1000;
unsigned long timeoutSensors = millis() + TimeoutSensors;
unsigned long systemOperations = millis() + TimeoutOperations;
unsigned long stopVenting = 0;
unsigned long resetVenting = 0;
unsigned long resetRotate =0;
unsigned long closePopup = 0;
unsigned long TimeSecond = 0;

float Temperature = 0;
byte Humidity = 0;
int Moisture = 0;
int Light = 0;
byte MoisturePercent = 0;
float dayInc = 0;

byte Blink = 0;
byte Blink2 = 0;

byte WaterError = 0;
byte HeaterError = 0;
byte HeaterBigError = 0;
byte HumidityError = 0;

struct INCUBATOR_PROGRAM {
  float temperature; //Температура
  byte humidity; //Влажность
  int ventingCount; //Количество вентиляций
  int ventingTime; //Длительность каждой вентиляции
  int rotateCount; //Количество переворотов
};
  
// const INCUBATOR_PROGRAM chickens[] PROGMEM = {
//   //Темп, Влж, Вент, Вент. Дл., Пов.
//   {37.8, 60, 0, 0, 6}, //1
//   {37.8, 60, 0, 0, 6}, //2
//   {37.8, 60, 0, 0, 6}, //3
//   {37.8, 60, 0, 0, 6}, //4
//   {37.8, 60, 0, 0, 6}, //5
//   {37.8, 55, 0, 0, 6}, //6
//   {37.8, 55, 0, 0, 6}, //7
//   {37.8, 55, 0, 0, 6}, //8
//   {37.8, 55, 0, 0, 6}, //9
//   {37.8, 50, 0, 0, 6}, //10
//   {37.8, 50, 0, 0, 6}, //11
//   {37.8, 50, 0, 0, 6}, //12
//   {37.8, 50, 0, 0, 6}, //13
//   {37.8, 50, 0, 0, 6}, //14
//   {37.8, 45, 2, 900, 6}, //15
//   {37.8, 45, 2, 900, 6}, //16
//   {37.8, 45, 2, 900, 6}, //17
//   {37.8, 45, 2, 900, 6}, //18
//   {37.5, 75, 2, 600, 0}, //19
//   {37.5, 75, 2, 600, 0}, //20
//   {37.5, 75, 2, 600, 0} //21
// };

// const INCUBATOR_PROGRAM chickens[] PROGMEM = {
//   //Темп, Влж, Вент, Вент. Дл., Пов.
//   {38.0, 70, 0, 0, 6}, //1 0
//   {38.0, 70, 0, 0, 6}, //2 1
//   {38.0, 70, 0, 0, 6}, //3 2
//   {38.0, 70, 0, 0, 6}, //4 3
//   {38.0, 70, 0, 0, 6}, //5 4
//   {38.0, 70, 0, 0, 6}, //6 5
//   {38.0, 70, 0, 0, 6}, //7 6 
//   {37.8, 60, 0, 0, 6}, //8 7
//   {37.8, 60, 0, 0, 6}, //9 8 
//   {37.8, 60, 0, 0, 6}, //10 9
//   {37.8, 60, 0, 0, 6}, //11 10
//   {37.8, 60, 0, 0, 6}, //12 11 
//   {37.8, 60, 0, 0, 6}, //13 12
//   {37.8, 60, 0, 0, 6}, //14 13
//   {37.8, 60, 2, 900, 6}, //15 14
//   {37.8, 60, 2, 900, 6}, //16 15
//   {37.8, 60, 2, 900, 6}, //17 16
//   {37.8, 60, 2, 900, 6}, //18 17 
//   {37.8, 60, 2, 900, 6}, //19 18
//   {37.8, 60, 2, 900, 6}, //20 19
//   {37.8, 60, 2, 900, 6}, //21 20 
//   {37.8, 60, 2, 900, 6}, //22 21
//   {37.8, 60, 2, 900, 6}, //23 22
//   {37.8, 60, 2, 900, 6}, //24 23
//   {37.8, 60, 2, 900, 6}, //25 24
//   {37.5, 90, 0, 0, 0} //26 25

// };

// const char popupTexts[] PROGMEM = {"Поставьте инкубацию на паузу!"};


//Инициализация модулей.
// Adafruit_Si7021 sensor = Adafruit_Si7021();
SI7021 sensor;
Servo downServo;

#ifdef RTCLibrary
  RTC_DS1307 rtc;
#endif

#ifdef GyverEncoderOn
  Encoder enc(P_EncoderCLK, P_EncoderDT, P_EncoderSW);
#endif

#ifdef NeopixelLed
  Adafruit_NeoPixel ledScreen (1, P_LED, NEO_GRB + NEO_KHZ800);
#endif

#ifdef AdafruitDisplay
  Adafruit_SSD1306 display(128, 64, &Wire, -1);
#endif

#ifdef LightWeightDisplay
  Cytron_SSD1306 display;
#endif


/* Функция перекодировки русских букв из UTF-8 в Win-1251 */
String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };
  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

// void showPopup(byte codePopup, unsigned long Time) {
//   Popup = codePopup;
//   closePopup = millis() + (Time*1000);
// }

void updateIncubateInfo() {
  // IncubateTime = EEPROM.read(0);
  // IncubateStatus = EEPROM.read(1);
  EEPROM.get(0, IncubateStatus);
  EEPROM.get(1, IncubateTime);
  EEPROM.get(5, KGServo);
}

unsigned long timeDay() {
  #ifdef RTCLibrary
    DateTime nowtime = rtc.now();
    unsigned long hoursSec = (unsigned long)nowtime.hour() * (unsigned long)3600;
    unsigned long minuteSec = (int)nowtime.minute() * 60;
    unsigned long Sec = (int)nowtime.second();
    unsigned long total = hoursSec+minuteSec+Sec;
    return total;
  #endif 
} 

float dayIncubation() {
  unsigned long unixnow;
  #ifdef RTCLibrary
    DateTime now = rtc.now();
    unixnow = now.unixtime();
  #endif
  unsigned long res = (unsigned long)unixnow - (unsigned long)IncubateTime;
  float days = (float)res / (float)86400;
  return days;
}

unsigned long timeDayEEPROM() {
  //Данная функция создана для точного рассчёта начала инкубации.
  //Она получает значение дня начала инкубации из EEPROM в UNIX-виде.
  //После сравнивает значение с текущим временем и получает float значение.
  //Данное float значение в диапазоне от 0.0 до 1.0.
  //После секунды дня (86400) умножаются на float и мы получаем время.
  float days = dayIncubation(); //Получаем дни инкубации.
  for (int i = 0; days > 1.0; i++) {
    days = days - 1.0; //С помощью цикла выполняем 4 пункт.
  }
  
  unsigned long result = 86400 * days;
  // Serial.print(F("RES: "));
  // Serial.println(result);
  // Serial.print(F("days: "));
  // Serial.println(days);
  return result;
}

void RotateServo(byte dir) {
  byte RotateSpeed = 17;
  int p = 400; //Мс обороты.

  float massKg = KGServo; //Масса яиц (КГ).
  float servoForce = 9.4; //Пусковой момент серво (КГ).
  float servoDegressAtMs = p/RotateSpeed; //За сколько мс серво делает передвижение на 1 градус.

  float deltaDegress = massKg/servoForce; //Насколько изменяться градусы за секунду.
  servoDegressAtMs = servoDegressAtMs+(servoDegressAtMs*deltaDegress); //Рассчитываем количество градусов за мс в зависимости от массы.
  
  if (dir == 0) {
    downServo.write(90+RotateSpeed);
    delay(servoDegressAtMs*80);
    downServo.write(90);
  } 
  else if (dir == 1) {
    downServo.write(90-RotateSpeed);
    delay(servoDegressAtMs*80);
    downServo.write(90);
  }

}


byte rotateOn = 0; //Триггер для вращения яиц.
byte ventingOn = 0;


void ledChangeColor(byte Red, byte Green, byte Blue) {
  #ifdef NeopixelLed
    ledScreen.setPixelColor(0, ledScreen.Color(Red, Green, Blue));
    ledScreen.show();
  #endif

  #ifdef LightLed
    colorLed.b = Red; colorLed.g = Green; colorLed.r = Blue;
	  LED.set_crgb_at(0, colorLed);
    LED.sync();
  #endif
}

void oneBlink(byte Red, byte Green, byte Blue) {
  ledChangeColor(Red, Green, Blue);
  delay(300);
  ledChangeColor(0, 0, 0);
}

void offRelays() {
  //Для реле задаём значение, чтобы не были включены.
  digitalWrite((byte)pgm_read_byte(&P_ReleySiren), HIGH);
  digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), HIGH);
  digitalWrite((byte)pgm_read_byte(&P_ReleyHumidity), HIGH);
  digitalWrite((byte)pgm_read_byte(&P_ReleyHeater), LOW);
}

void startupSequance() {
  #ifdef NeopixelLed
    ledScreen.begin(); //Подключаем светодиод.
    ledScreen.setBrightness(50); //Яркость светодиода.
  #endif

  #ifdef LightLed
    LED.setOutput(P_LED);
  #endif

  ledChangeColor(0, 124, 0);

  //Инициализируем все пины.
  pinMode(13, OUTPUT);
  // pinMode(3, OUTPUT);
  pinMode((byte)pgm_read_byte(&P_ReleySiren), OUTPUT);
  pinMode((byte)pgm_read_byte(&P_ReleyVenting), OUTPUT);
  pinMode((byte)pgm_read_byte(&P_ReleyHumidity), OUTPUT);
  pinMode((byte)pgm_read_byte(&P_ReleyHeater), OUTPUT);
  pinMode(P_Moisture, INPUT); //Датчик воды.
  pinMode(P_Light, INPUT); //Датчик света.
  
  offRelays();

  delay(100);
  BlueL = 56;
  ledChangeColor(124, 124, BlueL);
  Serial.begin(9600); //Соединяемся с serial портом компьютера.
  sensor.begin(); //Подключаем наш датчик температуры/влаги.
  
  
  downServo.attach((byte)pgm_read_byte(&P_Servo));

  #ifdef GyverEncoderOn
    enc.setType(TYPE1); //Устанавливаем тип энкодера.
  #endif

  #ifdef RTCLibrary
    rtc.begin();
    //rtc.adjust(DateTime(2021, 3, 4, 19, 37, 0));
  #endif

  #ifdef AdafruitDisplay
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Инициализируем дисплей.
    //Ниже настройки для дисплея.
    display.setTextColor(WHITE); //Цвет текста.
    display.cp437(true);
    display.clearDisplay();
  #endif

  #ifdef LightWeightDisplay
    display.begin();
  #endif
  
  delay(100);
  ledChangeColor(0, 0, BlueL);

  // Serial.println(F("Temperature Humidity Water RelayHeater RelayHumidity RelaySiren RelayVenting"));
}


int dayIncubationInt() {
  unsigned long unixnow;
  #ifdef RTCLibrary
    DateTime now = rtc.now();
    unixnow = now.unixtime();
  #endif

  unsigned long res = (unsigned long)unixnow - (unsigned long)IncubateTime;
  unsigned long days = (unsigned long)res / (unsigned long)86400;
  // Serial.println(unixnow);
  // Serial.println(res);
  // Serial.println(days);
  return (int)days;
}

int percentWater() {
  int empty = 570;
  int full = 372;
  
  empty = empty - full;
  float x = ((float)Moisture - (float)full) / (float)empty;
  x = (1.0 - x) * 100;
  if (x < 0.0) {
    x = 0.0;
  }
  return (int)x;
}

void readSensors() {
  Temperature = sensor.getCelsiusHundredths() / 100.0;
  Humidity = sensor.getHumidityBasisPoints() / 100.0;
  Moisture = analogRead(P_Moisture);
  MoisturePercent = percentWater();
  Light = analogRead(P_Light);
  dayInc = dayIncubation();
}


int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() {
  // Просто напиши код для инициализации тут:
  delay(1000);
  startupSequance();

  updateIncubateInfo();
  if (IncubateStatus == 1) {
    mode = 1;
    autoEnable = 1;
  } else if (IncubateStatus == 2) {
    mode = 1;
    autoEnable = 0;
  } else {
    mode = 0;
  }
  readSensors();
  // Serial.println(F("begin"));

  #ifdef WifiStuff
    // WiFi.begin(pgm_read_word(&ssid), pgm_read_word(&pass));
    Blynk.begin(auth, pgm_read_word(&ssid), pgm_read_word(&pass));
  #endif
  #ifdef SerialWifiESP
    SoftwareSerial esp(6, 7);
    esp.begin(9600);
  #endif

  wdt_enable (WDTO_8S); //Watchdog на мониторинг зависания платы.
}



void testCategory() {
  if (cursorPosition == 0) {
    if (SirenState == 0) {
      digitalWrite((byte)pgm_read_byte(&P_ReleySiren), LOW);
      SirenState = 1;
    } else {
      digitalWrite((byte)pgm_read_byte(&P_ReleySiren), HIGH);
      SirenState = 0;
    }
  }
  else if (cursorPosition == 1) {
    if (HeaterState == 0) {
      digitalWrite((byte)pgm_read_byte(&P_ReleyHeater), HIGH);
      HeaterState = 1;
    } else {
      digitalWrite((byte)pgm_read_byte(&P_ReleyHeater), LOW);
      HeaterState = 0;
    }
  }
  else if (cursorPosition == 2) {
    if (HumidityState == 0) {
      digitalWrite((byte)pgm_read_byte(&P_ReleyHumidity), LOW);
      HumidityState = 1;
    } else {
      digitalWrite((byte)pgm_read_byte(&P_ReleyHumidity), HIGH);
      HumidityState = 0;
    }
  }
  else if (cursorPosition == 3) {
    if (VentingState == 0) {
      digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), LOW);
      VentingState = 1;
    } else {
      digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), HIGH);
      VentingState = 0;
    }
  } 

  delay(100);
}


void encoderHandler() {
  if (secondsDisplay <= 0) {
    if (enc.isTurn() || enc.isPress()) {
      secondsDisplay = displayTimeout;
    }
  } else {
    if (enc.isRight()) {
      //Обычное меню.
      byte maxPunkt = 0;
      if (menu == 0) {
        maxPunkt = 3;
      } else if (menu == 1) {
        if (mode == 1) {
          maxPunkt = 2;
        } else if (mode == 0) {
          maxPunkt = 5;
        }
      } else if (menu == 3) {
        maxPunkt = 4;
      }
      if (cursorPosition < maxPunkt) {
        cursorPosition++;
      }
    } 
    else if (enc.isLeft()) {
      if (menu == 0) {
        if (cursorPosition > 0) {
          cursorPosition--;
        }
      } else {
        if (cursorPosition > 0) {
          cursorPosition--;
        }
      }
      
    }
    if (enc.isFastL()) {
      if (menu == 2 && cursorPosition == 0) {
        KGServo = 0.0;
      }
    }
    if (enc.isSingle()) {
      if (menu == 0) { //Если у нас не выбрано меню - выбираем.
        menu = cursorPosition;
        cursorPosition = 0;
      }
      else if (menu == 1) {
        //Тестирование.
        if (mode == 0) {
          //Прогревка
          if (cursorPosition == 1) {
            if (manualEnable == 0) {
              //Запускаем прогрев.
            } else {
              //Уже включён режим.
              
            }
          }
          //Включение/выключение.
          else if (cursorPosition == 2) {
            if (manualEnable == 0) {
              manualEnable = 1;
            } else {
              manualEnable = 0;
              offRelays();
            }
          }
        }
        else if (mode == 1) {
          if (cursorPosition == 1) {
            if (autoEnable == 1) {
              //Пауза.
              EEPROM.put(0, 2);
              autoEnable = 0;
              WaterError = 0;
              offRelays();
            } else if (autoEnable == 0) {
              if (IncubateTime != 0) {
                //Возобновление.
                autoEnable = 1;
                EEPROM.put(0, 1);
              } else {
                //Запустить.
                autoEnable = 1;
                #ifdef RTCLibrary
                  DateTime now = rtc.now();
                  EEPROM.put(1, now.unixtime());
                  
                #endif
                EEPROM.put(0, 1);
                updateIncubateInfo();
              }
            }

          }
          else if (cursorPosition == 2) {
            if (autoEnable == 0 && IncubateTime != 0) {
                IncubateTime = 0;
                EEPROM.put(1, IncubateTime);
                // EEPROM.put(0, 0);
                updateIncubateInfo();
            }
          }
        }
      }
      else if (menu == 3) {
        //Меню тестирования.
        testCategory();
        // if (mode == 0) {
          
        // }
      }
      
    } else if (enc.isDouble()) {
      if (menu != 0) {
        cursorPosition = menu;
        menu = 0;
      }
    }
    if (enc.isRightH()) {
      if (menu == 1 && cursorPosition == 0) {
        if (mode <= 0) {
          mode++;
          offRelays();
          manualEnable = 0;
        }
      }
      if (mode == 0 && menu == 1) {
        if (cursorPosition == 3) {
          ManualTemp = ManualTemp+0.1;
        } else if (cursorPosition == 4) {
          ManualHum++;
        } else if (cursorPosition == 1) {
          ManualVenting++;
        } else if (cursorPosition == 5) {
          ManualRotate++;
        }
      }
      if (menu == 3 && cursorPosition == 4) {
        RotateServo(0);
      } 
      if (menu == 2 && cursorPosition == 0) {
        KGServo = KGServo + 0.1;
      }
    }
    if (enc.isLeftH()) {
      if (menu == 1 && cursorPosition == 0) {
        if (mode > 0) {
          mode--;
          offRelays();
          manualEnable = 0;
        }
      }
      if (mode == 0 && menu == 1) {
        if (cursorPosition == 3) {
          ManualTemp = ManualTemp-0.1;
        } else if (cursorPosition == 4) {
          ManualHum--;
        } else if (cursorPosition == 1) {
          ManualVenting--;
        } else if (cursorPosition == 5) {
          ManualRotate--;
        }
      }
      if (menu == 3 && cursorPosition == 4) {
        RotateServo(1);
      } else {
        // showPopup(0, 10);
      }

      if (menu == 2 && cursorPosition == 0) {
        if (KGServo > 0.0) {
          KGServo = KGServo - 0.1;
        }
      }
    }
    if (enc.isHolded()) {
      if (menu == 2) {
        EEPROM.put(5, KGServo); //Записываем значение КГ.
      
        delay(100);
        cursorPosition = 0;
        menu = 0;
      }
    }
  }
}



boolean statusLight() {
  if (analogRead(P_Light) < 400) {
    return true;
  } else {
    return false;
  }
}



void defaultDisplay() {
  // #ifdef AdafruitDisplay

  // #endif
  display.setCursor(5, 5);
  #ifdef AdafruitDisplay
    display.setTextSize(2);
  #endif
  display.print(Temperature, 1);
  display.print(F(" C"));
  
  #ifdef AdafruitDisplay
    display.drawCircle(60, 5, 2, WHITE);
    display.drawLine(5, 23, 74, 23, WHITE);
    display.drawLine(5, 24, 74, 24, WHITE);
  #endif

  
  
  display.setCursor(5, 30);
  display.print(Humidity, 1);
  display.print(F("%"));

  display.setCursor(5, 55);
  #ifdef AdafruitDisplay
    display.setTextSize(0);
  #endif
  display.print(utf8rus(F("Вода")));
  display.print(F(": "));
  display.print(MoisturePercent);
  display.print(F("%"));
  display.print(F("  "));

  if (mode == 1) {
    display.print(utf8rus(F("День")));
    display.print(F(": "));
    display.print(dayIncubationInt() + 1);
  } else {
    #ifdef RTCLibrary
      DateTime today = rtc.now();
      display.print(today.hour());
      display.print(F(":"));
      display.print(today.minute());
      display.print(F(":"));
      display.print(today.second());
    #endif
  }
  
  #ifdef AdafruitDisplay
    display.setTextSize(1);
  #endif
  
  display.setCursor(100, 5);
  if (mode == 1) {
    if (autoEnable == 1) {
      byte kday = dayIncubationInt();
      // if (kday >= 26) {
      //   kday = 26;
      // }
      float tmpr = 37.8;
      byte humdt = 70;
      if (kday < 7) {
        tmpr = 38.0;
        humdt = 70;
      } else if (kday < 25) {
        tmpr = 37.8;
        humdt = 60;
      } else {
        tmpr = 37.5;
        humdt = 90;
      }
      display.print(F("Auto"));

      display.setCursor(100, 15);
      display.print((float)tmpr, 1);
      // display.print((float)pgm_read_float(&chickens[kday].temperature), 1);
      display.setCursor(105, 25);
      display.print((byte)humdt);
      // display.print((byte)pgm_read_byte(&chickens[kday].humidity));
      display.print(F("%"));

    } else {
      display.print(F("Stop"));
    }
  } else {
    if (manualEnable == 1) {
      display.print(F("Rezh"));

      display.setCursor(100, 15);
      display.print(ManualTemp, 1);
      display.setCursor(105, 25);
      display.print(ManualHum);
      display.print(F("%"));
    } else {
      display.print(F("Stop"));
    }
  }


  
}

void drawCursorDisplay(byte i) {
  //1 - Инкубация
  //2 - Калибровка
  //3 - Тестирование
  #ifdef AdafruitDisplay
    display.setTextSize(2);
  #endif
  if (i == 1) {
    display.setCursor(5, 32);
    display.println(utf8rus(F("Инкубация")));
  } else if (i == 2) {
    display.setCursor(5, 32);
    display.println(utf8rus(F("Калибровка")));
  } else if (i == 3) {
    display.setCursor(5, 32);
    display.println(utf8rus(F("Тест")));
  }
}

void drawMenuCursor() {
  if (menu != 0) {
    #ifdef AdafruitDisplay
      display.setTextSize(0);
    #endif
    display.setCursor(0, cursorPosition*10);
    display.print(F(">"));
  }
}

void drawMenuDisplay() {
  //1 - Инкубация
  //2 - Калибровка
  //3 - Тестирование
  if (menu == 1) {
    #ifdef AdafruitDisplay
      display.setTextSize(0);
    #endif
    display.setCursor(10, 0);
    display.print(utf8rus(F("Режим")));
    display.print(F(": "));
    //Тут мы проверяем с помощью if режим и в зависимости от него выводим пункты.
    if (mode == 0) {
      display.print(utf8rus(F("ручной")));

      display.setCursor(10, 20);
      // display.print(utf8rus(F("Прогреть")));

      if (manualEnable == 0) {
        display.print(utf8rus(F("Включить")));
      } else {
        display.print(utf8rus(F("Отключить")));
      }

      display.setCursor(10, 30);
      display.print(utf8rus(F("Темп")));
      display.print(F(".: "));
      display.print(ManualTemp, 1);
      display.print(utf8rus(F("C")));
      
      display.setCursor(10, 40);
      display.print(utf8rus(F("Влаж")));
      display.print(F(".: "));
      display.print(ManualHum);
      display.print(utf8rus(F("%")));

      display.setCursor(10, 50);
      display.print(utf8rus(F("К")));
      display.print(F(". "));
      display.print(utf8rus(F("Поворот")));
      display.print(F(": "));
      display.print(ManualRotate);
      
      display.setCursor(10, 10);
      display.print(utf8rus(F("К")));
      display.print(F(". "));
      display.print(utf8rus(F("Вент")));
      display.print(F(".: "));
      display.print(ManualVenting);

    } else if (mode == 1) {
      display.print(utf8rus(F("авто")));

      display.setCursor(10, 10);
      if (autoEnable == 0) {
        if (IncubateTime != 0) {
          display.print(utf8rus(F("Возобновить")));
          
          display.setCursor(10, 20);
          display.print(utf8rus(F("Остановить")));
        } else {
          display.print(utf8rus(F("Запустить")));
        }
      } else if (autoEnable == 1) {
        display.print(utf8rus(F("Пауза")));
      }
      
      

      // display.setCursor(10, 20);
      // display.print(IncubateTime);
      // display.print(F(" "));
      // display.print(IncubateStatus);

      // display.setCursor(10, 30);
      // display.print(IncubateStatus);

      // display.print(utf8rus(F("Закончить")));
    }
  } 
  else if (menu == 2) {
    #ifdef AdafruitDisplay
      display.setTextSize(0);
    #endif

    display.setCursor(10, 0);
    display.print(F("Mass (kg): "));
    display.print(KGServo);
  }
  else if (menu == 3) {
    //Загружаем список меню в данном пункте.
    #ifdef AdafruitDisplay
      display.setTextSize(0);
    #endif

    display.setCursor(10, 0);
    display.print(utf8rus(F("Сирена")));
    
    display.setCursor(10, 10);
    display.print(utf8rus(F("Обогреватель")));
    
    display.setCursor(10, 20);
    display.print(utf8rus(F("Увлажнитель")));
    
    display.setCursor(10, 30);
    display.print(utf8rus(F("Вентиляция")));

    display.setCursor(10, 40);
    display.print(F("Servo < >"));
    display.setCursor(10, 50);
    display.print(IncubateTime);
  }
}

void checkSensors() {
  //Функция для проверки статуса датчиков.
  int Bad = 0;

  //Проверка датчика влаги/температуры.
  if (!sensor.begin()) {
    if (Humidity > 101) {
      Bad++;
    } 
  }

  if (Bad > 0) {
    if (Warning != 1) {
      Warning = 1;
    }
  } else {
    if (systemEvent == 0) {
      Warning = 0;
    }
  }
}

void controlIncubator(float Temp, byte Hum) {

  float TempHesterezis = 0.15; //Допустимое отклонение температуры.
  byte HumHesterezis = 1; //Допустимое отклонение влажности.

  //Защита от перегрева
  if (Temperature > Temp+0.5) {
    HeaterError = 1;
    digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), LOW);
    VentingState = 1;
  } else if (HeaterError == 1 && Temperature < Temp - 0.1) {
    digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), HIGH);
    HeaterError = 0;
    VentingState = 0;
  }
  
  if (Temperature > Temp+1.0 && Temperature < 101.0) {
    HeaterBigError = 1;
    digitalWrite((byte)pgm_read_byte(&P_ReleySiren), LOW);
    SirenState = 1;
  } else if (HeaterBigError == 1 && Temperature <= Temp) {
    digitalWrite((byte)pgm_read_byte(&P_ReleySiren), HIGH);
    HeaterBigError = 0;
    SirenState = 0;
  }
  
  //Защита от повышенной влажности
  if (Humidity > Hum + 3) {
    HumidityError = 1;
    digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), LOW);
    VentingState = 1;
  } else if (HumidityError == 1 && Humidity <= Hum) {
    digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), HIGH);
    HumidityError = 0;
    VentingState = 0;
  }

  //Регулятор температуры.
  if (Temperature != Temp) {
    if (Temperature > Temp+TempHesterezis) {
      //Необходимо сделать короткое охлаждение.
      digitalWrite((byte)pgm_read_byte(&P_ReleyHeater), LOW);
      HeaterState = 0;
    } else if (Temperature < Temp) {
      //Необходимо начать нагрев, чтобы поднять температуру до установленной в программе.
      //А именно - включаем реле обогревателя.
      digitalWrite((byte)pgm_read_byte(&P_ReleyHeater), HIGH);
      HeaterState = 1;
      readSensors();
      if (statusLight() != true) {
        Warning = 1;
        digitalWrite((byte)pgm_read_byte(&P_ReleySiren), LOW);
        SirenState = 1;
      } else {
        Warning = 0;
        digitalWrite((byte)pgm_read_byte(&P_ReleySiren), HIGH);
        SirenState = 0;
      }
    }
  } 

  //Регулятор влажности.
  if (MoisturePercent > 1) {
    if (Humidity != Hum) {
      if (Humidity > Hum+HumHesterezis || VentingState == 1) {
        //Отключаем набор влажности.
        delay(600);
        digitalWrite((byte)pgm_read_byte(&P_ReleyHumidity), HIGH);
        HumidityState = 0;
      } else if (Humidity < Hum && VentingState == 0) {
        //Необходимо включить увлажнение воздуха.
        delay(600);
        digitalWrite((byte)pgm_read_byte(&P_ReleyHumidity), LOW);
        HumidityState = 1;
      }
    } else {
      //Мы достигли установленной влажности - отключаем увлажнитель.
      // digitalWrite(P_ReleyHumidity, HIGH);
      // HumidityState = 0;
    }
  } else {
    //Отключаем регулятор влажности. Закончилась вода.
    digitalWrite((byte)pgm_read_byte(&P_ReleyHumidity), HIGH);
    HumidityState = 0;
  }


}

void newDrobedTime(byte RotateCount, byte VentCount, unsigned long VentTime, unsigned long time) {
  unsigned long stepRotate;
  unsigned long stepVenting;

  float cycleRotate;
  float cycleVenting;

  if (RotateCount != 0) {
    stepRotate = 86400 / RotateCount;
    cycleRotate = (float)time / (float)stepRotate;
    int directionRotateByCycle = cycleRotate;
    for (int i = 0; cycleRotate >= 1.0; i++) {
      cycleRotate = cycleRotate - 1.0;
    }

    if (cycleRotate == 0.0 || cycleRotate == 1.0 || (cycleRotate > 0.0 && cycleRotate < 0.05)) {
      if (rotateOn == 0) {
        RotateServo(directionRotateByCycle % 2);
        resetRotate = millis()+(stepRotate*1000)-4000;
        rotateOn = 1;
      }
    }
  }

  if (VentCount != 0) {
    stepVenting = 86400 / VentCount;
    cycleVenting = (float)time / (float)stepVenting;
    for (int i = 0; cycleVenting >= 1.0; i++) {
      cycleVenting = cycleVenting - 1.0;
    }

    if (cycleVenting == 0.0 || cycleVenting == 1.0 || (cycleVenting > 0.0 && cycleVenting < 0.05)) {
      if (VentingState == 0 && ventingOn == 0) {
        digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), LOW);
        VentTime = VentTime*1000;
        stopVenting = (unsigned long)millis() + (unsigned long)VentTime;
        resetVenting = millis()+(stepVenting*1000)-4000;
        
        VentingState = 1;
        ventingOn = 1;
      }
    }
  }
  
}

void getRezhimeManual() {
  byte day = dayIncubationInt();

  float temperature = 37.8;
  byte humidity = 70;
  byte venting = 0;
  int ventingTime = 0;
  byte rotateCount = 0;

  // Температурный режим и влага.
  if (day < 7) {
    temperature = 38.0;
    humidity = 70;
  } else if (day < 25) {
    temperature = 37.8;
    humidity = 60;
  } else {
    temperature = 37.5;
    humidity = 90;
  }

  //Вентиляция.
  if (day >= 14 && day < 25) {
    ventingTime = 900;
    venting = 2;
  }

  // Для вращения яиц.
  if (day < 25) {
    rotateCount = 6;
  }

  // if (type == 0) {
  //   return temperature;
  // } else if (type == 1) {
  //   return humidity;
  // } else if (type == 2) {
  //   return venting;
  // } else if (type == 3) {
  //   return ventingTime;
  // } else if (type == 4) {
  //   return rotateCount;
  // }
  newDrobedTime((int)rotateCount, (int)venting, (int)ventingTime, timeDayEEPROM());
  controlIncubator((float)temperature, (byte)humidity);
}

void chickenRezhime() {
  getRezhimeManual();
  // byte day = dayIncubationInt(); //День инкубации.
  // byte kday = day;
  // if (kday >= 26) {
  //   kday = 26;
  // }



  // newDrobedTime((int)pgm_read_word(&chickens[kday].rotateCount), (int)pgm_read_word(&chickens[kday].ventingCount), (int)pgm_read_word(&chickens[kday].ventingTime), timeDayEEPROM());
  // controlIncubator((float)pgm_read_float(&chickens[kday].temperature), (byte)pgm_read_byte(&chickens[kday].humidity));
}



void loop() {
  #ifdef GyverEncoderOn
    enc.tick();
    encoderHandler(); //Обработчик энкодера.
  #endif

  now_time = millis();
  if (now_time >= timeoutSensors) {
    checkSensors();
    timeoutSensors = now_time + TimeoutSensors;
    readSensors();

    // if (Serial) {
      // Serial.println(dayIncubationInt());
      Serial.print(Temperature);
      Serial.print(F(" "));
      // Serial.println(Humidity);
      // Serial.print(F(" "));
      // Serial.println(percentWater());
    //   Serial.print(F(" "));
      Serial.println(HeaterState*10);
      // Serial.print(F(" "));
      // Serial.print(HumidityState*10);
      // Serial.print(F(" "));
      // Serial.println(SirenState*10);
      // Serial.print(F(" "));
      // Serial.println(VentingState*10);
    // }


    if (mode == 0 && manualEnable == 1) {
      newDrobedTime(ManualRotate, ManualVenting, 300, timeDay());
      controlIncubator(ManualTemp, ManualHum);
    }
    else if (mode == 1 && autoEnable == 1) {
      chickenRezhime();
    }
    // Blynk.virtualWrite(V0, millis());
    // Serial.println(millis());
  }



  now_time = millis();
  if (now_time >= systemOperations) {
    // Serial.println(freeRam());
    if (Warning == 1 || HeaterError == 1) {
      GreenL = 0;
      if (WarningState == 0) {
        RedL = 255;
        WarningState = 1;
      } else {
        RedL = 0;
        WarningState = 0;
      }
      if (HeaterError == 1) {
        digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), LOW);
      }
    } else {
      RedL = 0;
      GreenL = 24;
    }

    if (secondsDisplay > 0) {
      secondsDisplay--;
    }
    
    if (mode == 0) {
      BlueL = 64;
    } else {
      BlueL = 0;
    }

    if (Blink == 1) {
      Blink = 0;
      GreenL = 50;
      if (MoisturePercent <= 15) {
        RedL = 50;
        GreenL = 0;
      }
    } else {
      Blink = 1;
      GreenL = 255;
      if (WaterError == 1 && MoisturePercent > 60) {
          WaterError = 0;
          SirenState = 0;
          digitalWrite((byte)pgm_read_byte(&P_ReleySiren), HIGH);
      }
      if ((MoisturePercent < 55 && MoisturePercent > 15) || WaterError == 1) {
        RedL = 255;
        GreenL = 255;
      } else if (MoisturePercent <= 15 ) {
        RedL = 255;
        GreenL = 0;
        if (WaterError == 0 && VentingState == 0 && (autoEnable == 1 || manualEnable == 1)) {
          SirenState = 1;
          digitalWrite((byte)pgm_read_byte(&P_ReleySiren), LOW);
          WaterError = 1;
        }
      }
    }
    

    ledChangeColor(RedL, GreenL, BlueL);
    systemOperations = now_time + TimeoutOperations;
    TimeSecond++;
    wdt_reset();
  }

  now_time = millis();

  if (now_time >= stopVenting && (autoEnable == 1 || manualEnable == 1)) {
    digitalWrite((byte)pgm_read_byte(&P_ReleyVenting), HIGH);
    VentingState = 0;
    stopVenting = millis()*100;
    // Serial.println(F("Stop vent"));
  }
  
  if (now_time >= resetRotate && (autoEnable == 1 || manualEnable == 1)) { rotateOn = 0; resetRotate = millis()*100; }
  
  if (now_time >= resetVenting && (autoEnable == 1 || manualEnable == 1)) { ventingOn = 0; resetVenting = millis()*100; }

  //Закрываем всплывающее окно.
  if (now_time >= closePopup && Popup == 1 ) { Popup = 0; }

  #ifdef AdafruitDisplay
    display.clearDisplay();
  #endif

  #ifdef LightWeightDisplay
    display.clear();
  #endif 
  


  if (secondsDisplay > 0) {
    if (Popup != 0) {
      // display.setTextSize(0);
      // display.setCursor(10, 0);
      // display.print(utf8rus((String)pgm_read_word(&popupTexts[Popup])));
    } else {
      if (menu == 0) {
        if (cursorPosition == 0) { 
          //Если наше меню положение меню 0 рисуем обычный экран.
          defaultDisplay();
        } 
        else {
          drawCursorDisplay(cursorPosition);
        }
      } 
      else {
        drawMenuDisplay();
        drawMenuCursor();
      }
    }
  }
  #ifdef AdafruitDisplay
    display.display(); //Рисуем все наши данные на экране.
  #endif
}