void setup() {
#if (DEBUG == 1)
  Serial.begin(9600);
  PRINTS("start");
#endif
  // епром
  if (EEPROM.read(1000) != 10) {
    EEPROM.write(1000, 10);
    EEPROM.put(0, thisVolume);
  }
  EEPROM.get(0, thisVolume);

  // тыкаем ленту
  strip.setBrightness(130);
  strip.clear();
  strip.show();
  PRINTS("strip init");

  // настройка пинов
  pinMode(PUMP_POWER, 1);
  pinMode(SERVO_POWER, 1);
  for (byte i = 0; i < NUM_SHOTS; i++) {
    if (SWITCH_LEVEL == 0) pinMode(SW_pins[i], INPUT_PULLUP);
  }
  // настройка серво
  servoON();
  servo.attach(SERVO_PIN, 600, 2400);
  if (INVERSE_SERVO) servo.setDirection(REVERSE);
   
  servo.write(0);
  delay(800);
  servo.setTargetDeg(0);
  servo.setSpeed(60);
  servo.setAccel(0.3);
  servoOFF();
  PRINTS("Servo initialized");
  serviceMode();    // калибровка
  timeoutReset();   // сброс таймаута
  TIMEOUTtimer.start();
}
