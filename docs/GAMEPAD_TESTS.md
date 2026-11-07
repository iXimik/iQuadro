# Дневник геймпадов / Bluetooth — 2026-11-07

## Плата и стек

- Windows, `COM3` = Silicon Labs CP210x.
- Arduino CLI `1.5.1`.
- `esp32:esp32 3.3.12` и `esp32-bluepad32:esp32 4.1.0`.
- ESP32-D0WDQ6, rev v1.0, Bluetooth Classic, кристалл 40 МГц.
- `firmware/G910_Test/G910_Test.ino` — диагностический скетч; двигатели и радио не подключены.

### Сборка и загрузка

```text
arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 "E:\quadro\G910_Test"
Глобальные переменные используют 87044 байт (26%) динамической памяти...
```

При установке Bluepad32 сначала была ошибка `wsarecv` при загрузке компилятора xtensa с GitHub (сетевой разрыв). Повторная установка `esp32-bluepad32:esp32@4.1.0` завершилась успешно. При загрузке первые две попытки давали `PermissionError(13)` для COM3, третья **прошла полностью**:

```text
Chip is ESP32-D0WDQ6 (revision v1.0)
Writing at 0x000bf0c4... (100 %)
Wrote 717904 bytes (442981 compressed) at 0x00010000
Hash of data verified.
Hard resetting via RTS pin...
```

Это подтверждает прошивку; сетевые сбои и ошибки COM3 не были ошибкой C++-кода.

## Геймпад №1 — G910 (проект iSmartCar)

Использовался ранее на Raspberry Pi 3B+ через Bluetooth/`evdev`. В этом эксперименте ESP32 увидела:

```text
Bluepad32 ... v4.1.0
BTstack up and running
=== GoSHa Drone ===
Waiting for gamepad...
G910: CONNECTED
Model: Keyboard
```

**Подтверждено:** Bluetooth-подключение. **Не подтверждено:** данные осей в этом режиме. В исходнике вывод осей разрешён лишь для `gamepad->isGamepad()`, а модель пришла как `Keyboard`. Это не обязательно дефект геймпада. Длинная последовательность шестнадцатеричных байтов перед сообщением — HID report descriptor.

## Геймпад №2 — ARDOR GAMING Vigour

На фотографии маркировка: `Геймпад Модель/Vigour Ardor`, питание 5 В для зарядки. На задней панели `Normal/Custom` и два переключателя хода триггеров.

Испытанный режим: **Nintendo Switch / HOME + X**, `Normal`. Монитор выдал:

```text
Failed to inquiry name for ..., using a fake one
uni_hid_device_disconnect: Unknown GAP connection type: 0
G910: CONNECTED
Model: Switch Pro
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
```

Предупреждения до `CONNECTED` не помешали установлению соединения и обмену данными. Лейбл `G910: CONNECTED` — **строка из скетча**, а фактически `Switch Pro`.

Стики уверенно доходят до **−512…+512**:

```text
LX:9 LY:-512 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
LX:512 LY:512 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
LX:-512 LY:-41 RX:-23 RY:0 Buttons:0x0000 DPad:0x00
LX:9 LY:-41 RX:-512 RY:512 Buttons:0x0000 DPad:0x00
LX:9 LY:-41 RX:512 RY:-512 Buttons:0x0000 DPad:0x00
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0100 DPad:0x00
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0001 DPad:0x00
LX:9 LY:-41 RX:-23 RY:0 Buttons:0x0002 DPad:0x00
```

Центр стиков в одном из измерений: `LX=9 LY=-41 RX=-23 RY=0`. Не считать это аппаратной неисправностью без калибровки. Кнопки есть, но **соответствие физических A/B/X/Y битам ещё не записано**. DPad в присланном отрывке остаётся 0.

## Следующий безопасный тест

- Уменьшить частоту печати в Serial.
- Просканировать отдельными нажатиями каждую кнопку и составить таблицу кодов.
- Для каждой оси записать центр/минимум/максимум и применить deadzone.
- При старте вход газа должен считаться выключенным, даже если стик находится в центре.
- При потере Bluetooth-связи сразу переводить будущую команду газа в ноль.
- **Ни одного теста с вращением пропеллеров, пока не доказан радиопротокол и failsafe.**
