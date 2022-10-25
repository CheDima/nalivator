void setup() {
  #if (DEBUG == 1)
    Serial.begin(38400);
    PRINTS("start");
  #endif
  if (EEPROM.read(1000) != 10) {
    EEPROM.write(1000, 10);
    EEPROM.put(0, thisVolume);
  }
  EEPROM.get(0, thisVolume);

  strip.setBrightness(130);
  strip.clear();
  strip.show();
  PRINTS("strip init");


  for (byte i = 0; i < NUM_SHOTS; i++) {
    if (SWITCH_LEVEL == 0) pinMode(SW_pins[i], INPUT_PULLUP);
  }
  if (SWITCH_LEVEL == 0) pinMode(BTN_PIN, INPUT_PULLUP);
  displaySetup();
  servoSetup();
  pumpOFF();
  serviceMode();
  displayStatus();       // выводим на дисплей стандартные значения
  PRINTS("Init finished");
}
