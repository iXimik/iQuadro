// iQuadro / GoSHa Drone -- BLE/Bluetooth gamepad diagnostics ONLY
// Verified hardware: ESP32-D0WDQ6; core: esp32-bluepad32:esp32@4.1.0
// Used with MOBASE G910 and ARDOR Vigour in Switch mode.
// IMPORTANT: No motors, no RF control, no automatic arming.

#include <Bluepad32.h>

ControllerPtr gamepad = nullptr;

void onConnected(ControllerPtr ctl) {
  if (gamepad == nullptr) {
    gamepad = ctl;
    Serial.println("G910: CONNECTED");
    Serial.printf("Model: %s\n", ctl->getModelName().c_str());
  }
}

void onDisconnected(ControllerPtr ctl) {
  if (gamepad == ctl) {
    gamepad = nullptr;
    Serial.println("G910: DISCONNECTED");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== GoSHa Drone ===");
  Serial.println("Waiting for gamepad...");
  BP32.setup(&onConnected, &onDisconnected);
  BP32.enableVirtualDevice(false);
}

void loop() {
  bool updated = BP32.update();
  if (updated && gamepad != nullptr &&
      gamepad->isConnected() && gamepad->isGamepad()) {
    Serial.printf(
      "LX:%ld LY:%ld RX:%ld RY:%ld "
      "Buttons:0x%04x DPad:0x%02x\n",
      (long)gamepad->axisX(),
      (long)gamepad->axisY(),
      (long)gamepad->axisRX(),
      (long)gamepad->axisRY(),
      gamepad->buttons(),
      gamepad->dpad()
    );
  }
  delay(30);
}
