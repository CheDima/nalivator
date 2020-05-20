#include "Sprites.h"
#include "LinkedList.h"
#include "macros.h"

#define DELAYTIME 30

enum EffectType {SPRITE, SYMBOL, PERCENT, RUNNING_STRING, ANIMATION, ANIMATION_LOOP} ;

struct EffectParam {
  char symbol;
  int value;
  uint8_t* sprites[7];
  LinkedList<uint8_t*> spritesList;
  bool isLooped;
};

class MatrDisplayDevice {
  public:
    virtual void displayBitmap(uint8_t* bmp) const;
    virtual void clear() const;
    virtual void displayChar(char ch) const;
    virtual void displayPercent(int val) const;
};

class MatrEffect {
  public:
    MatrEffect( EffectType t, EffectParam param);
    MatrEffect();
    void refresh(MatrDisplayDevice* mx);
    void doRefresh();
    static MatrEffect sprite(Sprite sp);
    static MatrEffect symbol(char ch);
    static MatrEffect percent(int val);
    static MatrEffect animation(uint8_t* sprts[], byte arrLen, bool isLooped = false);
    EffectType type;
  private :
    uint16_t timeout;
    bool needsRefresh = true;
    EffectParam param;
};

class MatrDisplay {
  public:
//    void runningString(char* p);
    void brightness(int val);
    void clear();
    void setEffect(MatrEffect ef);
    void runTempEffect(MatrEffect ef, uint32_t timeout, bool forceRefresh = false);
    void tick();
    MatrDisplay(MatrDisplayDevice* concreteDevice);
  protected:
   MatrEffect mainEffect;
   MatrEffect tempEffect;
   uint32_t tempTimeout;
   uint32_t tempStartTime = 0;
   MatrDisplayDevice* mx;
};

MatrDisplay::MatrDisplay(MatrDisplayDevice* concreteDevice) {
  mx = concreteDevice;
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
      tempEffect.refresh(mx);
    } else {
      tempStartTime = 0;
      mainEffect.doRefresh();
    }
  } else {
    mainEffect.refresh(mx);
  }
}

void MatrDisplay::clear() {
  mx->clear();
}

void MatrDisplay::brightness(int val) {
  //TODO
}

//void MatrDisplay::runningString(char *p) {
//  uint8_t charWidth;
//  uint8_t cBuf[8];  // this should be ok for all built-in fonts
//  mx.clear();
//  while (*p != '\0')
//  {
//    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
//
//    for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
//    {
//      mx.transform(MD_MAX72XX::TSL);
//      if (i < charWidth)
//        mx.setColumn(0, cBuf[i]);
//      delay(DELAYTIME);
//    }
//  }
//}

MatrEffect::MatrEffect() {}

MatrEffect MatrEffect::animation(uint8_t* sprts[], byte arrLen, bool isLooped) {
  LinkedList<uint8_t*> ll;
  for (byte i = 0; i < arrLen; i++) {
    ll.Append(*sprts);
    sprts++;
  }
  EffectParam par;
  par.spritesList = ll;
  par.spritesList.moveToStart();
  par.isLooped = isLooped;
  return MatrEffect(ANIMATION, par);
}

MatrEffect MatrEffect::percent(int val) {
  EffectParam par;
  par.value = val;
  return MatrEffect(PERCENT, par);
}

MatrEffect MatrEffect::sprite(Sprite sp) {
  EffectParam par;
  par.sprites[0] = sp;
  return MatrEffect(SPRITE, par);
}

MatrEffect MatrEffect::symbol(char ch) {
  EffectParam par;
  par.symbol = ch;
  return MatrEffect(SYMBOL, par);
}


void MatrEffect::doRefresh() {
  needsRefresh = true;
}

//TODO
//void displayInt(int val) {
//  char buffer [3];
//  runningString(itoa(val, buffer, 10));
//}

MatrEffect::MatrEffect(EffectType t, EffectParam p) {
  param = p;
  type = t;
}

void MatrEffect::refresh(MatrDisplayDevice* mx) {

  if(needsRefresh) {
   if(type == ANIMATION) {
     EVERY_MS(500) {
        mx -> displayBitmap(param.spritesList.getCurrent());
      if (!param.spritesList.next()) {
        needsRefresh = param.isLooped;
        param.spritesList.moveToStart();
      } else {
        needsRefresh = true;
      }
     }
   }
  switch(type) {
    case(SPRITE):
      mx -> displayBitmap(param.sprites[0]);
      needsRefresh = false;
      break;
    case(SYMBOL):
      needsRefresh = false;
      break;
    case(PERCENT):
      mx -> displayPercent(param.value);
      needsRefresh = false;
      break;
   }
  }
}
