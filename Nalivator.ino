
#define  DEBUG  1

#define NUM_SHOTS 4       // количество рюмок (оно же кол-во светодиодов и кнопок!)
#define NUM_PRESETS 2
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
#define ENC_SW  41
#define ENC_DT  45
#define ENC_CLK 43

#include <MD_MAX72xx.h>
#include <ServoDriverSmooth.h>
#include <microLED.h>
#include <EEPROM.h>
#include <SPI.h>
#include "lib_encUniversalMinim.h"
#include "lib_buttonMinim.h"
#include "lib_timer2Minim.h"

microLED<NUM_SHOTS, LED_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB> strip;
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

encMinim enc(ENC_CLK, ENC_DT, ENC_SW, 1, 1);
buttonMinim btn(BTN_PIN);
buttonMinim encBtn(ENC_SW);
timerMinim ledTimer(100);
timerMinim flowTimer(2000);
timerMinim waitTimer(400);
timerMinim matrTimer(11000);
timerMinim funTimer(60000);

enum {NO_GLASS, EMPTY, IN_PROCESS, READY} shotStates[NUM_SHOTS];
enum {SEARCH, MOVING, WAIT, PUMPING, SERVICE} systemState;
enum Presets {SHOT, GLASS};
enum {STRAIGHTENING, STRAIGHTENED} systemSubState;
enum class DisplayMode {MAIN_SCREEN, SCROLL_TEXT, PERCENTAGE, PUMPING, MOVING, PACMAN, SPECTRUM, INVADERS};
void dispRefresh(DisplayMode mode, int intervalMs );

bool LEDchanged = false;
bool pumping = false;
int8_t curPumping = -1;
bool autoMode = true;  // 0 manual, 1 auto
int thisVolume = 50;
bool systemON = false;
bool timeoutState = false;
bool volumeChanged = false;
bool parked = true;
int8_t curPreset = Presets::SHOT;

// =========== MACRO ===========
#define pumpON() srvPump._servo.setPWM(SRV_PUMP_PIN, 4096, 0)
#define pumpOFF() srvPump._servo.setPWM(SRV_PUMP_PIN, 0, 4096)

#define servosLock() {srvBottom.setAutoDetach(false); srvBottom.tick(); srvCenter.setAutoDetach(false);srvCenter.tick(); srvTop.setAutoDetach(false);srvTop.tick();}
#define servosRelease() {srvBottom.setAutoDetach(true); srvBottom.tick(); srvCenter.setAutoDetach(true); srvCenter.tick(); srvTop.setAutoDetach(true);srvTop.tick();}

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.println(F(x))
#define PRINTD(x) Serial.println(x, DEC)
#define PRINTC(s, v)  { Serial.print(F(s)); Serial.print((char)v); }  // Print a string followed by a value (char)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)
#define PRINTC(s, v)
#endif
