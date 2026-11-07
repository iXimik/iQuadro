# Прошивка ESP32 под Windows / PowerShell

**Проверенная конфигурация:** ESP32-WROOM-32 (`ESP32-D0WDQ6`), Silicon Labs CP210x (`COM3`), Arduino CLI `1.5.1`, пакет `esp32-bluepad32:esp32 4.1.0`, `E:\quadro`.

## Установка платформы

```powershell
arduino-cli version
arduino-cli board list
arduino-cli core list

arduino-cli config add board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
arduino-cli config add board_manager.additional_urls "https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json"
arduino-cli core update-index

# В эксперименте сработала фиксированная версия:
arduino-cli core install esp32-bluepad32:esp32@4.1.0
arduino-cli core list
```

Обычное ядро `esp32:esp32` (без Bluepad32) **не заменяет** специальный пакет Bluepad32. Подробности: https://bluepad32.readthedocs.io/en/latest/plat_arduino/

## Папки

В ранних локальных испытаниях исходник лежал в `E:\quadro\G910_Test`. В репозитории он перенесён в `firmware/G910_Test` без изменения экспериментальной логики.

Открой PowerShell в корне клона:

```powershell
cd E:\quadro
arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 .\firmware\G910_Test
arduino-cli upload -p COM3 --fqbn esp32-bluepad32:esp32:esp32 .\firmware\G910_Test
arduino-cli monitor -p COM3 -c baudrate=115200
```

Для выхода из Serial Monitor: `Ctrl+C`. Если пусто — `EN/RESET` на плате. При подключении Vigour в `HOME + X` ожидается `Model: Switch Pro` и движение чисел осей.

## Ошибки, которые реально встретились

1. `Platform 'esp32-bluepad32:esp32' not found` — пакет ещё не установлен.
2. Сеть оборвала GitHub скачивание xtensa gcc (`wsarecv`) — повторная установка той же версии помогла.
3. `PermissionError(13) Cannot configure port` — COM3 временно недоступен или порт захвачен. Закрыть другие мониторы, переподключить USB, проверить Диспетчер устройств и повторить. Последняя повторная запись прошла успешно.
4. В PowerShell **не вставлять C++ по строкам**: строки `#include`, `void setup()` и `ControllerPtr` — это исходник `.ino`, а не команды оболочки.
5. Если `G910: CONNECTED` и `Model: Switch Pro` — это именно *ARDOR*, поскольку первая строка исторически жёстко зашита в коде.

**Не включать моторы с пропеллерами ради «быстрой проверки».**
