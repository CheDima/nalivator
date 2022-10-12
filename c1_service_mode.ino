
void serviceMode() {
  const ServoDriverSmooth srvs[4] = {srvPlatform, NULL, srvCenter, srvTop};
  if (!digitalRead(BTN_PIN)) {
    byte curSrv = 0;
    int servoPos = srvs[curSrv].getCurrentDeg();
    systemState = SERVICE;
    PRINTS("Entered service mode");
    while (!digitalRead(BTN_PIN));  // ждём отпускания
    delay(200);
    long pumpTime = 0;
    timerMinim timer100(100);
    //disp.displayInt(0);
    servosLock();
    bool flag;
    for (;;) {
      if (curSrv==1) srvBottom.tick(); else srvs[curSrv].tick(); // fuck inheritance, right?
      enc.tick();

      if (timer100.isReady()) {   // период 100 мс
        // работа помпы со счётчиком
        // if (!digitalRead(ENC_SW)) {
        //   if (flag) pumpTime += 100;
        //   else pumpTime = 0;
        //   //disp.displayInt(pumpTime);
        //   pumpON();
        //   flag = true;
        // } else {
        //   pumpOFF();
        //   flag = false;
        // }

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
        if (enc.isLeft()) {
          servoPos ++;
        }
        if (enc.isRight()) {
          servoPos --;
        }

        if (curSrv==1) {srvBottom.setTargetDeg(servoPos); srvBottom.setAutoDetach(false);} else {srvs[curSrv].setTargetDeg(servoPos); srvs[curSrv].setAutoDetach(false);}
        PRINT("srv ", curSrv);
        PRINT("pos \n", servoPos);
      }
      if (enc.isClick()) {
        curSrv = curSrv==3?0:curSrv+1;
        servoPos = curSrv==1 ? srvBottom.getCurrentDeg() : srvs[curSrv].getCurrentDeg();
        PRINT("srv ", curSrv);
        PRINT("pos \n", servoPos);
       
      }

      if (btn.holded()) {
        break;
      }
    }
  }
  tftClear();
}
