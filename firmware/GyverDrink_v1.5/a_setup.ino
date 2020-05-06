void setup() {
#if (DEBUG == 1)
  Serial.begin(9600);
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

  // Pump and servos
  pinMode(PUMP_POWER, 1);
  for (byte i = 0; i < NUM_SHOTS; i++) {
    if (SWITCH_LEVEL == 0) pinMode(SW_pins[i], INPUT_PULLUP);
  }
  if (SWITCH_LEVEL == 0) pinMode(BTN_PIN, INPUT_PULLUP);
  // настройка серво
  //servoON();
  rotator_servo.attach(PLATFORM_PIN, 600, 2400);
  lift_servo.attach(UPDOWN_PIN, 600, 2400);
  forward_servo.attach(FORWARD_PIN, 600, 2400);
  rotator_servo.setSpeed(SERVO_SPEED);
  rotator_servo.setAccel(SERVO_ACCELERATION);
  lift_servo.setSpeed(SERVO_SPEED);
  lift_servo.setAccel(SERVO_ACCELERATION);
  forward_servo.setSpeed(SERVO_SPEED);
  forward_servo.setAccel(SERVO_ACCELERATION);

  rotator_servo.write(parkingPosition[Servos::ROTOR]);
  rotator_servo.setCurrentDeg(parkingPosition[Servos::ROTOR]); 
  forward_servo.write(parkingPosition[Servos::FORWARD]);
  forward_servo.setCurrentDeg(parkingPosition[Servos::FORWARD]);
  lift_servo.write(parkingPosition[Servos::LIFT]);
  lift_servo.setCurrentDeg(parkingPosition[Servos::LIFT]);

  
  //servoOFF();
  PRINTS("Servo initialized");
  serviceMode();    // калибровка
  dispMode();       // выводим на дисплей стандартные значения
  timeoutReset();   // сброс таймаута
  TIMEOUTtimer.start();
  PRINTS("Init finished");
}
