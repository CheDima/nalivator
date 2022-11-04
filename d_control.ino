// кнопки-крутилки

void encTick() {
  enc.tick();
  if (enc.isTurn()) {
    volumeChanged = true;
    timeoutReset();
    if (enc.isLeft()) {
      thisVolume += 5;
      thisVolume = constrain(thisVolume, 5, 100);
    }
    if (enc.isRight()) {
      thisVolume -= 5;
      thisVolume = constrain(thisVolume, 5, 100);
    }
    dispRefresh(DisplayMode::PERCENTAGE, 1500);
  }
}

void btnTick() {
  if (btn.clicked()) {
    if (curPreset) curPreset--; else curPreset++;
    textToScroll = curPreset ? "Glass" : "Shocik";
    dispRefresh(DisplayMode::SCROLL_TEXT, 5000);
    srvPlatform.setTargetDeg(parkingPos[curPreset][Servos::PLATFORM]);
    srvBottom.setTargetDeg(parkingPos[curPreset][Servos::BOTTOM]);
    srvCenter.setTargetDeg(parkingPos[curPreset][Servos::CENTER]);
    srvTop.setTargetDeg(parkingPos[curPreset][Servos::TOP]);
    parked = false;
    servosLock();
  }
  if (btn.holded()) {
    autoMode = !autoMode;
    textToScroll = autoMode ? "Mode: Auto" : "Mode: Manual";
    dispRefresh(DisplayMode::SCROLL_TEXT, 7000);
  }
  if (encBtn.holded()) {
    pumpON();
    while (!digitalRead(ENC_SW));
    pumpOFF();
  }  
}
