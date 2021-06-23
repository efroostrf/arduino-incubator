# Arduino Nano (ATmega328) - Incubator
На данный момент документации нет, увы.

## Необходимые вещи для сборки
- Дисплей (OLED SSD1306);
- Encoder;
- Реле на 4 канала;
- Таймер времени;
- RGB светодиод (WS128B);
- Датчик влаги и температуры (SI7021);
- Датчик уровня воды (Capacitive Soil Moisture Sensor);
- Датчик уровня света (аналоговый);
- Пищалка (buzzer);
- Сервомашинка;
- 3 Компьютерных вентилятора любого размера (зависит от инкубатора).

## Подробнее

Цель разработки данного ПО — *сделать максимально эффективный и удобный контроллер для инкубатора*. 
На данный момент это сделать удалось и не удалось. Причина этому - объем памяти у Arduino Nano, как и Uno, Mega. Он слишком маленький, и его хватает только для поддержки работы OLED дисплея с русским текстом, энкодера.

>Заметка: библиотеки `Adafruit_SSD1306.h` и `GyverEncoder.h` занимают наибольший объем памяти.

Самый большой недостаток: `отсутствие логирования`.
Вы не можете отследить работу данного кода, потому что он ничего не записывает. Причина этому — отсутствие SD-модуля. Его можно было бы добавить, если бы было достаточное количество памяти у Ардуино. Поэтому, `проверяйте работу контроллера минимум 2 раза в день`.

Все проблемы можно решить двумя способами:
1. Отключить некоторые библиотеки, облегчить их, а после добавить новые;
2. Выбрать *другой* микроконтроллер.

## Подключение
В коде необходимо настроить следующие пины:
```cpp
//Цифровые пины
const byte P_LED = 8; //Светодиод
const byte P_Servo PROGMEM= 9; //Сервомашинка
const byte P_EncoderCLK = 10; //CLK Энкодера
const byte P_EncoderDT = 11; //DT Энкодера
const byte P_EncoderSW = 12; //SW Энкодера
const byte P_ReleyHeater PROGMEM = 5; //Реле нагревательного элемента
const byte P_ReleyHumidity PROGMEM = 4; //Реле для увлажнителя
const byte P_ReleyVenting PROGMEM = 3; //Реле для вентилятора проветривания
const byte P_ReleySiren PROGMEM = 2; //Реле для звуковой сирены

//Аналоговые пины
const byte P_Moisture = A0; //Датчик заполнености ёмкости водой
const byte P_Light = A1; //Датчик света
```
![N|Solid](https://raw.githubusercontent.com/efroostrf/arduino-incubator/main/schematic.jpg)

## `Реле нагревателя`
ОЧЕНЬ важно подключить нагреватель через `твердотельное` реле. Реле, указанное на картинке выше — при огромной нагрузке создаёт помехи в сети, из-за чего Arduino `зависает`. Такой сценарий развития событий предусмотрен через WatchDog, который перезагрузит ардуино в случае зависания. *Однако*, такие зависания влияют на таймеры в коде.
## Планы на будущее
Arduino, я считаю, не достаточно для такого проекта. Стоит посмотреть в сторону аналогов Arduino: NodeMCU, Teensy и другие, у которых памяти больше. 

Я же рекомендую `NodeMCU`. У данной платы большой объём памяти. Прошивку можно загружать как через Arduino IDE, так и через Platform.IO-. Есть встроенный WiFi модуль, с помощью которого можно обращаться к сети Интернет. Это очень удобно. Можно сделать логирование и управление в несколько кликов на сайте. 

> Для приведённого проекта инкубатора необходимо `11 цифровых` и `2 аналоговых` пина.

Но, спешу предупредить — у NodeMCU меньше как аналоговых, так и цифровых пинов. Это значит то, что необходимо больше подстраиваться под ваш инкубатор. Возможно, вам не нужен датчик света, так как вы не используете лампу накаливания в качестве нагревательного элемента. Значит 1 аналоговый или цифровой пин уже не будет занят. 
