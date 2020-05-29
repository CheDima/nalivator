// различные функции

void serviceMode() {
  if (!digitalRead(BTN_PIN)) {
  PRINTS("Entered service mode");
//    disp.runningString("Service");
    while (!digitalRead(BTN_PIN));  // ждём отпускания
    delay(200);
    int servoPos = 0;
    long pumpTime = 0;
    timerMinim timer100(100);
    //disp.displayInt(0);
    bool flag;
    for (;;) {
      rotator_servo.tick();
      enc.tick();

      if (timer100.isReady()) {   // период 100 мс
        // работа помпы со счётчиком
        if (!digitalRead(ENC_SW)) {
          if (flag) pumpTime += 100;
          else pumpTime = 0;
          //disp.displayInt(pumpTime);
          pumpON();
          flag = true;
        } else {
          pumpOFF();
          flag = false;
        }

        // зажигаем светодиоды от кнопок
        for (byte i = 0; i < NUM_SHOTS; i++) {
          if (!digitalRead(SW_pins[i])) {
            strip.setLED(i, mCOLOR(GREEN));
          } else {
            strip.setLED(i, mCOLOR(BLACK));
          }
          strip.show();
        }
      }

      if (enc.isTurn()) {
        // крутим серво от энкодера
        pumpTime = 0;
        if (enc.isLeft()) {
          servoPos += 5;
        }
        if (enc.isRight()) {
          servoPos -= 5;
        }
        servoPos = constrain(servoPos, 0, 180);
        //disp.displayInt(servoPos);
        rotator_servo.setTargetDeg(servoPos);
      }

      if (btn.holded()) {
        rotator_servo.setTargetDeg(0);
        break;
      }
    }
  }
  disp.clear();
}

// выводим объём и режим
void dispMode() {
  if (!workMode) pumpOFF();

  disp.runTempEffect(MatrEffect::percent(thisVolume), 1500, true);
  disp.setEffect(MatrEffect::sprite(workMode ? autoMode : manualMode));
}

// наливайка, опрос кнопок
void flowTick() {
  if (FLOWdebounce.isReady()) {
    for (byte i = 0; i < NUM_SHOTS; i++) {
      bool swState = !digitalRead(SW_pins[i]) ^ SWITCH_LEVEL;
      if (swState && shotStates[i] == NO_GLASS) {  // поставили пустую рюмку

        if (!workMode) { // TODO: refactor блять, SOLID rules  
          uint8_t* sprites[] = {smile3, smile2, smile1};
          disp.runTempEffect(MatrEffect::animation(sprites, 3), 3000);
        }
        
        timeoutReset();
        shotStates[i] = EMPTY;
        strip.setLED(i, mCOLOR(RED));
        LEDchanged = true;
        //PRINTS("set glass");
        //PRINTS(i);
      }
      if (!swState && shotStates[i] != NO_GLASS) {   // убрали пустую/полную рюмку
        shotStates[i] = NO_GLASS;
        strip.setLED(i, mCOLOR(BLACK));
        LEDchanged = true;
        timeoutReset();
        if (i == curPumping) {
          curPumping = -1; // снимаем выбор рюмки
          systemState = WAIT;                                       // режим работы - ждать
          WAITtimer.reset();
          pumpOFF();
        }
        //PRINTS("take glass");
        //PRINTS(i);
      }
    }

    if (workMode) {         // авто
      flowRoutnie();        // крутим отработку кнопок и поиск рюмок
    } else {                // ручной
      if (btn.clicked()) {  // клик!
        systemON = true;    // система активирована
        timeoutReset();     // таймаут сброшен
      }
      if (systemON) flowRoutnie();  // если активны - ищем рюмки и всё такое
    }
  }
}

// поиск и заливка
void flowRoutnie() {
  if (systemState == SEARCH) {                            // если поиск рюмки
    bool noGlass = true;
    for (byte i = 0; i < NUM_SHOTS; i++) {
      if (shotStates[i] == EMPTY && i != curPumping) {    // поиск
        noGlass = false;                                  // флаг что нашли хоть одну рюмку
        parking = false;
        curPumping = i;                                   // запоминаем выбор
        moveToUppermost();                                // we must not touch neighbour glasses
        systemState = MOVING;                             
        shotStates[curPumping] = IN_PROCESS;              
        rotator_servo.setTargetDeg(shotPos[curPumping][Servos::ROTOR]);
        lift_servo.setTargetDeg(shotPos[curPumping][Servos::LIFT]);
        forward_servo.setTargetDeg(shotPos[curPumping][Servos::FORWARD]);
        PRINTS("find glass");
        break;
      }
    }
    if (noGlass && !parking) {                            // если не нашли ни одной рюмки
      //servoON();
      rotator_servo.setTargetDeg(parkingPosition[Servos::ROTOR]);
      lift_servo.setTargetDeg(parkingPosition[Servos::LIFT]);
      forward_servo.setTargetDeg(parkingPosition[Servos::FORWARD]);
      if (rotator_servo.tick() && forward_servo.tick()) {
        systemON = false;
        parking = true;
        PRINTS("no glass");        
      }
    }
  } else if (systemState == MOVING) {
    bool rotorCame = rotator_servo.tick();
    bool forwardCame = forward_servo.tick();
    if (rotorCame && forwardCame) {                       // если приехали
      systemState = PUMPING;                              // режим - наливание
      FLOWtimer.setInterval((long)thisVolume * time50ml / 50);  // перенастроили таймер
      FLOWtimer.reset();                                  // сброс таймера
      pumpON();                                           // НАЛИВАЙ!
      strip.setLED(curPumping, mCOLOR(YELLOW));           // зажгли цвет
      strip.show();
      PRINTS("fill glass");
      //PRINTS(curPumping);
    }

  } else if (systemState == PUMPING) {        
    uint8_t* sprites[] = {glass1, glass2, glass3};
    disp.runTempEffect(MatrEffect::animation(sprites, 3, true), 3000);
    //rotator_servo.write(rotator_servo.getTargetDeg());    // Будем поддерживать ротор, так как он тяжелый и его легко сдвинуть во время наливания, особенно если платформа стоит криво
    if (FLOWtimer.isReady()) {                            // если налили (таймер)
      pumpOFF();                                          // помпа выкл
      shotStates[curPumping] = READY;                     // налитая рюмка, статус: готов
      strip.setLED(curPumping, mCOLOR(LIME));             // подсветили
      strip.show();
      curPumping = -1;                                    // снимаем выбор рюмки
      systemState = WAIT;                                 // режим работы - ждать
      WAITtimer.reset();
      PRINTS("wait");
    }
  } else if (systemState == WAIT) {

    if (WAITtimer.isReady()) {                            // подождали после наливания
      systemState = SEARCH;
      timeoutReset();
      PRINTS("search");
    }
  }
}

void moveToUppermost() {
  PRINTS("Move to upper position");
//  lift_servo.setTargetDeg(UPPER_POSITION);
  //  while (!lift_servo.tick()); 
}

// отрисовка светодиодов по флагу (100мс)
void LEDtick() {
  if (LEDchanged && LEDtimer.isReady()) {
    LEDchanged = false;
    strip.show();
  }
}

void displayTick() {
  disp.tick();
}

// сброс таймаута
void timeoutReset() {
  if (!timeoutState) disp.brightness(7);
  timeoutState = true;
  TIMEOUTtimer.reset();
  TIMEOUTtimer.start();
  PRINTS("timeout reset");
}
 
// сам таймаут
void timeoutTick() {
  if (systemState == SEARCH && timeoutState && TIMEOUTtimer.isReady()) {
    PRINTS("timeout");
    timeoutState = false;
    disp.brightness(1);
    POWEROFFtimer.reset();
    jerkServo();
    if (volumeChanged) {
      volumeChanged = false;
      EEPROM.put(0, thisVolume);
    }
  }

  // дёргаем питание серво, это приводит к скачку тока и powerbank не отключает систему
  if (!timeoutState && TIMEOUTtimer.isReady()) {
    if (!POWEROFFtimer.isReady()) {   // пока не сработал таймер полного отключения
      jerkServo();
    } else {
      disp.clear();
    }
  }
}

void jerkServo() {
  if (KEEP_POWER) {
    disp.brightness(7);
    //servoON();
    rotator_servo.attach();
    rotator_servo.write(random(0, 4));
    delay(200);
    rotator_servo.detach();
    //servoOFF();
    disp.brightness(1);
  }
}
