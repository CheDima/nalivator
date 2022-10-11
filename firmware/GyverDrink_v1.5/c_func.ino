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
      srvPlatform.tick();
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
            strip.leds[i] =  mGreen;
          } else {
            strip.leds[i] =  mBlack;
          }
          strip.show();
        }
      }

      if (enc.isTurn()) {
        pumpTime = 0;
        if (enc.isLeft()) {
          servoPos += 5;
        }
        if (enc.isRight()) {
          servoPos -= 5;
        }
        servoPos = constrain(servoPos, 0, 180);
        //disp.displayInt(servoPos);
        srvPlatform.setTargetDeg(servoPos);
      }

      if (btn.holded()) {
        srvPlatform.setTargetDeg(0);
        break;
      }
    }
  }
  tftClear();
}

// triggers once on click, setup, rotate (not tick)
void displayStatus() {
  if (!autoMode) pumpOFF();

  setEffect(EFFECT_PERCENT);
  screen.intValue = thisVolume;
  //disp.setEffect(MatrEffect::sprite(autoMode ? autoMode : manualMode));
}

// наливайка, опрос кнопок
void flowTick() {
    for (byte currShot = 0; currShot < NUM_SHOTS; currShot++) {
      bool switchPresssed = !digitalRead(SW_pins[currShot]) ^ SWITCH_LEVEL;
      if (switchPresssed && shotStates[currShot] == NO_GLASS) {  // поставили пустую рюмку
        servosLock(); // will be locked until it's parked
        if (!autoMode) {
            //setEffect(EFFECT_METERS);
        }
        //timeoutReset();
        shotStates[currShot] = EMPTY;
        strip.leds[currShot] =  mRed;
        LEDchanged = true;
      }
      if (!switchPresssed && shotStates[currShot] != NO_GLASS) {   // убрали пустую/полную рюмку
        shotStates[currShot] = NO_GLASS;
        strip.leds[currShot] =  mBlack;
        LEDchanged = true;
        //timeoutReset();
        if (currShot == curPumping) {
          curPumping = -1; // снимаем выбор рюмки
          systemState = WAIT;
          WAITtimer.reset();
          pumpOFF();
        }
        PRINTS("glass taken");
      }
    }

    if (autoMode) {         // авто
      flowRoutine();        // крутим отработку кнопок и поиск рюмок
    } else {                // ручной
      if (btn.clicked()) {  // клик!
        systemON = true;    // система активирована
        timeoutReset();     // таймаут сброшен
      }
      if (systemON) flowRoutine();  // если активны - ищем рюмки и всё такое
    }
}

// поиск и заливка
void flowRoutine() {
  if (systemState == SEARCH) {
    bool noGlass = true;
    for (byte currShot = 0; currShot < NUM_SHOTS; currShot++) {
      if (shotStates[currShot] == EMPTY && currShot != curPumping) {    
        noGlass = false;                                  // we have a glass to fill
        parked = false;
        curPumping = currShot;                      
        systemState = MOVING;
        systemSubState = STRAIGHTENING;       // we must not touch neighbour glasses                    
        shotStates[curPumping] = IN_PROCESS;              
        srvPlatform.setTargetDeg(shotPos[curPumping][Servos::PLATFORM]);
        srvBottom.setTargetDeg(shotPos[curPumping][Servos::BOTTOM]);
        srvCenter.setTargetDeg(shotPos[curPumping][Servos::CENTER]);
        srvTop.setTargetDeg(shotPos[curPumping][Servos::TOP]);
        PRINTS("Empty glass found. Pumping");
        //setEffect(EFFECT_METERS);
        break;
      }
    }
    if (noGlass && !parked) {                            // no glasses to fill, PARKING
      if (straightened()) {
        srvPlatform.setTargetDeg(parkingPos[Servos::PLATFORM]);
        srvBottom.setTargetDeg(parkingPos[Servos::BOTTOM]);
        srvCenter.setTargetDeg(parkingPos[Servos::CENTER]);
        srvTop.setTargetDeg(parkingPos[Servos::TOP]);
        if (srvPlatform.tick() && srvCenter.tick() && srvCenter.tick()) {
          systemON = false;
          parked = true;
          servosRelease();
          PRINTS("no glass");        
        }
      }
    }
  } else if (systemState == MOVING) {
    if (straightened()) {
      srvBottom.setTargetDeg(shotPos[curPumping][Servos::BOTTOM]);
      bool rotorInPlace = srvPlatform.tick();
      if (!rotorInPlace) {
        return;
      }
      bool servoBottomReady = srvBottom.tick(); 
      bool servoCenter2Ready = srvCenter.tick();
      bool servoTopReady = srvTop.tick();
      PRINTD(srvPlatform.getCurrentDeg());
      if (servoBottomReady && servoCenter2Ready && servoTopReady) {
        systemState = PUMPING;
        FLOWtimer.setInterval((long)thisVolume * time50ml / 50);
        FLOWtimer.reset();
        pumpON();
        
        strip.leds[curPumping] = mYellow;
        strip.show();
        PRINTS("filling glass#");
        PRINTD(curPumping);
      }
    }

  } else if (systemState == PUMPING) {        
    setEffect(EFFECT_METERS);
    if (FLOWtimer.isReady()) {                            // если налили (таймер)
      systemSubState = STRAIGHTENING;
      pumpOFF();                                          // помпа выкл
      shotStates[curPumping] = READY;                     // налитая рюмка, статус: готов
      strip.leds[curPumping] =  mLime;
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

bool straightened() {
        //servoON();
      if (systemSubState == STRAIGHTENED) {
        return true;
      } else {
         srvBottom.setTargetDeg(UPPER_POSITION);
         PRINTS("Moving to vertical position");
         if (srvBottom.tick()) {
          systemSubState = STRAIGHTENED;
          return true;
         }
      }
      return false;
}

// отрисовка светодиодов по флагу (100мс)
void LEDtick() {
  if (LEDchanged && LEDtimer.isReady()) {
    LEDchanged = false;
    strip.show();
  }
}

void displayTick() {
  tftTick();
}

 // сброс таймаута
 void timeoutReset() {
//   //if (!timeoutState) disp.brightness(7);
//   timeoutState = true;
//   TIMEOUTtimer.reset();
//   TIMEOUTtimer.start();
//   PRINTS("timeout reset");
}
 
// // сам таймаут
// void timeoutTick() {
//   if (systemState == SEARCH && timeoutState && TIMEOUTtimer.isReady()) {
//     PRINTS("timeout");
//     timeoutState = false;
//     //disp.brightness(1);
//     if (volumeChanged) {
//       volumeChanged = false;
//       EEPROM.put(0, thisVolume);
//     }
//   }
// }

