// Main file for TFT display-related stuff
const byte EFFECT_NONE = 0;
const byte EFFECT_MAIN = 1;
const byte EFFECT_PERCENT=2;
const byte EFFECT_METERS=3;
const byte EFFECT_TEXT=4; 

struct TftParam {
  byte effect = EFFECT_MAIN;
  byte previousEffect = EFFECT_NONE;
  byte effectToRestore = EFFECT_NONE;
  int intValue;
  String strValue;
  bool backgroundRefreshNeeded = true;
};
timerMinim tftEffectTimer(2000);
TftParam screen;

void setEffect(byte effect) {
    screen.effect = effect;
    screen.backgroundRefreshNeeded = screen.effect != screen.previousEffect;
    if (isTemporaryEffect()) {
      // prolonging 
      tftEffectTimer.start();
    }
}

void tftTick() {
    if(screen.backgroundRefreshNeeded) {
      
      // Draw background
      switch (screen.effect) {
        case EFFECT_PERCENT: 
          //am.scale = 2; 
          //drawAnalogMeter(); 
          break;
        case EFFECT_METERS:
          //am.scale = 1;
          //metersDraw();
          break;
        case EFFECT_TEXT:

        break;
        default: tftClear(); 
      }
      if (isTemporaryEffect()) {
        screen.effectToRestore = screen.previousEffect;
      }
      screen.previousEffect = screen.effect;
      screen.backgroundRefreshNeeded = false;
    } // screen.backgroundRefreshNeeded
   

      // Refresh of current effect
      switch (screen.effect) {
        case EFFECT_PERCENT: 
          //plotNeedle(screen.intValue, 10);
          break;
        case EFFECT_METERS:
          //am.scale = 1;
          //metersRedraw();
          break;
        case EFFECT_TEXT:

          break;
        default: tftClear(); 
      }

      if (isTemporaryEffect() && tftEffectTimer.isReady()) {
        screen.effect = screen.effectToRestore;
        screen.backgroundRefreshNeeded = true;
      }
}


void tftClear() {
    //tft.setTextColor(TFT_BLACK);
}

boolean isTemporaryEffect() {
  return screen.effect == EFFECT_METERS;
}
