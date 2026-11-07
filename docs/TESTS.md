# Лабораторный журнал: Arduino CLI / Bluepad32 / геймпады

**Дата:** 07.11.2026. **ОС:** Windows, PowerShell. **Порт:** COM3. **Проект:** `E:\quadro\G910_Test`.

## 1. Компьютер увидел ESP32

В Диспетчере устройств: **Silicon Labs CP210x USB to UART Bridge (COM3)**. `arduino-cli board list` показывал COM3 как Serial Port (USB), без автоматической модели платы — это нормально.

```text
Arduino CLI: 1.5.1
ESP32 Bluepad32 core: 4.1.0
ESP32 Espressif core (отдельный): 3.3.12
FQBN: esp32-bluepad32:esp32:esp32
Hardware: ESP32-D0WDQ6 rev v1.0, Bluetooth Classic, 240MHz
Serial: COM3, 115200 baud
```

## 2. Установка, компиляция, прошивка

Первое скачивание инструмента `xtensa-esp32-elf-gcc8_4_0-esp-2021r2-patch5-win64.zip` было прервано сетевой ошибкой `wsarecv` при доступе к GitHub Release. **Повторная установка `esp32-bluepad32:esp32@4.1.0` завершилась успешно.**

```powershell
arduino-cli core install esp32-bluepad32:esp32@4.1.0
arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 "E:\quadro\G910_Test"
arduino-cli upload -p COM3 --fqbn esp32-bluepad32:esp32:esp32 "E:\quadro\G910_Test"
arduino-cli monitor -p COM3 -c baudrate=115200
```

Компиляция прошла с отчётом `Global variables: 87044 bytes (26%), 240636 bytes free`. Первые две заливки завершились временными ошибками `PermissionError(13)` по COM3. **Третья заливка прошла**, в том числе запись `717904 bytes`, `Hash of data verified`, `Hard resetting via RTS pin`. Отсюда вывод: доступ к последовательному порту может иногда конфликтовать, но прошивка на плату записана и проверена.

## 3. MOBASE G910 — Bluetooth есть, режим HID проблемный

```text
BTstack up and running
=== GoSHa Drone ===
Waiting for gamepad...
G910: CONNECTED
Model: Keyboard
```

ESP32 видит G910 по Bluetooth, но библиотека определяет его как **Keyboard**. Старый тест содержит условие `isGamepad()`, поэтому оси в этом режиме не выводятся. Вывод: проблему не решали, G910 пока не выбран как пульт.

## 4. ARDOR GAMING Vigour — Nintendo Switch mode работает!

Из пользовательского лога:

```text
Failed to inquiry name for ..., using a fake one
uni_hid_device_disconnect: Unknown GAP connection type: 0
G910: CONNECTED
Model: Switch Pro
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
LX:512 LY:512 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
LX:-512 LY:-41 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
LX:9 LY:-41 RX:512 RY:-512 Buttons:0x0000 DPad:0x00
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0100 DPad:0x00
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0001 DPad:0x00
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0002 DPad:0x00
```

- **Режим геймпада:** Switch / Nintendo (в эксперименте запускался сочетанием HOME + X).
- **Bluepad32 модель:** `Switch Pro`; следовательно, `isGamepad()` выполняется.
- **Четыре оси считываются:** LX/LY/RX/RY. Полный наблюдаемый диапазон приблизительно **−512…+512**.
- **Нейтраль (пример):** LX 9, LY −41, RX −23, RY 0. Это отклонение центра, нужна индивидуальная калибровка.
- **Кнопки:** встречаются три маски `0x0100`, `0x0001`, `0x0002` — соответствие физическим A/B/X/Y **ещё не установлено**.
- `DPad:0x00` в предоставленном отрывке; полноценный тест крестовины **не подтверждён**.
- Отладочная строка **«G910: CONNECTED»** зашита в скетч и не означает подключение G910 во второй сессии.
- Системные сообщения `Failed to inquiry name` / `Unknown GAP connection type` не помешали установлению соединения. Их причину ещё не диагностировали.

**Вывод:** канал **ARDOR Vigour → Bluetooth → ESP32** продемонстрирован реально. Радиосвязь **ESP32 → ZY-860R** не проверялась.

## 5. Что нужно улучшить

1. Заменить старую строку G910 на универсальное имя контроллера.
2. Ограничить частоту диагностического вывода, печатать только изменения.
3. Привязать конкретные кнопки к битовым маскам.
4. Добавить deadzone/центрирование четырёх осей.
5. До любого полёта: газ на нуле при старте, арминг только в безопасном положении, таймаут связи и аппаратное отключение моторов.

Скетч без управления моторами: [../firmware/G910_Test/G910_Test.ino](../firmware/G910_Test/G910_Test.ino).