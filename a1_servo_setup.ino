#define SRV_PLATFORM_PIN 0
#define SRV_BOTTOM_R_PIN 1
#define SRV_BOTTOM_L_PIN 2
#define SRV_CENTER_PIN 3
#define SRV_TOP_PIN 4
#define SRV_PUMP_PIN 5

#define SERVO_ACCELERATION 0.3
#define SERVO_SPEED 130
#define BOTTOM_L_SERVO_CORRECTION 73

enum Servos {PLATFORM, BOTTOM, CENTER, TOP};
const byte parkingPos[NUM_PRESETS][4] = {{20, 150, 192, 85}, {20, 131, 31, 230}} ; 
// 2  3
// 1  4
//   PLTFRM

// Platform: the more - the lefter
// Bottom: the more - the more vertical
// Center: the more - the lower
// Top: the more - the closer to base
const byte shotPos[NUM_PRESETS][NUM_SHOTS][4] = {
  {{133, 108, 130, 139}, {93, 90, 93, 141}, {32, 93, 100, 148}, {32, 120, 160, 115}}, // shots
  {{133, 110, 50, 220}, {93, 85, 18, 200}, {32, 90, 30, 210}, {32, 130, 65, 210}}  // glasses
};

ServoDriverSmooth srvPlatform(0x40, 270);
ServoDriverSmooth srvCenter(0x40, 270);
ServoDriverSmooth srvTop(0x40, 300);
ServoDriverSmooth srvPump(0x40);

class ServoWrapper  {
  ServoDriverSmooth left = ServoDriverSmooth(0x40, 270);
  ServoDriverSmooth right = ServoDriverSmooth(0x40, 270);
  public:
    ServoWrapper();
    void attach(byte pinR, byte pinL, byte parkPosition) const ;
    void setSpeed(int speed) const ;
    void setAccel(double accel) const ;
    void setTargetDeg(int angle) const ;
    int getCurrentDeg() const ;
    bool tick() const;
    void setAutoDetach(bool detach); 
};
ServoWrapper::ServoWrapper() {
}
void ServoWrapper::attach(byte pinR, byte pinL, byte parkPosition) const {
  right.attach(pinR, parkPosition);
  right._servo.setPWMFreq(50);
  left.attach(pinL, parkPosition + BOTTOM_L_SERVO_CORRECTION);
  left._servo.setPWMFreq(50);
  right.setDirection(false);
  left.setDirection(true); // reversed
  
  left.tick();

}
void ServoWrapper::setSpeed(int speed) const {
  right.setSpeed(speed);
  left.setSpeed(speed);
}
void ServoWrapper::setAccel(double accel) const {
  right.setAccel(accel);
  left.setAccel(accel);
}
void ServoWrapper::setTargetDeg(int angle) const {
  right.setTargetDeg(angle);
  left.setTargetDeg(angle + BOTTOM_L_SERVO_CORRECTION);
}
bool ServoWrapper::tick() const {
  bool rt = right.tick();
  bool lt = left.tick();
  return rt && lt;
}
int ServoWrapper::getCurrentDeg() const {
  return right.getCurrentDeg();
}
void ServoWrapper::setAutoDetach(bool isReleased) {
  left.setAutoDetach(isReleased);
  right.setAutoDetach(isReleased);
}

ServoWrapper srvBottom;

void servoSetup() {
  srvPlatform.attach(SRV_PLATFORM_PIN, parkingPos[curPreset][Servos::PLATFORM]);
  srvBottom.attach(SRV_BOTTOM_R_PIN, SRV_BOTTOM_L_PIN, parkingPos[curPreset][Servos::BOTTOM]);
  srvCenter.attach(SRV_CENTER_PIN, parkingPos[curPreset][Servos::CENTER]);
  srvTop.attach(SRV_TOP_PIN, parkingPos[curPreset][Servos::TOP]);
  srvPump.attach(SRV_PUMP_PIN, 500, 2500);
  srvPlatform._servo.setPWMFreq(50);
  srvCenter._servo.setPWMFreq(50);
  srvTop._servo.setPWMFreq(50);
  srvPump._servo.setPWMFreq(50);

  srvTop.setDirection(true); srvTop.tick();

  srvPlatform.setSpeed(160);
  srvPlatform.setAccel(0.6);
  srvBottom.setSpeed(SERVO_SPEED);
  srvBottom.setAccel(SERVO_ACCELERATION);
  srvCenter.setSpeed(160);
  srvCenter.setAccel(0.6);
  srvTop.setSpeed(SERVO_SPEED);
  srvTop.setAccel(SERVO_ACCELERATION);
  srvPlatform.tick();
  srvBottom.tick();
  srvCenter.tick();
  srvTop.tick();

  PRINTS("Servo initialized");
}

int8_t compensateCCW() {
  return shotPos[curPreset][curPumping][Servos::PLATFORM] < srvPlatform.getCurrentDeg() ? -10 : 0;
}