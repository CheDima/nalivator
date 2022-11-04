// triggers once on click, setup, rotate (not tick)
void displayStatus() {
  if (!autoMode) pumpOFF();
}

// наливайка, опрос кнопок
void flowTick() {
    for (byte currShot = 0; currShot < NUM_SHOTS; currShot++) {
      bool switchPresssed = !digitalRead(SW_pins[currShot]) ^ SWITCH_LEVEL;
      if (switchPresssed && shotStates[currShot] == NO_GLASS) {  // поставили пустую рюмку
        servosLock(); // will be locked until it's parked
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
          waitTimer.reset();
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
        shotStates[curPumping] = IN_PROCESS;              
        srvPlatform.setTargetDeg(shotPos[curPreset][curPumping][Servos::PLATFORM] + compensateCCW());
        srvBottom.setTargetDeg(shotPos[curPreset][curPumping][Servos::BOTTOM]);
        srvCenter.setTargetDeg(shotPos[curPreset][curPumping][Servos::CENTER]);
        srvTop.setTargetDeg(shotPos[curPreset][curPumping][Servos::TOP]);
        //servosLock();
        dispRefresh(DisplayMode::MOVING);
        PRINTS("Empty glass found. Moving");
        break;
      }
    }
    if (noGlass && !parked) {                            // no glasses to fill, PARKING
        srvPlatform.setTargetDeg(parkingPos[curPreset][Servos::PLATFORM]);
        srvBottom.setTargetDeg(parkingPos[curPreset][Servos::BOTTOM]);
        srvCenter.setTargetDeg(parkingPos[curPreset][Servos::CENTER]);
        srvTop.setTargetDeg(parkingPos[curPreset][Servos::TOP]);
        bool rotorReady = srvPlatform.tick();
        bool servoBottomReady = srvBottom.tick(); 
        bool servoCenterReady = srvCenter.tick();
        bool servoTopReady = srvTop.tick();
        if (rotorReady && servoBottomReady && servoCenterReady && servoTopReady) {
          systemON = false;
          parked = true;
          //servosRelease();
          PRINTS("no glass");        
        }
    }
  } else if (systemState == MOVING) {
      PRINTS("Moving to target position");
      bool rotorReady = srvPlatform.tick();
      bool servoBottomReady = srvBottom.tick(); 
      bool servoCenterReady = srvCenter.tick();
      bool servoTopReady = srvTop.tick();
      PRINTD(srvPlatform.getCurrentDeg());
      if (rotorReady && servoBottomReady && servoCenterReady && servoTopReady) {
        systemState = PUMPING;
        flowTimer.setInterval((long)thisVolume * time50ml / 50);
        flowTimer.reset();
        pumpON();
        strip.leds[curPumping] = mYellow;
        strip.show();
        dispRefresh(DisplayMode::PUMPING);
        PRINTS("filling glass#");
        PRINTD(curPumping);
      }
  } else if (systemState == PUMPING) {        
    if (flowTimer.isReady()) {                            // если налили (таймер)
      pumpOFF();                                          // помпа выкл
      shotStates[curPumping] = READY;                     // налитая рюмка, статус: готов
      strip.leds[curPumping] =  mLime;
      strip.show();
      dispRefresh(DisplayMode::MAIN_SCREEN);
      curPumping = -1;                                    // снимаем выбор рюмки
      systemState = WAIT;                                 // режим работы - ждать
      waitTimer.reset();
      PRINTS("wait");
    }
  } else if (systemState == WAIT) {

    if (waitTimer.isReady()) {                            // подождали после наливания
      systemState = SEARCH;
      timeoutReset();
      PRINTS("search");
    }
  }
}


// отрисовка светодиодов по флагу (100мс)
void LEDtick() {
  if (LEDchanged && ledTimer.isReady()) {
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

