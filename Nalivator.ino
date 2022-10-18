
#define  DEBUG  1

#define NUM_SHOTS 4       // количество рюмок (оно же кол-во светодиодов и кнопок!)
#define SWITCH_LEVEL 0    // кнопки 1 - высокий сигнал при замыкании, 0 - низкий

const long time50ml = 3000;

//====================== PINS =======================
const byte SW_pins[] = {30, 32, 34, 36};
#define LED_PIN 38

// Display pins
#define TFT_CS         42
#define TFT_RST        46
#define TFT_DC         44

#define BTN_PIN 40
#define ENC_SW  43
#define ENC_DT  45
#define ENC_CLK 41


#include <ServoDriverSmooth.h>
#include <microLED.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "lib_encUniversalMinim.h"
#include "lib_buttonMinim.h"
#include "lib_timer2Minim.h"

microLED<NUM_SHOTS, LED_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB> strip;
Adafruit_ST7735 disp = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
encMinim enc(ENC_CLK, ENC_DT, ENC_SW, 1, 1);
buttonMinim btn(BTN_PIN);
buttonMinim encBtn(ENC_SW);
timerMinim ledTimer(100);
timerMinim flowTimer(2000);
timerMinim waitTimer(400);

enum {NO_GLASS, EMPTY, IN_PROCESS, READY} shotStates[NUM_SHOTS];
enum {SEARCH, MOVING, WAIT, PUMPING, SERVICE} systemState;
enum {STRAIGHTENING, STRAIGHTENED} systemSubState;

bool LEDchanged = false;
bool pumping = false;
int8_t curPumping = -1;
bool autoMode = true;  // 0 manual, 1 auto
int thisVolume = 50;
bool systemON = false;
bool timeoutState = false;
bool volumeChanged = false;
bool parked = false;

// =========== MACRO ===========
#define pumpON() srvPump._servo.setPWM(SRV_PUMP_PIN, 4096, 0)
#define pumpOFF() srvPump._servo.setPWM(SRV_PUMP_PIN, 0, 4096)

#define servosLock() {srvBottom.setAutoDetach(false); srvBottom.tick(); srvCenter.setAutoDetach(false);srvCenter.tick(); srvTop.setAutoDetach(false);srvTop.tick();}
#define servosRelease() {srvBottom.setAutoDetach(true); srvBottom.tick(); srvCenter.setAutoDetach(true); srvCenter.tick(); srvTop.setAutoDetach(true);srvTop.tick();}

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.println(F(x))
#define PRINTD(x) Serial.println(x, DEC)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)
#endif
