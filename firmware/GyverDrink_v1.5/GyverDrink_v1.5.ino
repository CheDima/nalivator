// ======== НАСТРОЙКИ ========
#define NUM_SHOTS 4       // количество рюмок (оно же кол-во светодиодов и кнопок!)
#define TIMEOUT_OFF 5     // таймаут на выключение (перестаёт дёргать привод), минут
#define SWITCH_LEVEL 0    // кнопки 1 - высокий сигнал при замыкании, 0 - низкий
#define INVERSE_SERVO 0   // инвертировать направление вращения серво

// 3  2
// 4  1
// положение серво над центрами рюмок
// rotor: 0 - 180 ccw
// LEG1: 70 (lowest) to 125 (highest)
// LEG2: 40 (short) to 120 (longest)
const byte shotPos[NUM_SHOTS][4] = {{180, 10, 120, 90}, {153, 0, 60, 90}, {115, 0, 50, 90}, {115, 10, 110, 90}};

enum Servos {ROTOR, LEG1, LEG2, LEG3};
const byte UPPER_POSITION = 70;
const byte parkingPosition[4] = {0, 0, 0, 90};  // See Servos enum




// время заполнения 50 мл
const long time50ml = 1000;

#define KEEP_POWER 0    // 1 - система поддержания питания ПБ, чтобы он не спал

//Board pins
const byte SW_pins[] = {2, 3, 4, 5};
#define LED_PIN 6

// CONTROL pins
#define BTN_PIN 8
#define ENC_SW 9
#define ENC_DT 10
#define ENC_CLK 11

//MAIN pins
#define PUMP_POWER 14
#define ROTATING_PIN 15 //Rotating servo
#define SERVO1_PIN 16  //servos, from rotor to up
#define SERVO2_PIN 17
#define SERVO3_PIN 18

#define SERVO_ACCELERATION 0.8

#define SERVO_SPEED 500


#include <ServoSmooth.h>
#include <microLED.h>
#include <EEPROM.h>
#include "encUniversalMinim.h"
#include "buttonMinim.h"
#include "timer2Minim.h"
#include "Sprites.h"
#include <TFT_HX8357.h> 

microLED<NUM_SHOTS, LED_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB> strip;

TFT_HX8357 tft = TFT_HX8357(); 
  
encMinim enc(ENC_CLK, ENC_DT, ENC_SW, 1, 1);

ServoSmooth rotator_servo;
ServoSmooth servo1;
ServoSmooth servo2;
ServoSmooth servo3;

buttonMinim btn(BTN_PIN);
buttonMinim encBtn(ENC_SW);
timerMinim LEDtimer(100);
timerMinim FLOWdebounce(20);
timerMinim FLOWtimer(2000);
timerMinim WAITtimer(400);
timerMinim TIMEOUTtimer(15000);   // таймаут дёргания приводом
timerMinim POWEROFFtimer(TIMEOUT_OFF * 60000L);

bool LEDchanged = false;
bool pumping = false;
int8_t curPumping = -1;

enum {NO_GLASS, EMPTY, IN_PROCESS, READY} shotStates[NUM_SHOTS];
enum {SEARCH, MOVING, WAIT, PUMPING} systemState;
enum {STRAIGHTENING, STRAIGHTENED} systemSubState;
bool workMode = true;  // 0 manual, 1 auto
int thisVolume = 50;
bool systemON = false;
bool timeoutState = false;
bool volumeChanged = false;
bool parked = false;

// =========== МАКРО ===========
#define pumpON() digitalWrite(PUMP_POWER, 1)
#define pumpOFF() digitalWrite(PUMP_POWER, 0)

#define  DEBUG  1

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.println(F(x))
#define PRINTD(x) Serial.println(x, DEC)

#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)

#endif
