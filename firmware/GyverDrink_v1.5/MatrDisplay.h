#include <MD_MAX72xx.h>
#include "Sprites.h"
#include "MatrEffect.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 2
#define DELAYTIME 30
#define PRINT(s, x) { Serial.print(F(s)); Serial.println(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTD(x) Serial.println(x, DEC)

class MatrDisplay {
  public:
    void runningString(char* p);
    void brightness(int val);
    void clear();
    void setEffect(MatrEffect ef);
    void runTempEffect(MatrEffect ef, uint32_t timeout, bool forceRefresh = false);
    void tick();
    MatrDisplay();
  protected:
   MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DISP_CS_PIN, MAX_DEVICES);
   MatrEffect mainEffect;
   MatrEffect tempEffect;
   uint32_t tempTimeout;
   uint32_t tempStartTime = 0;
};

MatrDisplay::MatrDisplay() {
  mx.begin();
}

void MatrDisplay::setEffect(MatrEffect ef) {
  mainEffect = ef;
}

void MatrDisplay::runTempEffect(MatrEffect ef, uint32_t timeoutMs, bool forceRefresh = false) {
  if (!forceRefresh && ef.type == tempEffect.type && tempStartTime) {
    // This effect is active currently, let it work on
    tempStartTime = millis();
    return;
  }
  tempEffect = ef;
  tempTimeout = timeoutMs;
  tempStartTime = millis();
  
}

void MatrDisplay::tick() {
  if (tempStartTime > 0) {
    if (millis() - tempStartTime < tempTimeout) {
      tempEffect.refresh(&mx);
    } else {
      tempStartTime = 0;
      mainEffect.doRefresh();
    }
  } else {
    mainEffect.refresh(&mx);
  }
}

void MatrDisplay::clear() {
  mx.clear();
}

void MatrDisplay::brightness(int val) {
  //TODO
}

void MatrDisplay::runningString(char *p) {
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts
  mx.clear();
  while (*p != '\0')
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

    for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      if (i < charWidth)
        mx.setColumn(0, cBuf[i]);
      delay(DELAYTIME);
    }
  }
}
