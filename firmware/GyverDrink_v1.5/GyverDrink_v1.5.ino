// ======== НАСТРОЙКИ ========
#define NUM_SHOTS 4       // количество рюмок (оно же кол-во светодиодов и кнопок!)
#define TIMEOUT_OFF 5     // таймаут на выключение (перестаёт дёргать привод), минут
#define SWITCH_LEVEL 0    // кнопки 1 - высокий сигнал при замыкании, 0 - низкий
#define INVERSE_SERVO 0   // инвертировать направление вращения серво

// 3  2
// 4  1
// положение серво над центрами рюмок
// rotor: 0 - 180 ccw
// LIFT: 70 (lowest) to 125 (highest)
// FORWARD: 40 (short) to 120 (longest)
const byte shotPos[NUM_SHOTS][3] = {{135, 120, 120}, {150, 100, 60}, {100, 100, 50}, {100, 120, 110}};

enum Servos {ROTOR, LIFT, FORWARD};
const byte UPPER_POSITION = 160;
const byte parkingPosition[3] = {10, UPPER_POSITION, 40};  // See Servos enum




// время заполнения 50 мл
const long time50ml = 5500;

#define KEEP_POWER 0    // 1 - система поддержания питания ПБ, чтобы он не спал

// =========== ПИНЫ ===========
#define PUMP_POWER 4
#define PLATFORM_PIN 7 //Rotating servo
#define FORWARD_PIN 6  //Left lever servo
#define UPDOWN_PIN 0   //Right lever servo
#define SERVO_ACCELERATION 0.1
#define SERVO_SPEED 20
#define LED_PIN 3
#define BTN_PIN 2

#define ENC_SW 12
#define ENC_DT 9
#define ENC_CLK 8
#define DISP_CLK_PIN   13  // or SCK
#define DISP_DATA_PIN  11  // or MOSI
#define DISP_CS_PIN    10  // or SS
const byte SW_pins[] = {A0, A1, A2, A3, A4, A5};

// =========== ЛИБЫ ===========
#include <ServoSmooth.h>
#include <microLED.h>
#include <EEPROM.h>
#include "encUniversalMinim.h"
#include "buttonMinim.h"
#include "timer2Minim.h"
#include "MatrDisplay.h"

// =========== ДАТА ===========
#define COLOR_DEBTH 2   // цветовая глубина: 1, 2, 3 (в байтах)
LEDdata leds[NUM_SHOTS];  // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED strip(leds, NUM_SHOTS, LED_PIN); 

MatrDisplay disp;

// пин clk, пин dt, пин sw, направление (0/1), тип (0/1)
encMinim enc(ENC_CLK, ENC_DT, ENC_SW, 1, 1);

ServoSmooth rotator_servo;
ServoSmooth lift_servo;
ServoSmooth forward_servo;

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
bool workMode = false;  // 0 manual, 1 auto
int thisVolume = 50;
bool systemON = false;
bool timeoutState = false;
bool volumeChanged = false;
bool parking = false;

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
