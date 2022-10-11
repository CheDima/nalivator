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
    displayStatus();
  }
}

void btnTick() {
  if (btn.holded()) {
    autoMode = !autoMode;
    displayStatus();
  }
  if (encBtn.holded()) {
    pumpON();
    while (!digitalRead(ENC_SW));
    pumpOFF();
  }  
}
