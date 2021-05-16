void setup() {
//#if (DEBUG == 1)
  Serial.begin(38400);
  PRINTS("start");
//#endif
  if (EEPROM.read(1000) != 10) {
    EEPROM.write(1000, 10);
    EEPROM.put(0, thisVolume);
  }
  EEPROM.get(0, thisVolume);

  strip.setBrightness(130);
  strip.clear();
  strip.show();
  PRINTS("strip init");

  tft.init();
  tft.setRotation(1);
  tftClear();
  
  analogMeter();

  // Pump and servos
  pinMode(PUMP_POWER, 1);
  for (byte i = 0; i < NUM_SHOTS; i++) {
    if (SWITCH_LEVEL == 0) pinMode(SW_pins[i], INPUT_PULLUP);
  }
  if (SWITCH_LEVEL == 0) pinMode(BTN_PIN, INPUT_PULLUP);
  // настройка серво
  //servoON();
  rotator_servo.attach(ROTATING_PIN);
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  rotator_servo.setSpeed(SERVO_SPEED);
  rotator_servo.setAccel(SERVO_ACCELERATION);
  servo1.setSpeed(SERVO_SPEED);
  servo1.setAccel(SERVO_ACCELERATION);
  servo2.setSpeed(SERVO_SPEED);
  servo2.setAccel(SERVO_ACCELERATION);
  servo3.setSpeed(SERVO_SPEED);
  servo3.setAccel(SERVO_ACCELERATION);
  
  rotator_servo.write(parkingPosition[Servos::ROTOR]);
  rotator_servo.setCurrentDeg(parkingPosition[Servos::ROTOR]); 
  servo3.write(parkingPosition[Servos::LEG3]);
  servo3.setCurrentDeg(parkingPosition[Servos::LEG3]);
  servo2.write(parkingPosition[Servos::LEG2]);
  servo2.setCurrentDeg(parkingPosition[Servos::LEG2]);
  servo1.write(parkingPosition[Servos::LEG1]);
  servo1.setCurrentDeg(parkingPosition[Servos::LEG1]);

  
  //servoOFF();
  PRINTS("Servo initialized");
  serviceMode();    // калибровка
  dispMode();       // выводим на дисплей стандартные значения
  timeoutReset();   // сброс таймаута
  TIMEOUTtimer.start();
  PRINTS("Init finished");
}
