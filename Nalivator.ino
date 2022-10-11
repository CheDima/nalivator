
#define  DEBUG  1

// ======== НАСТРОЙКИ ========
#define NUM_SHOTS 4       // количество рюмок (оно же кол-во светодиодов и кнопок!)
#define TIMEOUT_OFF 5     // таймаут на выключение (перестаёт дёргать привод), минут
#define SWITCH_LEVEL 0    // кнопки 1 - высокий сигнал при замыкании, 0 - низкий

// время заполнения 50 мл
const long time50ml = 3000;

//Board pins
const byte SW_pins[] = {2, 3, 4, 5};
#define LED_PIN 6

// Display pins
#define TFT_CS        10
#define TFT_RST        8 
#define TFT_DC         9

// CONTROL pins
#define BTN_PIN 8
#define ENC_SW 19
#define ENC_DT 21
#define ENC_CLK 20


#include <ServoDriverSmooth.h>
#include <microLED.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "encUniversalMinim.h"
#include "buttonMinim.h"
#include "timer2Minim.h"

microLED<NUM_SHOTS, LED_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB> strip;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
encMinim enc(ENC_CLK, ENC_DT, ENC_SW, 1, 1);
buttonMinim btn(BTN_PIN);
buttonMinim encBtn(ENC_SW);
timerMinim LEDtimer(100);
timerMinim FLOWtimer(2000);
timerMinim WAITtimer(400);


bool LEDchanged = false;
bool pumping = false;
int8_t curPumping = -1;

enum {NO_GLASS, EMPTY, IN_PROCESS, READY} shotStates[NUM_SHOTS];
enum {SEARCH, MOVING, WAIT, PUMPING} systemState;
enum {STRAIGHTENING, STRAIGHTENED} systemSubState;
bool autoMode = true;  // 0 manual, 1 auto
int thisVolume = 50;
bool systemON = false;
bool timeoutState = false;
bool volumeChanged = false;
bool parked = false;

// =========== МАКРО ===========
#define pumpON() srvPump.setCurrent(2500)
#define pumpOFF() srvPump.setCurrent(500)

#define servosLock() {srvPlatform.setAutoDetach(false); srvBottomL.setAutoDetach(false); srvBottomR.setAutoDetach(false); srvCenter.setAutoDetach(false); srvTop.setAutoDetach(false);}
#define servosRelease() {srvPlatform.setAutoDetach(true); srvBottomL.setAutoDetach(true); srvBottomR.setAutoDetach(true); srvCenter.setAutoDetach(true); srvTop.setAutoDetach(true);}

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.println(F(x))
#define PRINTD(x) Serial.println(x, DEC)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)
#endif
