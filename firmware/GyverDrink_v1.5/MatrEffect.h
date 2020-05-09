#include <MD_MAX72xx.h>
#include "LinkedList.h"
#include "macros.h"

enum EffectType {SPRITE, SYMBOL, PERCENT, RUNNING_STRING, ANIMATION, ANIMATION_LOOP} ;
struct EffectParam {
  char symbol;
  int value;
  uint8_t* sprites[7];
  LinkedList<uint8_t*> spritesList;
  bool isLooped;
};

class MatrEffect {
  public:
    MatrEffect( EffectType t, EffectParam param);
    MatrEffect();
    void refresh(MD_MAX72XX *device);
    void doRefresh();
    static MatrEffect sprite(Sprite sp);
    static MatrEffect symbol(char ch);
    static MatrEffect percent(int val);
    static MatrEffect animation(uint8_t* sprts[], byte arrLen, bool isLooped = false);
    EffectType type;
  protected:
    uint16_t timeout;
    MD_MAX72XX *mx;
    bool needsRefresh = true;
    EffectParam param;
};

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

void MatrEffect::refresh(MD_MAX72XX *mx) {

  if(needsRefresh) {
   if(type == ANIMATION) {
     EVERY_MS(500) {
      for (uint8_t i=0; i<8; i++) mx->setRow(i, param.spritesList.getCurrent()[i]);
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
      for (uint8_t i=0; i<8; i++) mx->setRow(i, param.sprites[0][i]);
      needsRefresh = false;
      break;
    case(SYMBOL):
      mx->clear();
      mx->setChar(7, param.symbol);
      needsRefresh = false;
      break;
    case(PERCENT):
      mx->clear();
      byte dotsNo = 64 * param.value / 100;
      for (uint8_t col=0; col<8; col++) {  
        for (uint8_t row=0; row<8; row++) {    
          mx->setPoint(7-col, 7-row, col * 8 + row <= dotsNo);
        }
      }
      needsRefresh = false;
      break;
   }
  }
}
