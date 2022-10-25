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
    dispRefresh(DisplayMode::PERCENTAGE);
  }
}

void btnTick() {
  if (btn.clicked()) {
    if (curPreset) curPreset--; else curPreset++;
    textToScroll = curPreset ? "Glass" : "Shotik";
    dispRefresh(DisplayMode::SCROLL_TEXT);
  }
  if (btn.holded()) {
    autoMode = !autoMode;
    textToScroll = autoMode ? "Mode: A" : "Mode: M";
    dispRefresh(DisplayMode::SCROLL_TEXT);
  }
  if (encBtn.holded()) {
    pumpON();
    while (!digitalRead(ENC_SW));
    pumpOFF();
  }  
}
